#!/bin/bash
#
# beautify.sh
#
# Usage: ./beautify.sh
# will reformat all files in the src directory
#
# This file is part of INVERITA.
#
# INVERITA Personal Backup Software
# Copyright (C) 2012 - 2013  Marc Weidler <marc.weidler@web.de>,
#                            Ulrichstr. 12/1, 71672 Marbach, Germany.
#                            All rights reserved.
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

astyle --version 2>/dev/null
if [[ $? == 0 ]]
then
  ASTYLE_VER=`(astyle --version >/dev/null) 2>&1 | sed 's/[[:alpha:]0[:space:]]//g'`
  ASTYLE_VER_MAJOR=`echo $ASTYLE_VER | awk -F. '{print $1}'`
  ASTYLE_VER_MINOR=`echo $ASTYLE_VER | awk -F. '{print $2}'`

  if [[ $ASTYLE_VER_MAJOR -ge 2 ]]
  then
    astyle --options=astyle.opt --verbose --formatted src/*.cpp src/*.h
  else
    echo "'astyle' version is too old - must be at least version 2."
  fi
else
  echo "'astyle' code beautifier not found. Please install and re-run again."
fi

