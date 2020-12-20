ZSC - Zaurus Star Castle

A port of XSC
By Mark B. Hanson <mbh@panix.com>
http://www.panix.com/~mbh/projects.html
Copyright (c) 1993-2001

Ported to SDL, and also scaled down for the Sharp Zaurus SL-5500 PDA
By Bill Kendrick <bill@newbreedsoftware.com>
ftp://ftp.sonic.net/pub/users/nbs/unix/zaurus/zsc/
2002.Apr.22 - 2002.Apr.27


About
-----
  XSC is a clone of a classic vector-graphics arcade game, "Star Castle."
  ZSC is a conversion of that from X-Window to SDL, the addition of
  sound effects, and compile-time options for displaying on a small
  screen (240x320 16bpp) like that of the Sharp Zaurus SL-5500
  Linux-based PDA.

xsc-1.4.1
---------
  This directory contains XSC version 1.4 with changes (SDL and embedded
  code) added to it.  It also contains a "data/" subdirectory with
  sound effects...

Makefile
--------
  Makefile included in the "xsc-1.4.1/" directory (a copy of
  "Makefile-FOR-ZSC", in case "Makefile" gets removed) was simply
  created by running "./configure" on a Debian Linux 80x86 box.

  It was modified to include 3 more targets.  These should be incorporated
  into the 'automake/autoconf' setup (e.g., "./configure") for XSC.
  I don't know these tools, though.  Help is appreciated!

  The new targets are:

    sdl - Build more-or-less the original game, but using SDL instead of
          X-Window for graphics, and also include sound effects.
	  It's currently forced to an unresizable 640x480 window, however.

    emtest - Build using SDL, with 'embedded' options turned on.
             (In otherwords, display with a different aspect ration and shape,
	     within a 240x320 window.)  Builds for the host system.
	     Meant for testing the embedded version.
	     Assumes data files (sounds) are in "./data/".

    embedded - Build using SDL, with 'embedded' options turned on, and
               build for the ARM architecture.  (Lineo Embedix compiler
	       toolchain is assumed to be available.)
               Assumes data files (sounds) are in "/opt/QtPalmtop/share/xsc/".
	       Statically links against "libSDL.a" and "libSDL_mixer.a"
	       for ARM.  (Graciously provided by David Hedbor.)

mkipk.sh
--------
  This shell script simply creates a "tmp/" directory, tries to build
  the "embedded" target within the "xsc-1.4.1" subdirectory, and
  throws the resulting binary ("xsc", renamed to "zsc"), a wrapper
  shell script ("zsc.sh"), a desktop file ("zsc.desktop"), the colorful
  icon ("zsc.png"), and the sound files together and creates an ".ipk"
  package file for iPaqs running Linux and the Sharp Zaurus.
  (Requires "ipkg-build" tool.)

Controls
--------
  Left/Right rotate
  Up thrusts
  Selector (space) shoots
  Cancel (escape) quits
  Ok (enter) starts a new game if you die


To-Do
-----
  * Replace floating point math with fixed point (Zaurus has no FPU) for
    better speed.

  * Integrate Makefile changes into the "./configure" script
    (autoconf/automake) stuff...

  * Integrate code changes (SDL ("#define USE_SDL") and
    embedded ("#define EMBEDDED")) and sound effects into
    Mark's main XSC codebase, rather than being this weird branch.

  * Squash any bugs.


-bill!
bill@newbreedsoftware.com
http://www.newbreedsoftware.com/

