README.txt for kpx2png
Bill Kendrick <bill@newbreedsoftware.com>
Tux Paint project <http://www.tuxpaint.org/>

2009.Oct.08

A school contacted the Tux Paint project to find out if Tux Paint could
load templates for Kid Pix, another childrens drawing program.
Knowing nothing about Kid Pix or its templates, we asked for some examples.
The school sent along four ".kpx" files, and we discovered they simply
contain a JPEG inside.

Tux Paint 0.9.22 (not yet released at the time of writing) will support
KPXes directly, via its Starters feature.  In the meantime, the tools
found in this FTP directory can be used to convert KPX files to PNG,
the open graphics format that Tux Paint uses (and which any modern
general-purpose graphics viewing or editing program, including web browsers,
should be able to support).


Windows
-------
John Popplewell created an MSDOS program for Windows users,
found in the file "kpx2png-0.0.1-win32.zip".  He wrote:
  
  It uses libpng and libjpeg and some glue code, plus your 'skip the first
  60 bytes' tip :-) All statically linked so it's a single file. Includes
  source-code and a 'go.sh' file to build it in MinGW/MSYS.
  
  Program file is 'kpx2png.exe'. Needs to be run in a command prompt, no
  installer or anything.  Converts '.kpx' files in the current directory if
  run with no arguments.  Accepts '-h' to show some help, otherwise it
  expects a directory path to the '.kpx' files.  Converts them in-place
  'filename.kpx' -> 'filename.png' leaving the originals untouched.
  
  Example:
  
  C:\>kpx2png "C:/Documents and Settings/jfp/My Documents/templates"
  
  Works with those examples you sent me producing monochrome images that
  look plausible.


Linux, etc.
-----------
Bill Kendrick created a BASH shell script which utilizes the "dd"
command to skip the header in KPX files, and the "convert" command
(part of the ImageMagick package) to convert the JPEG into a PNG.
It's found in the file "kpxtopng.sh".


Mac OS X
--------
Nothing has been created for OS X yet, but it may be possible to use
the script for Linux, assuming "dd" and ImageMagick's "convert" are
available.  (If you can provide instructions, or a tool, please contact us!)


Enjoy!

