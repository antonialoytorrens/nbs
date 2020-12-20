# makefile for xbomber

# by Bill Kendrick
# kendrick@zippy.sonoma.edu

# New Breed Software
# http://zippy.sonoma.edu/kendrick/nbs/

# January 28, 1998 - June 18, 1998


# Makefile user-definable variables------------------------------------------

CC=gcc
MATHLIB=-lm
CFLAGS=-I/usr/X11/include

# Change this to "touch" if your system doesn't have the "strip" command:
POSTPROCESS=strip

# adjust according to target
XLIB=-L/usr/X11/lib -lX11

# uncomment for sunos/solaris
#XLIB=-L/usr/openwin/lib -lX11

# uncomment for FreeBSD
#XLIB=-L /usr/X11R6/lib -lX11 -lcompat

# End of makefile user-definable variables-----------------------------------


# Makefile other variables

OBJECTS=xbomber.o window.o connect.o hints.o visual.o gc.o color.o randnum.o \
	text.o


# Makefile commands: 

all:	xbomber
	chmod 755 sounds bitmaps pixmaps sounds/numbers levels
	chmod 644 *.c *.h makefile sounds/*.au *.txt \
		bitmaps/*.xbm pixmaps/*.ppm \
		sounds/numbers/*.au levels/*.dat

clean:
	rm *.o


# Applications:

xbomber:	$(OBJECTS)
	$(CC)	$(OBJECTS) $(XLIB) $(MATHLIB) $(CFLAGS) -o xbomber
	chmod 755 xbomber
	$(POSTPROCESS) xbomber


# Application objects:

xbomber.o:	xbomber.c window.h connect.h hints.h visual.h gc.h color.h \
		randnum.h text.h
	$(CC)	xbomber.c -c $(CFLAGS)


# Library objects:

window.o:	window.c window.h
	$(CC)	window.c -c $(CFLAGS)

connect.o:	connect.c connect.h
	$(CC)	connect.c -c $(CFLAGS)

hints.o:	hints.c hints.h
	$(CC)	hints.c -c $(CFLAGS)

visual.o:	visual.c visual.h
	$(CC)	visual.c -c $(CFLAGS)

gc.o:	gc.c gc.h
	$(CC)	gc.c -c $(CFLAGS)

color.o:	color.c color.h
	$(CC)	color.c -c $(CFLAGS)

randnum.o:	randnum.c randnum.h
	$(CC)	randnum.c -c $(CFLAGS)

text.o:		text.c text.h
	$(CC)	text.c -c $(CFLAGS)
