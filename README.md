This is a cut-down version of a single part of Duncan White's mailing list
flattener example project.  It depends on Duncan's c-tools directory,
which lives in ~dcw/c-tools.

To build it, first copy the c-tools to your home directory:

cp -pr ~dcw/c-tools ~

bash users should do the following:

export TOOLDIR=$HOME/c-tools
export ARCH=x86_64

Whereas csh users should do:

setenv TOOLDIR $HOME/c-tools
setenv ARCH x86_64

Then build it via:

make

To run it's tests:

make test
