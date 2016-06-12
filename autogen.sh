#!/bin/sh
# Run this to generate all the initial makefiles, etc.

set -ex

test -d lib || mkdir lib

gnulib-tool --update
autoreconf --force --install --verbose
intltoolize --copy --force --automake
./configure "$@"
