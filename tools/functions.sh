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

err() {
    str="$*"
    echo "ERR : $str" >&2
}

die() {
    err "$*"
    exit 1
}


msg() {
    str="$*"
    echo "MSG : $str" >&2
}

dbg() {
    str="$*"
    #echo "DEBG: $str" >&2
}

warn() {
    str="$*"
    echo "WARN: $str" >&2
}

info() {
    str="$*"
    echo "INFO: $str" >&2
}

timeout_command_exists () {
    type timeout &> /dev/null
}

if timeout_command_exists; then
    export	TIMEOUT_CMD=timeout
else
#     if [ ! -f timeout.sh ]; then
#        echo "ERROR: timeout.sh not found."
#     else
#        echo "export TIMEOUT_CMD=$TOOLS_DIR/timeout.sh"
       export TIMEOUT_CMD=$TOOLS_DIR/timeout.sh
#     fi
fi

# from http://stackoverflow.com/questions/688849/associative-arrays-in-shell-scripts

####################################################################
# Bash v3 does not support associative arrays
# and we cannot use ksh since all generic scripts are on bash
# Usage: map_put map_name key value
#
function map_put
{
    alias "${1}$2"="$3"
}

# map_get map_name key
# @return value
#
function map_get
{
    alias "${1}$2" | awk -F"'" '{ print $2; }'
}

# map_keys map_name
# @return map keys
#
function map_keys
{
    alias -p | grep $1 | cut -d'=' -f1 | awk -F"$1" '{print $2; }'
}

#-----------------------------------------------------------------

# indexed array
declare -a KARMA_TOPOLOGY

# parse KARMA_TOPOLOGY
function parse_karma_conf() {
    local conf=${1}
    KARMA_PARTITION=""

    if [ ! -e $conf ]; then
        err "conf file $conf does not exist!"
        return 1
    fi

    l=0
    while read tag p0 p1 p2 p3 p4 p5 p6; do
        l=$(( l + 1 ));

        case $tag in
            KARMA_TOPOLOGY)
                KARMA_TOPOLOGY[0]=$p0
                KARMA_TOPOLOGY[1]=$p1
                KARMA_TOPOLOGY[2]=$p2
                ;;

            KARMA_PARTITION)
                map_put KARMA_PARTITION $p0 "$p1 $p2 $p3 $p4 $p5 $p6"
                ;;

            \#*)
                # skip comments
            ;;

        *)
            err "invalid tag in file $conf"
            exit 1
            ;;
    esac

done <$conf
}

function parse_karma_host_def() {
    local hdef=${*}
    local l=0
    local host=""
    local coord=""
    for f in $(echo $hdef | tr ':' '\n'); do
        case $l in
            0)
                host=$f
                ;;
            [1-4])
                coord="$coord $f"
                ;;
            *)
                err "while parsing host definition $f"
                return 1
        esac
        l=$(( l+1 ))
    done
    coord=${coord:1}
    dbg "parse_karma_host_def: host=$host coord=$coord"
    map_put KARMA_HOST_LIST $host "$coord"
    return 0
}

# parse KARMA_TORUS
function parse_karma_torus() {
    local tor=${*}
    local nhosts=$#

    for (( i=0; i<$nhosts; i++ )); do
        tok=$1
        parse_karma_host_def $tok
        shift
    done

    return 0
}


function karma_insmod() {
    local hname=$(cut_dot $1)
    local tools_dir=$2
    local sw_dir=$3
    local hcoord=""
    local l=0
    local coords="$(map_get KARMA_HOST_LIST $hname)"
    declare -a host_coord

    dbg "karma_insmod: host name=$hname coords=$coords"

    if [ -z "$coords" ]; then
        err "host $hname is not defined"
        return -1
    fi

    for c in $coords; do
        host_coord[$l]=$c
        dbg "host_coord[$l]=$c"
        l=$((l+1))
    done

    insmod.sh ${host_coord[0]} ${host_coord[1]} ${host_coord[2]} ${host_coord[3]} ${KARMA_TOPOLOGY[0]} ${KARMA_TOPOLOGY[1]} ${KARMA_TOPOLOGY[2]}
}

function cut_dot {
    local hn=$1

    a=$(expr index $hn '.' )
    #echo $a

    if [ $a -gt 0 ]; then
        hn=${hn:0:$a-1}
    fi

    echo $hn
}
# # karma_host2coord hostname
# # @return x:y:z

function karma_host2coord() {
    if [ $# -ne 1 ]; then
        err "usage: karma_host2coord hostname"
        return 1
    fi
    local hname=$(cut_dot $1)
    local hcoord=""
    local l=0
    local coords="$(map_get KARMA_HOST_LIST $hname)"
    declare -a host_coord

    dbg "karma_host2coord: host name=$hname coords=$coords"

    if [ -z "$coords" ]; then
        err "host $hname is not defined"
        return 1
    fi

    for c in $coords; do
        host_coord[$l]=$c
        dbg "host_coord[$l]=$c"
        l=$((l+1))
    done

    echo ${host_coord[0]}:${host_coord[1]}:${host_coord[2]}:${host_coord[3]}
}

#-------------------------------------------------------
# karma_coord2host x:y:z
# @return hostname

function karma_coord2host() {
    if [ $# -ne 1 ]; then
        err "usage: karma_coord2host x:y:z"
        return 1
    fi
    local hname=""
    local hcoord=$1
    local coords=""
    local hosts="$(map_keys KARMA_HOST_LIST)"
    declare -a host_coord

    host_coord[0]=`echo $hcoord | awk -F":" '{ print $1}'`
    host_coord[1]=`echo $hcoord | awk -F":" '{ print $2}'`
    host_coord[2]=`echo $hcoord | awk -F":" '{ print $3}'`
    host_coord[3]=`echo $hcoord | awk -F":" '{ print $4}'`
    coords=`echo ${host_coord[0]} ${host_coord[1]} ${host_coord[2]} ${host_coord[3]}`
    dbg "karma_coord2host: host coords are $coords"

    for i in $hosts; do
        key="$(map_get KARMA_HOST_LIST $i)"
        if [ "$key" == "$coords" ]; then
            hname=$i
        fi
    done

    if [ -z "$hname" ]; then
        err "hostname for $hcoord is not defined"
        return 1
    fi

    echo $hname
    return 0
}

#-------------------------------------------------------
# karma_*_coord x:y:z
# @return value

function karma_x_coord() {
    if [ $# -ne 1 ]; then
        err "usage: karma_x_coord x:y:z"
        return 1
    fi
    echo $1 | awk -F":" '{ print $1}'
}

function karma_y_coord() {
    if [ $# -ne 1 ]; then
        err "usage: karma_y_coord x:y:z"
        return 1
    fi
    echo $1 | awk -F":" '{ print $2}'
}

function karma_z_coord() {
    if [ $# -ne 1 ]; then
        err "usage: karma_z_coord x:y:z"
        return 1
    fi
    echo $1 | awk -F":" '{ print $3}'
}


#-------------------------------------------------------
# remote_exec_single hostname command command_params

remote_exec_single() {
    local remote_host=$1
    local remote_cmd=${2}
    local simple_remote_cmd=${2##*/}
    shift 2
    local remote_params="$*"
    #echo "ssh $remote_host $remote_cmd $remote_params"
    ssh -t $remote_host "env PATH=$PATH LD_LIBRARY_PATH=$LD_LIBRARY_PATH QUARTUS_ROOTDIR=$QUARTUS_ROOTDIR CUDA_USE_NVP2P=$CUDA_USE_NVP2P KARMA=$KARMA KARMA_TORUS=\"$KARMA_TORUS\" KARMA_HOSTS=\"$KARMA_HOSTS\" KARMA_TOPO=$KARMA_TOPO $remote_cmd $remote_params" 2>/dev/null || {
        echo "ssh $h error, aborting..."
        return 1
    }

}

#-------------------------------------------------------
# remote_exec_single_nonblocking hostname command command_params

remote_exec_single_nonblocking() {
    local remote_host=$1
    local remote_cmd=${2}
    shift 2
    local remote_params="$*"
    dbg "ssh -f $remote_host $remote_cmd $remote_params"
    ssh -f -t $remote_host "env PATH=$PATH LD_LIBRARY_PATH=$LD_LIBRARY_PATH QUARTUS_ROOTDIR=$QUARTUS_ROOTDIR CUDA_USE_NVP2P=$CUDA_USE_NVP2P KARMA=$KARMA KARMA_TORUS=\"$KARMA_TORUS\" KARMA_HOSTS=\"$KARMA_HOSTS\" KARMA_TOPO=$KARMA_TOPO $remote_cmd $remote_params" 2>/dev/null || {
        echo "ssh $h error, aborting..."
        return 1
    }

}

#-------------------------------------------------------

remote_exec () {
    #local prog=${1##*/}
    #local command=$1
    #shift 1
    local h
    local remote_cmd=${1}
    local simple_remote_cmd=${1##*/}
    #local remote_params="$2 $3 $4 $5 $6 $7"
    shift 1
    local remote_params="$*"
    local nn=0
    #echo "remote_cmd=$remote_cmd"
    #echo "hosts list: $HOSTS_LIST"
    for h in $HOSTS_LIST; do
        [ "$verbose" != "no" ] && echo "###### spawning $simple_remote_cmd on host $h"
        #echo "ssh $h $remote_cmd $remote_params"
        ssh -t $h "env PATH=$PATH LD_LIBRARY_PATH=$LD_LIBRARY_PATH QUARTUS_ROOTDIR=$QUARTUS_ROOTDIR CUDA_USE_NVP2P=$CUDA_USE_NVP2P KARMA=$KARMA KARMA_TORUS=\"$KARMA_TORUS\" KARMA_HOSTS=\"$KARMA_HOSTS\" KARMA_TOPO=$KARMA_TOPO $remote_cmd $remote_params $nn" 2>/dev/null || {
            echo "ssh $h error, aborting..."
        exit 1
    }
    nn=$((nn+1))
done
}
