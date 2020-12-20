#!/bin/bash

for x in $(seq 1 $#)
do
  i="${!x}"
  FNAME=`dirname "$i"`/`basename "$i" .kpx`
  JPG="$FNAME.jpg"
  PNG="$FNAME.png"

  echo From $i to $JPG
  dd ibs=1 obs=1 if="$i" skip=60 of="$JPG"
  identify "$JPG"

  echo Now from $JPG to $PNG
  convert "$JPG" "$PNG"
  # we could also use "djpeg ... | pnmtopng > ..."
  identify "$PNG"

  echo Cleaning up
  rm "$JPG"
done
