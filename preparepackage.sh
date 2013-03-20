#!/bin/bash
#
# preparepackage.sh
#
# This file is part of INVERITA.
#
# INVERITA Personal Backup Software
# Copyright (C) 2012-2013  Marc Weidler <marc.weidler@web.de>,
#                          Ulrichstr. 12/1, 71672 Marbach, Germany.
#                          All rights reserved.
#
# INVERITA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# INVERITA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
#set -x

COMMIT_VERSION=`git describe HEAD 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/'`
TARGETROOT="temp"
TARGETDIR="$TARGETROOT/inverita-$COMMIT_VERSION"
DIST_RELEASE="unknown"
ARGC=$#

if [ $ARGC -ne 1 ]
then
   echo "Please specify one distribution release name, e.g. 'preparepackage.sh precise'"
   exit
fi

DIST_RELEASE=$1

rm -rf $TARGETDIR
mkdir -p $TARGETDIR

./versioning.sh
./createchangelog.sh $DIST_RELEASE

echo "Copying source files to '$TARGETDIR'..."

cp -R -a packaging      $TARGETDIR
cp -R -a images         $TARGETDIR
cp -R -a translations   $TARGETDIR
cp -R -a src            $TARGETDIR
cp -R -a debian         $TARGETDIR

cp -a LICENSE           $TARGETDIR
cp -a Inverita.pro      $TARGETDIR
cp -a resources.qrc     $TARGETDIR
cp -a COPYRIGHT         $TARGETDIR
cp -a COPYRIGHT         $TARGETDIR/debian/copyright
cp -a README            $TARGETDIR
cp -a Inverita.pro.user $TARGETDIR

mkdir $TARGETDIR/mocs
mkdir $TARGETDIR/objs

echo "Done."

echo "Now enter '$TARGETDIR' and execute"
echo "       1) 'dpkg-buildpackage -B' for building a binary package and/or"
echo "       2) 'dpkg-buildpackage -S' for building a source package"

