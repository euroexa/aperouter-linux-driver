#!/bin/sh

# KARMA environment setup on Roma Trenz boards
export KARMA=$(pwd)

export KARMA_TOPO="allnodes"
export KARMA_HOSTS="trenz00 trenz01 trenz02 trenz03"
export KARMA_TORUS="trenz00:0:0:0:0 trenz01:0:0:1:1 trenz02:0:0:1:0 trenz03:0:0:0:1"
