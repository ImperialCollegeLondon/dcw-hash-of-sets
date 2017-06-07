# destination: home dir..
# DEST	=	$(HOME)
# or can install into TOOLDIR (env var, eg ~/c-tools)
DEST	=	$(TOOLDIR)
BINDIR	=	$(DEST)/bin
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CC	=	gcc
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR) -lADTs
BUILD	=	testfamcoll transform

all:	$(BUILD)

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

install:
	install -m 755 summarisetests $(BINDIR)
