This is a new "combine hashes and sets" example, even simpler than my
dcw-mini-list-flattening example.

It implements a named collection of sets of strings: a hash-of-sets-of-strings,
given that our c-tools library already contains sets-of-strings and
hashes-from-strings-to-generic-pointers.


# BACKGROUND...

Suppose we are presented with an input file with any number of lines
of the form:

```one: a               (meaning that "one is the parent of a")
two: c               (meaning that "two is the parent of c")
one: b               (meaning that "one is the parent of b")
one: c               (meaning that "one is the parent of c")```

ie. a named parent (eg "one") may have any number of named children (eg "b"),
there may be any number of parents, any number of children per parent,
and the parent,child information may come in any order.

and our goal is to collect all the children belonging to each parent,
producing output showing, for each parent, the complete sorted list of
their children:

```one: b,c,d          (one is the parent of b, c and d - in sorted order)
two: c              (two is the parent of c only)```

Clearly, the core data type here is a hash from parent names (strings) to
sets of strings (the set of children).

We store this as a parent -> set(child) collection.

A hash-of-sets-of-strings,
implemented by the hashofsets.[ch] module and unit-tested by "testhos.c".
The main program that transforms the input to the output is "transform.c".


# BUILDING THIS...

It depends on my c-tools, May 2017 revision (i.e. new libADTs.a) -
which you can download from my website (URL shown below).

To build this "hash of sets of strings" example:

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
