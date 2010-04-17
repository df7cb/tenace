#!/bin/sh
# Run this to generate all the initial makefiles, etc.

set -ex

intltoolize --copy --force --automake
libtoolize --force --copy
autoreconf
./configure "$@"
