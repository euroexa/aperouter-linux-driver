#!/bin/bash
# 
#   This file is part of the APErouter kernel driver.
#   Copyright (C) 2017 INFN APE Lab.
# 
# 
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

THIS_DIR=$(dirname $0)
if [ "${THIS_DIR:0:1}" == "." ]; then
	THIS_DIR=$PWD/$THIS_DIR
fi

KARMA_SOFTWARE_DIR=$THIS_DIR/..



TOOLS_DIR=$KARMA_SOFTWARE_DIR/tools
TEST_DIR=$KARMA_SOFTWARE_DIR/test

PATH=$PATH:$TEST_DIR:$TOOLS_DIR
# if [ $# != 7 ]; then
# 	echo "syntax: $0 <path to software dir> <nodex> <nodey> <nodez> <nnodex> <nnodey> <nnodez>"
# 	exit 1
# fi

taddr=$1
zaddr=$2
yaddr=$3
xaddr=$4
xproc=$5
yproc=$6
zproc=$7

echo "KARMA 3D Torus coord ($taddr|$zaddr,$yaddr,$xaddr)"
caddr=`printf "0x%08x" $[$taddr << 12 | $zaddr << 8 | $yaddr << 4 | $xaddr]`

echo "X_PROCS=$xproc Y_PROCS=$yproc Z_PROCS=$zproc"
ctopo=`printf "0x%08x" $[$zproc << 8 | $yproc << 4 | $xproc]`

sudo /sbin/rmmod karmadrv 2> /dev/null

sudo /sbin/insmod ../driver/karmadrv.ko karma_addr=$caddr karma_topo=$ctopo

sudo chmod a+wr /dev/aperouter
