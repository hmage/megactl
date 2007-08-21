
SRCS=		megactl.c adapter.c megaioctl.c megatrace.c callinfo.c dumpbytes.c logpage.c ntrim.c
INC=		-I./schily -Iincludes-hack
HDRS=		mega.h adapter.h megaioctl.h callinfo.h logpage.h dumpbytes.h
ARCH=		-m32
CFLAGS=		-g -Wall $(INC) $(ARCH)
LDFLAGS=	-g $(ARCH)
PROGRAMS=	megactl megasasctl megatrace

all:		$(PROGRAMS)

megatrace:	megatrace.o callinfo.o dumpbytes.o
	$(CC) $(LDFLAGS) -o $@ megatrace.o callinfo.o dumpbytes.o

megactl:	megactl.o adapter.o dumpbytes.o megaioctl.o logpage.o ntrim.o
	$(CC) $(LDFLAGS) -o $@ megactl.o adapter.o dumpbytes.o megaioctl.o logpage.o ntrim.o

megasasctl:	megasasctl.o adapter.o dumpbytes.o megaioctl.o logpage.o ntrim.o
	$(CC) $(LDFLAGS) -o $@ megasasctl.o adapter.o dumpbytes.o megaioctl.o logpage.o ntrim.o

megasasctl.o:	megactl.c
	$(CC) $(CFLAGS) -c -o $@ -DMEGA_SAS_CTL megactl.c

%.o:		Makefile.bak %.c
	$(CC) $(CFLAGS) -c -o $@ $*.c

clean:
	$(RM) $(PROGRAMS) *.o

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

megactl.o:	mega.h adapter.h megaioctl.h logpage.h dumpbytes.h
megasasctl.o:	mega.h adapter.h megaioctl.h logpage.h dumpbytes.h
adapter.o:	mega.h megaioctl.h logpage.h ntrim.h
megaioctl.o:	mega.h megaioctl.h logpage.h 
megatrace.o:	mega.h megaioctl.h logpage.h callinfo.h dumpbytes.h
callinfo.o:	callinfo.h
logpage.o:	mega.h megaioctl.h logpage.h ntrim.h dumpbytes.h
ntrim.o:	ntrim.h

# DO NOT DELETE
