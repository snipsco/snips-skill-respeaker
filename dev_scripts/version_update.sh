#!/usr/bin/env bash

set -e

OPT=$1
NEW_VERSION=$2
TIME=$(date "+%b-%d-%Y")

if [ -z $OPT ]; then
    echo "Usage: $0 [-s, -b] CURRENT_VERSION"
    exit 1
fi

if [ -z $NEW_VERSION ]; then
    echo "Usage: $0 [-s, -b] CURRENT_VERSION"
    exit 1
fi

SPLIT_VERSION=( ${NEW_VERSION//./ } )
if [ ${#SPLIT_VERSION[@]} -ne 3 ]; then
  echo "Version number is invalid (must be of the form x.y.z)"
  exit 1
fi

if [ $1 == "-s" ]; then
    let SPLIT_VERSION[2]+=1
fi

if [ $1 == "-b" ]; then
    let SPLIT_VERSION[1]+=1
fi

NEXT_NEW_VERSION="${SPLIT_VERSION[0]}.${SPLIT_VERSION[1]}.${SPLIT_VERSION[2]}"

perl -p -i -e "s/^#define VERSION \".*\"\$/#define VERSION \"$NEXT_NEW_VERSION\"/g" ../include/common.h
perl -p -i -e "s/^#define LAST_UPDATE \".*\"\$/#define LAST_UPDATE \"$TIME\"/g" ../include/common.h

perl -p -i -e "s/snips--led--animation-v.*-blue.svg/snips--led--animation-v$NEXT_NEW_VERSION-blue.svg/g" ../README.md

perl -p -i -e "s/^APP = action-led_animation_.*\$/APP = action-led_animation_$NEXT_NEW_VERSION/g" ../makefile

echo -e "New version bumped to \033[1;32;32m$NEXT_NEW_VERSION\033[m"