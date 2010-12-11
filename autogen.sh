#!/bin/sh
# Run this to generate all the initial makefiles, etc.

set -ex

test -d lib || mkdir lib

gnulib-tool --update
intltoolize --copy --force --automake
libtoolize --force --copy
automake --add-missing
autoreconf
./configure "$@"
