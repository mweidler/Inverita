#!/bin/bash
#
# createppas.sh
#
# The following packages must be installed on your system:
#   sudo apt-get install dh-make
#   sudo apt-get install devscripts
#   sudo apt-get install dput
#
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

rm -rf temp

COMMIT_VERSION=`git describe HEAD 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/'`
TARGETROOT="temp"
TARGETDIR="$TARGETROOT/inverita-$COMMIT_VERSION"

./preparepackage.sh precise
cd $TARGETDIR
dpkg-buildpackage -S
cd ../..

./preparepackage.sh quantal
cd $TARGETDIR
dpkg-buildpackage -S
cd ../..

./preparepackage.sh raring
cd $TARGETDIR
dpkg-buildpackage -S
cd ../..

./preparepackage.sh saucy
cd $TARGETDIR
dpkg-buildpackage -S
cd ../..

./preparepackage.sh trusty
cd $TARGETDIR
dpkg-buildpackage -S
cd ..

ls -rtl

echo "Now 'dput ppa:mweidler/inverita *.changes'"

