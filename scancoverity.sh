#!/bin/bash
#
# scancoverity.sh
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
#
# --------------------------------------------------------------
#
# HOW-TO:
#
# Do once:
#
#    1. Download and extract the tarball
#
#    2. Add the bin directory to your path
#
# Do for each build:
#
#    1. cd to your build directory
#        optional: Run any build steps that you don't want to analyze – i.e. ./configure
#    2. cov-build --dir cov-int [BUILD CMD and ARGS]
#    3. Create a README file with your name, email, and project's name
#    4. tar czvf project.tgz README cov-int
#    5. Upload the project.tgz to your server
#
# --------------------------------------------------------------
#
#set -x

export PATH=$PATH:/home/mweidler/cov-analysis-linux64-6.0.2/bin

qmake Inverita.pro
make clean
./versioning.sh

rm -f coverity/Inverita.tgz
rm -rf coverity/cov-int

cov-build --dir coverity/cov-int make all

tar czvf coverity/Inverita.tgz --directory coverity cov-int README

