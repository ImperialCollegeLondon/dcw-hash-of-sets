DEST	=	$(HOME)/c-tools
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CC	=	gcc
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR) -lADTs
BUILD	=	testfamcoll transform

BUILD:	$(BUILD)

clean:
	rm -f lib* *.o $(BUILD)

test:	testfamcoll
	./testfamcoll | ./summarisetests

testfamcoll:	testfamcoll.o famcoll.o
transform:	transform.o famcoll.o readline.o
testfamcoll.o:	famcoll.h
transform.o:	famcoll.h readline.h
famcoll.o:	famcoll.h
readline.o:	readline.h
