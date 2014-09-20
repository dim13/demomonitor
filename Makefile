PROG=	demomonitor
SRCS=	demomonitor.c Graph.c GraphDisplay.c BarDisplay.c
NOMAN=
LIBS=	xt
PCCF!=	pkg-config --cflags ${LIBS}
CFLAGS+=${PCCF}
PCLD!=	pkg-config --libs ${LIBS}
LDADD+=	${PCLD}
DEBUG+=	-Wall -ggdb

.include <bsd.prog.mk>
