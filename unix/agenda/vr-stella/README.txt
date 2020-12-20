vr-stella

bill kendrick
bill@newbreedsoftware.com

2001.Apr.16

This is a port of Stella, the Atari 2600 game system emulator, 
to the Agenda VR3 Linux-based PDA.

Files:

  vr-stella.gz
    MIPS binary build of Stella's "linux-x" target (normally called "xstella").
    Built with the hacked "mainX11.cxx".
    Cross-compiled on an i386 Linux box using mipsel-linux tools and
    ...the hacked "makefile".
    Gzipped.

  TPS.BIN
    "This Planet Sucks," a free game ROM that comes with Stella.

  TPS.gif
    A screenshot of "This Planet Sucks" running under Stella on an Agenda.

  mainX11.cxx
    Hacked X11 source.  Adds "Tiny" mode (by default) and extra keyboard
    ...controls (PageUp/PageDown for joystick up/down; either Shift for fire).
    Place in Stella's "src/ui/x11/" directory.

  makefile
    Hacked "makefile" for cross-compiling "linux-x" target
    ...to create "vr-stella".
    Place in Stella's "src/build/" directory.

  
Resources:

  Stella
    http://www4.ncsu.edu/~bwmott/www/2600/

  Agenda:
    http://www.agendacomputing.com/
