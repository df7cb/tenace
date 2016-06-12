#!/bin/sh
# Run this to generate all the initial makefiles, etc.

set -ex

test -d lib || mkdir lib

gnulib-tool --update
autoreconf --force --install --verbose --warnings=all "$srcdir"
intltoolize --copy --force --automake
./configure "$@"
