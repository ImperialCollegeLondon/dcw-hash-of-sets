CC	=	gcc
DEST	=	$(HOME)/c-tools
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR) -lADTs
ALL	=	testmld

all:	$(ALL)

clean:
	rm -f lib* *.o $(ALL)

test:	testmld
	./testmld | ./summarisetests

testmld:	testmld.o mlistdefns.o
testmld.o:	mlistdefns.h
mlistdefns.o:	mlistdefns.h
