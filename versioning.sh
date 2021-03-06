#!/bin/bash
#
# versioning.sh
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#set -x

COMMIT_VERSION=`git describe HEAD 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/'`
TAG_VERSION=`git describe HEAD 2>/dev/null | sed 's/\([^-]*\).*/\1/'`

echo "#define INVERITA_COMMIT_VERSION \"$COMMIT_VERSION\"" >src/version.h
echo "#define INVERITA_TAG_VERSION \"$TAG_VERSION\"" >>src/version.h

echo "Updated 'src/version.h' with tag $TAG_VERSION and commit $COMMIT_VERSION."

