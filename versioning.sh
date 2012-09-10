#!/bin/bash

#set -x

COMMIT_VERSION=`git describe HEAD 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/'`
TAG_VERSION=`git describe HEAD 2>/dev/null | sed 's/\([^-]*\).*/\1/'`

echo "#define INVERITA_COMMIT_VERSION \"$COMMIT_VERSION\"" >src/version.h
echo "#define INVERITA_TAG_VERSION \"$TAG_VERSION\"" >>src/version.h
