#!/bin/bash
#
# createmanuals.sh
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

echo "Creating man page..."
#rm -f inverita.man
#pandoc inverita.md -t man --standalone | grep -v "\[IMAGE:" >inverita.man

echo "Creating pdf document..."
convert images/info-icon.svg info-icon.pdf
convert images/warning-icon.svg warning-icon.pdf
pdflatex Inverita.latex
pdflatex Inverita.latex
rm Inverita.aux Inverita.lof Inverita.log Inverita.out Inverita.toc
rm info-icon.pdf warning-icon.pdf


#echo "Creating html version..."
#rm -f inverita.html
#pandoc Inverita.md -t html --standalone -o inverita.html

