MAN=

CFLAGS=	-Wall -lmd -lz

PROG=	ogit	

SRCS=	ogit.c ini.c index.c common.c remote.c init.c hash_object.c update_index.c

CLEANFILES+=	${PROG}.core

.include <bsd.prog.mk>
