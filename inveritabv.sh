#!/bin/bash
#
# inveritabv.sh
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

SNAPSHOT_LOCATION=""
VALIDATION_RESULT="UNKNOWN"


###########################################################################
# logmessage
#
# Print a colored message to stdout.
#
# Params:
# $1 Message to be printed on stdout
#
function logmessage {
   echo -e "\033[1;34m$1\033[m"
}


if [[ $# -ne 1 ]]
then
   echo "Please specify the location of your backup, e.g. '$0 /media/share/backup'"
   exit
fi

if [[ ! -d $1 ]]
then
   echo "Backup location '$1' not found, exiting."
   exit
fi


SNAPSHOT_LOCATION=$1/latest

cd $SNAPSHOT_LOCATION
if [[ $? -ne 0 ]]
then
   logmessage "Backup location exists, but unable to enter directory '$SNAPSHOT_LOCATION', exiting."
   exit
fi


logmessage "Validating backup snapshot '$1', please wait..."
sha1sum -c digests >/tmp/validationresults

if [[ $? -ne 0 ]]
then
   VALIDATION_RESULT="FAILED"
   grep -v ": OK" /tmp/validationresults >/tmp/validationfails
   echo mail -s "Backup snapshot validation failed!" mail@marcweidler.de </tmp/validationfails
else
   VALIDATION_RESULT="PASSED"
   echo mail -s "Backup snapshot validation succeeded." mail@marcweidler.de
fi

rm /tmp/validationresults
logmessage "Validating finished, result: $VALIDATION_RESULT"

