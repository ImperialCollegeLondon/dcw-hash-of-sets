This is a new "combine hashes and sets" example, even simpler than my
dcw-mini-list-flattening example.

It implements a named collection of sets of strings.

Consider some form of "a contains b" information, where each
named entity (eg "a") may contain any number of other named
entities (eg "b").

We store this as a name -> set(members) collection.

It depends on my c-tools, May 2017 revision (i.e. new libADTs.a) -
which you can soon download from my website (URL below).

To build this "hash of sets" example:

1. download the c-tools to your home directory:

cd

wget http://www.doc.ic.ac.uk/~dcw/c-tools.tgz

tar xzf c-tools.tgz

2. set up this project to use the c-tools library:

bash users should do the following:

export ARCH=`arch`

Whereas csh users should do:

setenv ARCH `arch`

3. build this project via:

make

4. run it's tests:

make test
