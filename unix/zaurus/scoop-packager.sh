#!/bin/bash

# scoop-packager.sh
#
# Turns the HTML files downloaded by "Site Scooper"
# ("~/.sitescooper/txt/" on your desktop)
# into an ".ipk" package for upload onto your Zaurus
# (into "/home/root/sites/" on the Zaurus)
#
# by Bill Kendrick <bill@newbreedsoftware.com>
# http://www.newbreedsoftware.com/
#
# 2002.Jan.09 - 2002.Jan.10 - 15:13pst


# BUGS:
#   The .ipk does not appear in the GUI "Add/Remove Programs" application!!!
#   "ipkg upgrade" doesn't seem to work
#   "ipkg install"'ing a newer ".ipk" simply adds to what's inside
#     "/home/root/sites/", rather than replacing it
#
# HELP!!!  - bill@newbreedsoftware.com


PACKAGE=scooped
VER=`date +%Y%m%d%H%M`
DATE=`date`
USER=`whoami`
CONTROL=control
ARCH=arm
RM=rm

echo
echo "CREATING CONTROL FILE"

echo "Package: $PACKAGE" > $CONTROL
echo "Priority: optional" >> $CONTROL
echo "Version: $VER" >> $CONTROL
echo "Section: web" >> $CONTROL
echo "Architecture: $ARCH" >> $CONTROL
echo "Maintainer: $USER" >> $CONTROL
echo "Description: Scooped Sites" >> $CONTROL
echo "  HTML pages which were scooped on $DATE" >> $CONTROL


echo -n "... compressing ..."

tar -czf control.tar.gz control
$RM control


echo
echo "COPYING SCOOPED SITES"

mkdir -p home/root/sites/
cp -R $HOME/.sitescooper/txt/* home/root/sites/

echo -n "... compressing ..."

tar -czf data.tar.gz ./home/root/sites/
$RM -r home/


echo 
echo -n "CREATING IPK..."

echo "2.0" > debian-binary

tar -czf ${PACKAGE}_$ARCH.ipk \
         ./control.tar.gz ./data.tar.gz ./debian-binary

echo 
echo "CLEANING UP"

$RM control.tar.gz data.tar.gz debian-binary

echo

