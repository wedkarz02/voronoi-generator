#!/bin/sh

set -e

CC="gcc"
CFLAGS="-Wall -Wextra"

$CC main.c -o voronoi $CFLAGS
