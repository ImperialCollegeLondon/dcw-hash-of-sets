DEST	=	$(HOME)/c-tools
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CC	=	gcc
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR) -lADTs
ALL	=	testfamcoll transform

all:	$(ALL)

clean:
	rm -f lib* *.o $(ALL)

test:	testfamcoll
	./testfamcoll | ./summarisetests

testfamcoll:	testfamcoll.o famcoll.o csvsplit.o
transform:	transform.o famcoll.o readline.o
testfamcoll.o:	famcoll.h csvsplit.h
transform.o:	famcoll.h readline.h
famcoll.o:	famcoll.h
csvsplit.o:	csvsplit.h
