#!/bin/bash
#
# createchangelog.sh
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

TEMPFILENAME=`mktemp`
TARGETFILENAME="debian/changelog"
DEBIAN_VERSION="0"
RELEASE_VERSION="1"
DIST_RELEASE="unknown"
ARGC=$#

if [ $ARGC -ne 1 ]
then
   echo "Please specify one distribution release name, e.g. 'createchangelog.sh precise'"
   exit
fi

DIST_RELEASE=$1

echo "Creating '$TARGETFILENAME' for release '$DIST_RELEASE' ..."

# log the head
git --no-pager log --first-parent HEAD~1..HEAD --format="@@@@ %H%n%n  * %s%d%n"  >$TEMPFILENAME

# log all others
git --no-pager log --first-parent HEAD~1 --format="  * %s%d%n" >>$TEMPFILENAME

# log the head again for the footer
git --no-pager log --first-parent HEAD~1..HEAD --format=" -- %aN <%aE>  %aD%n"  >>$TEMPFILENAME

#git --no-pager log --format="@@@@ %H%n%n  * %s%d%n%n -- %aN <%aE>  %aD%n%n" >$TEMPFILENAME
rm -f $TARGETFILENAME

while IFS= read line
do
    MARKER=`echo $line | awk -F" " '{print $1}'`
    HASH=`echo $line | awk -F" " '{print $2}'`

    if [[ $MARKER == "@@@@" ]]
    then
       COMMIT_VERSION=`git describe $HASH 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/'`
       if [[ $COMMIT_VERSION == "" ]]
       then
          COMMIT_VERSION="0"
       fi

       echo "inverita ($COMMIT_VERSION-$DEBIAN_VERSION$DIST_RELEASE$RELEASE_VERSION) $DIST_RELEASE; urgency=low" >>$TARGETFILENAME
    else
       echo "$line" >>$TARGETFILENAME
    fi

done <$TEMPFILENAME

rm $TEMPFILENAME

echo "Done."

