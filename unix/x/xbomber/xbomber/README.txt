README.txt

For X-Bomber v. 0.8

by Bill Kendrick
kendrick@zippy.sonoma.edu

New Breed Software
http://zippy.sonoma.edu/kendrick/nbs/

June 19, 1998


WHAT IS X-BOMBER?

  X-Bomber is a multiplayer game for the X-Window system.  The concept is
  loosely based on the game "Super Bomberman" for the Super Nintendo
  game system.

  Up to four players can play simultaneously.  The game runs as one client
  which connects to multiple X displays (servers).


ABOUT THIS VERSION

  Version 0.8, released 19.Jun.1998

  This is the tenth beta release of "X-Bomber."  If you have problems,
  please e-mail me.  <kendrick@zippy.sonoma.edu>

  Changes since v. 0.7 include:

    * Two players can play on one keyboard!  Use "SAME" as a server to
      share the previous player's keyboard.
    * A bug which would make X-Bomber die right when you started has been
      fixed.  Thanks to Pascal Rigaux <rigaux@irisa.fr> for pointing out
      my error!
    * A bug which could cause some machines to Seg. Fault when playing
      3 or less human players is fixed.
    * Windows are updated in a different order, thus removing the speed
      advantage lower-numbered players had over higher-numbered players.
    * You can now specify a particular graphics set in "-color" mode!
      Included in this version is the "jungle" set.
    * Shields and invisibility temporary-upgrades have been added.
    * GRAPHICS.txt, LEVELS.txt and SOUND.txt have been created to document
      how to tweak the graphics, levels and sound in X-Bomber.


WHAT'S THE OBJECT?

  The object of X-Bomber is to be the last one standing.  In other words,
  kill all of your opponents (and/or let them kill each other or themselves).
  If you survive a level, you gain a point and, more importantly, keep all
  of the nifty upgrades you've gathered during the level.

  To do all this, you use bombs! (Yay!)


COMPILING:

  To compile the game, simply type "make".  You may need to edit the
  "makefile" first, though, in case your system is configured differently.

  Some useful settings in the "makefile" include "CFLAGS" and "XLIB".
  The former can be used in case your X11 ".h" includes aren't readily
  available to your C compiler, and the latter lets you tell your
  C compiler where the X11 objects are stored.  You'll almost definitely
  need to set this one.


RUNNING THE GAME:

  To run the game, you can simply run "xbomber" with no arguments to
  play a one-player game on your current X display:

    % ./xbomber

  Or you can specify one to four displays on which to play the game.
  For example, to play a game where player 1 is on "x.computer.domain",
  and player 2 is on "my.home.computer", you would probably say:

    % ./xbomber x.computer.domain:0.0 my.home.computer:0.0

  Note that you can probably replace the address of YOUR display with
  the "DISPLAY" environment variable, if it's set:

    % ./xbomber $DISPLAY my.home.computer:0.0

  To play with two players on the same keyboard, use "SAME" (all uppercase)
  after a player's display.  For example, to play two players on one
  computer, and a third on a different computer, you could go:

    % ./xbomber my.home.computer:0.0 SAME sonoma.else.com:0.0

  You can select the starting level to play on by appending "-L#", where
  "#" is a number between 1 and 99:

    % ./xbomber $DISPLAY someone.else.com:0.0 -L35

  If you wish to play the game using sound (using the machine's "/dev/audio"
  device), append "-sound" to the end of the command line:

    % ./xbomber $DISPLAY my.home.computer:0.0 -sound

  Remember that only the machine that the game is actually RUNNING on
  will be making any noise.  The X-Window protocol doesn't (yet!) support
  network sound.  A future version of X-Bomber may use the "Network
  Audio System" (NAS) for multiplayer audio.  "-sound" must appear AFTER
  any "-L" value.

  Finally, you can choose to use full-color graphics, or the simpler
  monochrome graphics.  Use "-color" or "-c":

    % ./xbomber $DISPLAY -color

  Color graphics take longer to load, and may cause the game to run more
  slowly on some machines, but it look a lot better.  Also, different
  graphics sets can be created and used.  Specify the graphics set after
  the "-color".  A directory containing PPM graphics files needs to exist
  for this to work.

  The alternate graphics set that comes with X-Bomber 0.8 is the "Jungle":

    % ./xbomber $DISPLAY -color jungle


  A few other command-line switches exist, too.  They simply display
  some information and then quit back to the shell without starting a game:

  For your copy of X-Bomber's version number, use "-version" (or simply
  "-v"):

    % ./xbomber -v

  For a brief help display, append "-help" (or simply "-h"):

    % ./xbomber -h


PLAYING THE GAME

  CONTROLS:

    X-Bomber is controlled with the keyboard.  The following movement controls
    are available:

        Normal Player                         "SAME" keyboard user:
        -----------------------------------------------------------
      * Up-Arrow:                                       S
        Move your person up a space.

      * Down-Arrow:                                     X
        Move your person down a space.

      * Left-Arrow:                                     Z
        Move your person left a space.

      * Right-Arrow:                                    C
        Move your person right a space.

    On the screen, your person faces the direction you last (tried to) move(d).

    These controls are also quite useful:

      * SPACE:                                         TAB
        Drop a bomb.

      * RETURN/ENTER:                                   A
        Detonate a radio-bomb (described below).

    And finally:

      * Q:
        Quit the game.  (It kills the entire game, so everyone leaves!)


  MOUSE-BASED HELP:

    While you're playing, you can use the bomb-shaped mouse-pointer to
    click an object on the screen.  A short description (ie, "Box", or
    "Bomb") will appear over the item to remind you what it is.
    The description will disappear after a moment.


  ON THE SCREEN:

    The game area consists of a room which you view from above in your window.
    Each level typically starts out with the room containing the following
    things:

      * Blank spaces -
        ...where you can walk.
        In normal graphics mode, blank spaces are simply black.
        In color graphics mode, different levels have different "floor"
        patterns on them.

      * Blocks -
        Indestructable blue blocks laid out in a pattern and along the outer
        edges of the room.  Less and less of these appear the higher
        the level you are on.

      * Boxes -
        Red wooden boxes.  They can be destroyed, and often provide upgrades
        (or "goodies") in their place.

     Each level has a different "map" associated with it.  This map describes
     where the indestructable blocks appear.  Sometimes they are arranged
     in a pattern, sometimes more randomly, and sometimes (just for fun)
     in the shape of pictures. :)


  BOMB TYPES:

    Each of the four players can drop bombs on the screen.  There are currently
    two types of bombs:

      * Fuse bombs -
          * The standard.  You start out with this type at the beginning of
            the game and after you've died.
          * They look like little grey round bombs.
          * These bombs go off a few seconds after being dropped.

      * Radio bombs -
          * You get these by picking up a "radio bomb" upgrade
            (a green icon with a radio bomb inside of it).
          * These bombs lay dormant until the player who set them send a signal
            for them to go off.  (Use the RETURN or ENTER key.)
          * In normal graphics mode, they look like fuse bombs with "X"'s
            in them.
          * In color mode, they look like fuse bombs with a radio and
            antenna attached to them.

    When bombs go off, they explode in four directions:
    up, down, left and right.  An explosion noise will be heard.

    If you get hit by any of this explosion, you die.  (When you die,
    an "uugh!" will be heard.)

    If another bomb gets hit by an explosion, it goes off immediately.

    Each player can only drop a certain number of bombs at a time.
    When you start the game, and after you've died, you can only drop one
    bomb at a time.  You must wait for each bomb to go off before you can
    lay another.

    To be able to drop multiple bombs at a time, you need to pick up a
    "more-bombs" upgrade (a green icon with a fuse bomb inside of it).

    You can only drop multiple fuse bombs, however.  Only one radio bomb
    can be laid at a time.  Getting a "more-bombs" icon cancels your
    radio-bomb, if you have one.  Similarly, getting a "radio-bomb" icon
    cancels your "more-bombs!"


  UPGRADES:

    As described above, these two upgrades change your bomb type:

      * More-bombs -
        * A green icon with a fuse bomb in it.
        * Increases how many bombs you can drop at a time.
        * Cancels out radio-bomb.

      * Radio-bomb -
        * A green icon with a radio bomb in it.
        * Changes you bomb type to radio bomb.
        * Cancels out any more-bombs.

    A number of other upgrades help you out as well:

      * Pusher -
        * A green icon with a bull-dozer inside it.
        * This lets you push boxes around, one at a time.  Simply walk into the
          box and it will move forward.  (If you've ever played a game like
          "Soko-ban," you understand how this works.)  A "click" will be heard.

      * Kicker -
        * A green icon with a foot kicking a bomb.
        * This lets you kick bombs out of the way.  Simply walk into the
          bomb and it will move forward.  A "tink!" will be heard.
          If you kick a bomb into a non-blank spot (a box, upgrade, etc.),
          it continues to move the same direction "over" the object(s) until
          it finds a blank spot.
        * You can EVEN kick a bomb from one side of the game area to the
          other!
        * Bombs which are moving in this way don't go off until after
          they've landed.

      * More-fire -
        * A green icon with a happy flame in it.
        * This makes your bombs' explosions larger by one unit in each
          direction.
        * You can have explosions which are up to 8 units in each direction!

      * Total-fire -
        * A green icon with a mad-looking flame in it.
        * This instanlty makes your bombs' explosions the maximum size
          (8 units).

    You keep all upgrades until you die.


  STATUS LINE:

    At the bottom of the screen is a status bar.  It lists, from left to
    right:

      * Each player and their score.
        * A "person" represents a player who is still alive.
        * An explosion represents a player who's died in the current level.
        * The white "person" represents you.  The others are your opponents.

      * Your bomb status.
        * A fuse bomb followed by "x1", "x2", etc., lets you know you have
          fuse bombs, and how many you can drop at a time.
        * A radio bomb lets you know you have radio bombs.

      * Your bomb power.
        * A green flame face followed by "x2", "x3", etc., lets you know
          how large your explosions are.  (More about this later.)
        * A mad-looking outlined face followed by "x8" lets you know you
          have the maximum explosive power.  (More about this later.)

      * If you have pusher.
        * The green "pusher" icon will appear if you have the pusher upgrade.

      * If you have kicker.
        * The green "kicker" icon will appear if you have the kicker upgrade.

      * A text message.
        It typically states the current level, but for a few moments at the
        end of a level, it states who won (or if it was a draw).


  OTHER OBJECTS:

    Along with (or instead of!) indestructable blocks and destroyable boxes,
    some levels of the game provide other interesting objects!

      * TNT boxes:
        * They look very similar to normal boxes, but have "TNT" written
          on them.
        * They act just like boxes (you can destroy them, they provide
          "goodies," and you can push them if you have the "pusher" upgrade),
          but they also happen to explode when they're destroyed, so don't
          stand too close!

      * Warps:
        * They look like concentric circles on the ground.
        * When you walk into these, you're randomly moved somewhere
          on the screen.  A "boing!" will be heard. 
        * You'll possibly be warped somewhere useful, but also possibly
          into the line of an explosion!
        * They CAN be destroyed.

      * Nuke:
        * They look like green upgrades with a "radioactive" symbol inside.
        * When you walk into this, all of the bombs on the screen explode
          right then. (Be careful!)

      * Invert:
        * Green upgrades which are diagonally half green and half black.
        * When you walk into this, all of the light blue blocks turn into
          boxes, all of the boxes turn into light blue blocks, all of the
          dark blue blocks turn into TNT and all of the TNT turns into
          dark blue blocks.  Hit another one to "undo" this.
        * A strange "whistle" noise will be heard.

      * Mushroom:
         * Green upgrades which have a little mushroom in them.
         * When you walk onto this, you go crazy for a few moments.
           And cannot control your character.  Good luck! :)

      * Death:
        * A mean-looking yellow face.  A meniacle laugh will he heard when
          he appears.
        * Death picks a player and chases them around the screen.
          If that player dies, death chooses another to attack.
        * When death touches a player, that player dies, (he eats them!)
          and then spits out a pile of bones.
        * When death touches bombs or upgrades ("goodies"), death eats them.
        * Death pauses for a moment when he eats (his facial expression
          will change for a moment).
        * Death appears randomly.  He rarely appears in levels 1 through 10,
          and appears much more often in levels 11 and up.
        * He cannot be stopped!

      * Bones:
        * As stated above, Death spits out a pile of bones when he eats a
          player.
        * Bones are a little pile of rubble with a skull sitting on top.
        * If a player touches the bones, one of two things may occur:
          * The player gains Kicker, Pusher and maximum Fire (COOL!), or...
          * The player acts as if they at a mushroom (BAD!).
        * Eat bones as your own risk.  You MAY be rewarded well, or you
          may just go nuts. >:^)  (This is of course not very safe with
          Death roaming around the screen!)


  WHEN THE LEVEL ENDS:

    The level ends when:

      * There is only one player left.
        * All of the boxes explode, leaving a large collection of goodies.
        * All loose bombs go away, and the winning player can no longer
          drop any bombs, making the game area safe.
        * The player gets a few moments with which to collect as many
          goodies as possible.
        * A spiffy music plays.

      * There are no more players left.
        * The same as above, but since there are no players, nobody can get
          any goodies.

    Another kind of ending occurs when the level's time runs out:

      * Just before "time runs out," new indestructable blocks start appearing
        on the screen, starting from the outer edge and spiraling inwards.
      * If you're hit by one of these blocks, you die.
      * It's of course best to be in the very center when this happens!

      This still causes one of the two endings listed above, of course.


CREDITS:

  X-Bomber

  (c) 1998

  by Bill Kendrick
  kendrick@zippy.sonoma.edu

  New Breed Software
  http://zippy.sonoma.edu/kendrick/nbs/

  7673 Melody Drive
  Rohnert Park, CA 94928 USA

  Based on Super Bomberman for the Super Nintendo.  (c) Who? When? I dunno!


THANKS:

  * Thanks to Pascal Rigaux for fixing a minor bug which kept many folks
    from playing this game (and a number of others)!
  * Thanks to my two major fans at Sonoma State University:
      * Matt Hermes     (player 2, bomb-kicker extraordenaire)
      * Brian Mordecai  (player 3, Death's special friend >:^) )


DO YOU WANT TO HELP?

  Send me a note, money or any other form of encouragement.  Positive feedback
  always gets my programming juices flowing! :)  (And maybe one day I'll
  afford to replace my bike which was stolen! <:^( )
