#!/bin/sh

set -e

CC="gcc"
CFLAGS="-Wall -Wextra -std=c17 -pedantic -lm"

$CC main.c -o voronoi $CFLAGS
