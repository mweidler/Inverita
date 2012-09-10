#!/bin/bash
#
# buildpackage.sh
#
# This file is part of INVERITA.
#
# INVERITA Personal Backup Software
# Copyright (C) 2012  Marc Weidler <marc.weidler@web.de>,
#                     Ulrichstr. 12/1, 71672 Marbach, Germany.
#                     All rights reserved.
#
# INVERITA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# INVERITA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#set -x

qmake Inverita.pro
make clean
./versioning.sh

make all

if [[ `uname -i` == i386 ]]
then
   ARCH="i386"
else
   ARCH="amd64"
fi

VERSION=`git describe HEAD 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/'`
PACKAGENAME=inverita-"$VERSION"_"$ARCH".deb

cp Inverita packaging/usr/bin/inverita

echo -e "Package: INVERITA\n\
Version: $VERSION\n\
Section: utility\n\
Priority: optional\n\
Architecture: $ARCH\n\
Depends: libc6 (>= 2.15), libqtcore4 (>=4:4.8.0), libqtgui4 (>=4:4.8.0), encfs (>= 1.7.4)\n\
Installed-Size: 500\n\
Maintainer: Marc Weidler <marc.weidler@web.de>\n\
Description: INVERITA Personal Backup is a software for data backup on various media.\n\
 .\n\
 The main features are:\n\
   * Backup data in snapshots\n\
   * Nice and user-friendly user interface\n\
   * Hard-link technology for each backup snapshot to minimize needed media space.\n\
   * Integrity validation of backups\n\
" >packaging/DEBIAN/control

fakeroot dpkg -b packaging $PACKAGENAME
dpkg -c $PACKAGENAME

