CC	=	gcc
DEST	=	$(TOOLDIR)
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
OBJS	=	hash.o
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR)
ALL	=	testmld

all:	$(ALL)

clean:
	rm -f lib* *.o $(ALL)

test:	testmld
	./testmld

hash.o:		hash.h
set.o:		set.h

testmld:	testmld.o mlistdefns.o set.o hash.o
testmld.o:	mlistdefns.h hash.h
mlistdefns.o:	mlistdefns.h set.h hash.h
