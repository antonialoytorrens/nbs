#!/bin/sh

for i in $*; do
  sed -e s/None/#FFFFFF/ $i | xpmtoppm | ppmtogif > $i.gif
  chmod 644 $i.gif
done
