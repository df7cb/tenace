#!/bin/sh
# Run this to generate all the initial makefiles, etc.

set -ex

test -d lib || mkdir lib

gnulib-tool --update
intltoolize --copy --force --automake
libtoolize --force --copy
aclocal -I m4
autoheader
automake --add-missing --copy --foreign
autoreconf
./configure "$@"
