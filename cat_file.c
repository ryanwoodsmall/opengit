#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <fcntl.h>
#include "cat_file.h"
#include "common.h"
#include "ini.h"

static struct option long_options[] =
{
	{NULL, 0, NULL, 0}
};

int
cat_file_usage(int type)
{
	fprintf(stderr, "usage: git cat-file (-t [--allow-unknown-type] | -s [--allow-unknown-type] | -e | -p | <type> | --textconv | --filters) [--path=<path>] <object>\n");
	fprintf(stderr, "\n");
	exit(type);
}

void
cat_file_type(char *sha) {
	// Find the sha or fail
	DIR *d;
	struct dirent *dir;
	char packdir[PATH_MAX];
	char idxfile[PATH_MAX];
	int idxfilefd;
	char *file_ext;
	char *idxmap;
	struct stat sb;

	printf("Comes here\n");

	sprintf(packdir, "%s/objects/pack", dotgitpath);
	d = opendir(packdir);

	if (d) {
		while((dir = readdir(d)) != NULL) {
			file_ext = strrchr(dir->d_name, '.');
			if (!file_ext || strncmp(file_ext, ".idx", 4))
				continue;
			sprintf(idxfile, "%s/objects/pack/%s", dotgitpath, dir->d_name);

			idxfilefd = open(idxfile, O_RDONLY);
			fstat(idxfilefd, &sb);
			idxmap = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, idxfilefd, 0);

			if (idxmap == NULL) {
				fprintf(stderr, "mmap(2) error, exiting.\n");
				exit(0);
			}
			close(idxfilefd);

			pack_find_sha(sha, idxmap);

			munmap(idxmap, sb.st_size);
		}
	}

}

int
cat_file_main(int argc, char *argv[])
{
	int ret = 0;
	int ch;
	int q = 0;
	char *sha;

	uint8_t flags = 0;

	argc--; argv++;

	while((ch = getopt_long(argc, argv, "p:t:", long_options, NULL)) != -1)
		switch(ch) {
		case 'p':
			argc--;
			argv++;
			sha = argv[1];
			flags |= CAT_FILE_PRINT;
			printf("The p target is %s\n", sha);
			break;
		case 't':
			argc--;
			argv++;
			sha = argv[1];
			printf("The t target is %s\n", sha);
			flags |= CAT_FILE_TYPE;
			break;
		default:
			printf("cat-file: Currently not implemented\n");
			cat_file_usage(0);
		}

	if (git_repository_path() == -1) {
		fprintf(stderr, "fatal: not a git repository (or any of the parent directories): .git");
		exit(0);
	}

	if (flags & CAT_FILE_TYPE) {
		unsigned char sha_hex[20];
		int i;
		for(i=0;i<20;i++)
			sscanf(sha+i*2, "%2hhx", &sha_hex[i]);

		printf("%02x%02x%02x%02x BBBB\n",   sha_hex[0], sha_hex[1], sha_hex[2], sha_hex[3]);

		cat_file_type(&sha_hex);
	}
	else if (flags & CAT_FILE_PRINT) {
		printf("Unimplemented -p option\n");
		exit(0);
	}

	argc = argc - q;
	argv = argv + q;

/*
struct fan *fans;
struct entry *entries;
struct checksum *checksums;
struct offset *offsets;
*/

//	pack_pointer(fans, entries, checksums, offsets);

	return (ret);
}

