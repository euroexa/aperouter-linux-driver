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
# 

THIS_DIR=$(dirname $0)
if [ "${THIS_DIR:0:1}" == "." ]; then
    THIS_DIR=$PWD/$THIS_DIR
fi

KARMA_SOFTWARE_DIR=$THIS_DIR/..



TOOLS_DIR=$KARMA_SOFTWARE_DIR/tools
TEST_DIR=$KARMA_SOFTWARE_DIR/test

PATH=$PATH:$TEST_DIR:$TOOLS_DIR

. $TOOLS_DIR/functions.sh

do_reset="yes"
verbose="no"

while [ "$1" != "${1##[-+]}" ]; do
    case $1 in
        -r)
            do_reset="no"
            shift 1
            ;;
        -v)
            verbose="yes"
            shift 1
            ;;
        *)  echo $"Usage: $0 [PARAMS]"
            echo $" -r        don't reset after driver load"
            echo $" -v        verbose output"
            echo $" -h        this help text"
            exit 1;;
    esac
done

if [ "$verbose" == "yes" ]; then
    set -x
fi

# parse KARMA_TOPOLOGY
if [ -z "$KARMA" ];then
    err "ERROR: KARMA environment var is not defined!"
    exit 1
fi
parse_karma_conf /apotto/home1/homedirs/martinelli/apelink/apenet_plus/software/tools/karma.conf || { echo "ERROR: while parsing karma.conf"; exit 1; }

# parse KARMA_TORUS  host/coord list
if [ -z "$KARMA_TORUS" ]; then
    err "KARMA_TORUS environment variable is not set!\n"
    exit 1
fi
parse_karma_torus $KARMA_TORUS || { err "in parse_ape_torus"; exit 1; }

NX=${APENET_TOPOLOGY[0]}
NY=${APENET_TOPOLOGY[1]}
NZ=${APENET_TOPOLOGY[2]}

info "KARMA 3D Torus topology: $NX $NY $NZ"

dbg "unload/load of the APEnet driver:"
{
    hn=$HOSTNAME
    [ -z "$hn" ] && hn=$HOST

    if [ "$verbose" != "yes" ]; then
        1>/dev/null
        2>&1
    fi

    karma_insmod $HOSTNAME $TOOLS_DIR $SOFTWARE_DIR || { \
    err "this host name is not supported!!!"; \
        exit 1; \
    }

} || { err "driver loading error"; exit 1; }

if [ "$do_reset" == "yes" ]; then
    info "sleeping 1sec then issuing a (simple) reset:"
    sleep 1
#     $TEST_DIR/reset 2>&1 >/dev/null || { err "while resetting, exiting...."; exit 1; }

    info "setting default Link thresholds:"

    for l in Xp Xn Yp Yn Zp Zn Tp Tn Tc
    do
        set_reg_name -s LINK_CONFIG_${l}_THR_RED_HEADER 0x64 \
                     -s LINK_CONFIG_${l}_THR_RED_DATA 0x140 \
                     -s LINK_CONFIG_${l}_LOOPBACK 0x0 \
                     -s LINK_CONFIG_${l}_TX_WAITING_CYCLE 0x40
    done

else
    warn "skipping reset"
fi
