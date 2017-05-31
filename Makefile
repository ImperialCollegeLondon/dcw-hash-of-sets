DEST	=	$(HOME)/c-tools
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CC	=	gcc
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR) -lADTs
ALL	=	testhos transform

all:	$(ALL)

clean:
	rm -f lib* *.o $(ALL)

test:	testhos
	./testhos | ./summarisetests

testhos:	testhos.o hashofsets.o
transform:	transform.o hashofsets.o
testhos.o:	hashofsets.h
transform.o:	hashofsets.h
hashofsets.o:	hashofsets.h
