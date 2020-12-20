README for Explosions Demo

A simplified 3D world with alpha-blended objects that look kind of like
fireworks or sparks.

by Bill Kendrick
bill@newbreedsoftware.com
http://www.newbreedsoftware.com/bill/

July 30, 2001 - August 5, 2001


ABOUT:

  This is a simplified 3D world with alpha-blended objects that look kind
  of like fireworks or sparks.  It also has a number of effects built in,
  such as fading, blurring, fire ("heat"), and zooming..

  It serves no purpose except to look pretty.  The source-code is also
  well-documented, so someone might find it useful.


LICENSE:

  This program is released under the GNU General Public License (GPL).
  See "COPYING.txt" which came with this program for the full text of
  the license.


COMPILING:

  You'll need both "libSDL" and "SDL_image" libraries installed on your
  system to compile "explosions."  If you're using packages (eg, RPM or DEB)
  to install, you'll also need the respective "-dev" packages for these
  libraries (so that you get the header files and so forth).

  To download libSDL, visit:

    http://www.libsdl.org/


  To compile, all you'll need to do is type:

    $ make

  To "uncompile" (eg, remove the executable and ".o" files) to return it
  to a pristine state (as if you just untarred it), you can type:

    $ make clean


RUNNING:

  This program looks for a number of ".png" image files which are
  included in the 'tar' archive.  It simply tries to open them from
  whatever the current directory is, so you'll want to run the
  "explosions" exectuable in the un-tar'd directory.  If it can't
  find the ".png" files, it complains and aborts.

    $ ./explosions &

  A window should open with some fuzzy round objects move around the
  outer edge, and a multi-colored "Y" shape (also made of the round things)
  in the center.


CONTROLS:

  Keyboard:

    ESCAPE                - Quits.

    LEFT/RIGHT ARROW KEYS - Rotates the view of the world around the Y axis.

    UP/DOWN ARROW KEYS    - Rotates the view of the world around the X axis.

    F                     - Toggle 'fade' effect.

    Z                     - Change 'zoom' effect (off, zoom-in, zoom-out).

    R                     - Toggle 'rotation' effect.

    B                     - Toggle 'blur' effect.

    D                     - Toggle 'dissolve' effect.

    H                     - Toggle 'heat' (fire/flame) effect.

    PAGE-UP               - Increase angle of 'rotation' effect.

    PAGE-DOWN             - Decrease angle of 'rotation' effect.

    KEYPAD ARROW KEYS     - Move the center of the 'zoom' effect.
      (ie, 8, 4, 6, 2)


  Mouse:

    BUTTON-PRESS          - Causes some 'sparks' to fly out at the cursor.

    RIGHT-BUTTON-PRESS    - Same as above, but also changes the 'color' of
                            the sparks each time it is clicked.

    MOUSE-MOTION          - If a mouse button is pressed, sparks will
                            continue to fly out at the cursor.  Moving the
                            mouse moves the cursor.

  Note:  The "cursor" is really a virtual spot within the 3D world which
  maps to the position of the mouse within the window.  If the view of
  the world is rotated (with the ARROW KEYS), the cursor is STILL a position
  in the 3D world that maps to the mouse position in the window.

  In other words, depending on the rotation of the view, moving the mouse
  left may cause the cursor (and sparks) to move right... or 'in'... or
  'out' toward the viewer...  Play with it and you'll understand.


COLORS:

  There are different "spark#.png" files in the directory.  Each one
  contains different sprites.  The first few are simply balls of different
  colors with increasing fuzz (alpha, or see-through-ness) towards the outer
  edges...

  One type of ball is generally kind of transparent, and has rainbow colors
  going down it.

  The next shape is a set of numbers, from 1 to 9.  "1" representing the
  smallest and "farthest" sprite.  "9" being the largest, closest.

  Finally, the last PNG contains pictures of bell-peppers (like the ball
  shapes, just another default paintbrush available in "The GIMP" paint
  program).

  RIGHT-BUTTON-PRESS the mouse to rotate through the available colors/shapes
  that can shoot out of the cursor.


EFFECTS:

  Normally, just before all of the objects are drawn in the window, the
  window is erased (cleared to black).  If any of the available effects
  are activated, they will be executed instead.  (See "CONTROLS" above
  for how to turn each one on and off.)

  The available effects are:

    Fade
      The contents of the screen is darkened before the next frame is drawn.

    Zoom
      The contents of the screen is zoomed in or out.  Looks best with "Fade"
      activated simultaneously.  "Zoom-out" looks good with "Blur," too.
      You can change where the zoom is centered by using the KEYPAD ARROW
      keys.

    Rotation
      The contents of the screen is rotated.  You can change the angle it
      is rotated per frame by using the PAGE-UP and PAGE-DOWN keys.

    Blur
      The entire screen is blurred.  (Each pixel becomes the average of
      surrounding pixels.)

    Dissolve
      A random collection of about 1/4th of all pixels on the screen is
      erased (turned black).  This making any moving objects leave a
      dissolving trail.

    Heat
      (aka "Burn," "Fire" or "Flame" - "B" and "F" keys were taken ;^)  )
      Similar to burn, except 1) the new pixels are averages of pixels
      below (causing an illusion of upward motion of the flames),
      2) the new pixels have relatively less blue and slightly less yellow
      (causing them to look more red/orange) and 3) the new pixels are,
      per row, slightly randomly shifted left or right (causing a wavering
      effect).


SOURCE CODE:

  The source-code is HEAVILY documented.  Look at it!  You might even learn
  something!  (Perhaps you'll learn I'm not a very good coder? :) )

  Some #define'd values at the top that you might want to adjust include:

    FPS
      Maximum frames to display per second.  If each frame takes less
      than (1000 / FPS) milliseconds to render, the program will idle
      after each frame to make sure the program doesn't run too fast.

      Note: If your PC takes LONGER to render a frame, the program will
      not skip frames!

    NUM_EXPLOSIONS
      The maximum number of explosion bits that can be handled
      (moved and displayed every frame) at a time.  These are the 'bits'
      that shoot out of the mouse pointer.

    MAX_BITS
      The maximum number of objects that can be displayed on the screen.
      This is the side of the array used for depth-sorting the objects
      (the bit 'queue').

    F_WIDTH
      The width of the window to open and draw in.  (Full Width)

    F_HEIGHT
      The height of the window to open and draw in.  (Full Height)

    BURST_PER_FRAME
      The number of new explosion bits that should come out of the
      mouse pointer, per frame, while the mouse button is held down.

    BURST_SPEED
      The velocity that bits should start at when they 'shoot out'
      of the mouse.

    GRAVITY
      Every frame, the vertical (Y) speed of each explosion bit is
      increased by this amount.  It is used to simulate gravity.

    DISTANCE
      Distance from the 3D origin, in the Z direction (away).

    ASPECT
      (Hard to explain. Care to help?)  When calculating screen positions
      of the 3D objects, this is used to scale down the effect of
      objects' Z values.


CREDITS:

  The idea was sparked (pun intended) from watching "Twisted Metal: Black"
  on the PlayStation 2 ( http://www.scea.com/games/categories/shooter/tmb/ )

  Other ideas taken from Atari 8-bit computer demos, the Atari Jaguar
  CD Virtual Light Machine (by Jeff Minter) and visualization plug-ins for
  audio players (like XMMS)

  Most sprite graphics simply came as brushes with
  "The Gimp" ( http://www.gimp.org/ )

  Encouragement to do the queueing and sorting of objects (for realistic
  object layering in the 3D space) from Emily Stumpf. ( http://emily.dv8.org/ )

  "getpixel()" and "putpixel()" functions (used by the effects) from
  the SDL Library Documentation ( http://sdldoc.csn.ul.ie/ )

  All other code by Bill Kendrick ( http://www.newbreedsoftware.com/bill/ )

