//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-1999 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
//============================================================================

#include <assert.h>
#include <fstream.h>
#include <iostream.h>
#include <strstream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #include <string>

#include <SDL.h>

#include "bspf.hxx"
#include "Console.hxx"
#include "DefProps.hxx"
#include "Event.hxx"
#include "MediaSrc.hxx"
#include "PropsSet.hxx"
#include "SndUnix.hxx"
#include "System.hxx"

#ifdef LINUX_JOYSTICK
  #include <unistd.h>
  #include <fcntl.h>
  #include <linux/joystick.h>

  // File descriptors for the joystick devices
  int theLeftJoystickFd;
  int theRightJoystickFd;
#endif


#define SIDEWAYS
#define ZAURUS

void FillRect(SDL_Surface * surf, SDL_Rect * rect, Uint32 color);


// Globals for SDL stuff
SDL_Surface * theWindow;

// A graphic context for each of the 2600's colors
Uint32 theGCTable[256];

// Enumeration of the possible window sizes
enum WindowSize { Small = 1, Medium = 2, Large = 3 };

// Indicates the current size of the window
WindowSize theWindowSize = Small;  /* [was Medium] */

// Indicates the width and height of the game display based on properties
uInt32 theHeight;
uInt32 theWidth;

// Pointer to the console object or the null pointer
Console* theConsole;

// Event object to use
Event theEvent;

// Indicates if the entire frame should be redrawn
bool theRedrawEntireFrameFlag = true;

// Indicates if the user wants to quit
bool theQuitIndicator = false;

// Indicates what the desired frame rate is
uInt32 theDesiredFrameRate = 60;

// Indicate which paddle mode we're using:
//   0 - Mouse emulates paddle 0
//   1 - Mouse emulates paddle 1
//   2 - Mouse emulates paddle 2
//   3 - Mouse emulates paddle 3
//   4 - Use real Atari 2600 paddles
uInt32 thePaddleMode = 0;

/**
  This routine should be called once the console is created to setup
  the SDL system and open a window for us to use
*/
void setupSDL()
{
  // Get the desired width and height of the display
  theHeight = theConsole->mediaSource().height();
  theWidth = theConsole->mediaSource().width();

  // Figure out the desired size of the window
  int width = theWidth;
  int height = theHeight;
  if(theWindowSize == Small)
  {
    width *= 2;
  }
  else if(theWindowSize == Medium)
  {
    width *= 4;
    height *= 2;
  }
  else if(theWindowSize == Large)
  {
    width *= 6;
    height *= 3;
  }

  // Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    cerr << "ERROR: Cannot init SDL...\n";
    exit(1);
  }

#ifndef SIDEWAYS
  theWindow = SDL_SetVideoMode(width, height, 16, SDL_HWSURFACE);
#else
  theWindow = SDL_SetVideoMode(240, 320, 16, SDL_HWSURFACE);
#endif


  // Set window and icon name:
  char name[512];
  sprintf(name, "Stella: \"%s\"", 
      theConsole->properties().get("Cartridge.Name").c_str());

  SDL_WM_SetCaption(name, "Stella");

  // Allocate colors in the default colormap
  const uInt32* palette = theConsole->mediaSource().palette();
  for(uInt32 t = 0; t < 256; t += 2)
  {
    theGCTable[t] = SDL_MapRGB(theWindow->format,
                               (Uint8)((palette[t] & 0x00ff0000) >> 16),
                               (Uint8)((palette[t] & 0x0000ff00) >> 8),
                               (Uint8)((palette[t] & 0x000000ff)));
    theGCTable[t + 1] = theGCTable[t];
  }
}

/**
  This routine should be called anytime the display needs to be updated
*/
void updateDisplay(MediaSource& mediaSource)
{
  uInt8* currentFrame = mediaSource.currentFrameBuffer();
  uInt8* previousFrame = mediaSource.previousFrameBuffer();
  uInt16 screenMultiple = (uInt16)theWindowSize;
  SDL_Rect dest;

  struct Rectangle
  {
    uInt8 color;
    uInt16 x, y, width, height;
  } rectangles[2][160];

  // This array represents the rectangles that need displaying
  // on the current scanline we're processing
  Rectangle* currentRectangles = rectangles[0];

  // This array represents the rectangles that are still active
  // from the previous scanlines we have processed
  Rectangle* activeRectangles = rectangles[1];

  // Indicates the number of active rectangles
  uInt16 activeCount = 0;


  // This update procedure requires theWidth to be a multiple of four.  
  // This is validated when the properties are loaded.
  for(uInt16 y = 0; y < theHeight; ++y)
  {
    // Indicates the number of current rectangles
    uInt16 currentCount = 0;

    // Look at four pixels at a time to see if anything has changed
    uInt32* current = (uInt32*)(currentFrame); 
    uInt32* previous = (uInt32*)(previousFrame);

    for(uInt16 x = 0; x < theWidth; x += 4, ++current, ++previous)
    {
      // Has something changed in this set of four pixels?
      if((*current != *previous) || theRedrawEntireFrameFlag)
      {
        uInt8* c = (uInt8*)current;
        uInt8* p = (uInt8*)previous;

        // Look at each of the bytes that make up the uInt32
        for(uInt16 i = 0; i < 4; ++i, ++c, ++p)
        {
          // See if this pixel has changed
          if((*c != *p) || theRedrawEntireFrameFlag)
          {
            // Can we extend a rectangle or do we have to create a new one?
            if((currentCount != 0) && 
               (currentRectangles[currentCount - 1].color == *c) &&
               ((currentRectangles[currentCount - 1].x + 
                 currentRectangles[currentCount - 1].width) == (x + i)))
            {
              currentRectangles[currentCount - 1].width += 1;
            }
            else
            {
              currentRectangles[currentCount].x = x + i;
              currentRectangles[currentCount].y = y;
              currentRectangles[currentCount].width = 1;
              currentRectangles[currentCount].height = 1;
              currentRectangles[currentCount].color = *c;
              currentCount++;
            }
          }
        }
      }
    }

    // Merge the active and current rectangles flushing any that are of no use
    uInt16 activeIndex = 0;

    for(uInt16 t = 0; (t < currentCount) && (activeIndex < activeCount); ++t)
    {
      Rectangle& current = currentRectangles[t];
      Rectangle& active = activeRectangles[activeIndex];

      // Can we merge the current rectangle with an active one?
      if((current.x == active.x) && (current.width == active.width) &&
         (current.color == active.color))
      {
        current.y = active.y;
        current.height = active.height + 1;

        ++activeIndex;
      }
      // Is it impossible for this active rectangle to be merged?
      else if(current.x >= active.x)
      {
        // Flush the active rectangle
	if (screenMultiple > 0)
	{
	  dest.x = active.x * 2 * screenMultiple;
	  dest.y = active.y * screenMultiple;
	  dest.w = active.width * 2 * screenMultiple;
	  dest.h = active.height * screenMultiple;
	}
	else
	{
	  dest.x = active.x;
	  dest.y = active.y >> 1;
	  dest.w = active.width;
	  dest.h = active.height / 2;
	}

	FillRect(theWindow, &dest, theGCTable[active.color]);

        ++activeIndex;
      }
    }

    // Flush any remaining active rectangles
    for(uInt16 s = activeIndex; s < activeCount; ++s)
    {
      Rectangle& active = activeRectangles[s];

      if (screenMultiple > 0)
      {
        dest.x = active.x * 2 * screenMultiple;
        dest.y = active.y * screenMultiple;
	dest.w = active.width * 2 * screenMultiple;
	dest.h = active.height * screenMultiple;
      }
      else
      {
	dest.x = active.x;
	dest.y = active.y >> 1;
	dest.w = active.width;
	dest.h = active.height / 2;
      }

      FillRect(theWindow, &dest, theGCTable[active.color]);
    }

    // We can now make the current rectangles into the active rectangles
    Rectangle* tmp = currentRectangles;
    currentRectangles = activeRectangles;
    activeRectangles = tmp;
    activeCount = currentCount;
 
    currentFrame += theWidth;
    previousFrame += theWidth;
  }

  // Flush any rectangles that are still active
  for(uInt16 t = 0; t < activeCount; ++t)
  {
    Rectangle& active = activeRectangles[t];

    if (screenMultiple > 0)
    {
      dest.x = active.x * 2 * screenMultiple;
      dest.y = active.y * screenMultiple;
      dest.w = active.width * 2 * screenMultiple;
      dest.h = active.height * screenMultiple;
    }
    else
    {
      dest.x = active.x;
      dest.y = active.y >> 1;
      dest.w = active.width;
      dest.h = active.height / 2;
    }

    FillRect(theWindow, &dest, theGCTable[active.color]);
  }

  // The frame doesn't need to be completely redrawn anymore
  theRedrawEntireFrameFlag = false;
}

/**
  This routine should be called regularly to handle events
*/
void handleEvents()
{
  struct Switches
  {
    SDLKey scanCode;
    Event::Type eventCode;
  };

  static Switches list[] = {
    { SDLK_1,           Event::KeyboardZero1 },
    { SDLK_2,           Event::KeyboardZero2 },
    { SDLK_3,           Event::KeyboardZero3 },
    { SDLK_q,           Event::KeyboardZero4 },
    { SDLK_w,           Event::KeyboardZero5 },
    { SDLK_e,           Event::KeyboardZero6 },
    { SDLK_a,           Event::KeyboardZero7 },
    { SDLK_s,           Event::KeyboardZero8 },
    { SDLK_d,           Event::KeyboardZero9 },
    { SDLK_z,           Event::KeyboardZeroStar },
    { SDLK_x,           Event::KeyboardZero0 },
    { SDLK_c,           Event::KeyboardZeroPound },

    { SDLK_8,           Event::KeyboardOne1 },
    { SDLK_9,           Event::KeyboardOne2 },
    { SDLK_0,           Event::KeyboardOne3 },
    { SDLK_i,           Event::KeyboardOne4 },
    { SDLK_o,           Event::KeyboardOne5 },
    { SDLK_p,           Event::KeyboardOne6 },
    { SDLK_k,           Event::KeyboardOne7 },
    { SDLK_l,           Event::KeyboardOne8 },
    { SDLK_SEMICOLON,   Event::KeyboardOne9 },
    { SDLK_COMMA,       Event::KeyboardOneStar },
    { SDLK_PERIOD,      Event::KeyboardOne0 },
    { SDLK_SLASH,       Event::KeyboardOnePound },

#ifndef SIDEWAYS
    { SDLK_DOWN,        Event::JoystickZeroDown },
    { SDLK_UP,          Event::JoystickZeroUp },
    { SDLK_LEFT,        Event::JoystickZeroLeft },
    { SDLK_RIGHT,       Event::JoystickZeroRight },
#else
    { SDLK_DOWN,        Event::JoystickZeroRight },
    { SDLK_UP,          Event::JoystickZeroLeft },
    { SDLK_LEFT,        Event::JoystickZeroDown },
    { SDLK_RIGHT,       Event::JoystickZeroUp },
#endif

    { SDLK_SPACE,       Event::JoystickZeroFire }, 
    { SDLK_RETURN,      Event::JoystickZeroFire }, 
    { SDLK_z,           Event::BoosterGripZeroTrigger },
    { SDLK_x,           Event::BoosterGripZeroBooster },

    { SDLK_w,           Event::JoystickZeroUp },
    { SDLK_s,           Event::JoystickZeroDown },
    { SDLK_a,           Event::JoystickZeroLeft },
    { SDLK_d,           Event::JoystickZeroRight },
    { SDLK_TAB,         Event::JoystickZeroFire }, 
    { SDLK_1,           Event::BoosterGripZeroTrigger },
    { SDLK_2,           Event::BoosterGripZeroBooster },

    { SDLK_l,           Event::JoystickOneDown },
    { SDLK_o,           Event::JoystickOneUp },
    { SDLK_k,           Event::JoystickOneLeft },
    { SDLK_SEMICOLON,   Event::JoystickOneRight },
    { SDLK_j,           Event::JoystickOneFire }, 
    { SDLK_n,           Event::BoosterGripOneTrigger },
    { SDLK_m,           Event::BoosterGripOneBooster },

#ifndef ZAURUS
    { SDLK_F1,          Event::ConsoleSelect },
    { SDLK_F2,          Event::ConsoleReset },
    { SDLK_F3,          Event::ConsoleColor },
    { SDLK_F4,          Event::ConsoleBlackWhite },
    { SDLK_F5,          Event::ConsoleLeftDifficultyA },
    { SDLK_F6,          Event::ConsoleLeftDifficultyB },
    { SDLK_F7,          Event::ConsoleRightDifficultyA },
    { SDLK_F8,          Event::ConsoleRightDifficultyB }
#else
    { SDLK_s,           Event::ConsoleSelect },
    { SDLK_r,           Event::ConsoleReset },
    { SDLK_c,           Event::ConsoleColor },
    { SDLK_b,           Event::ConsoleBlackWhite },
    { SDLK_1,           Event::ConsoleLeftDifficultyA },
    { SDLK_2,           Event::ConsoleLeftDifficultyB },
    { SDLK_3,           Event::ConsoleRightDifficultyA },
    { SDLK_4,           Event::ConsoleRightDifficultyB }
#endif
  };

  static Event keyboardEvent;

  SDL_Event event;
  
  while(SDL_PollEvent(&event));
  {
    char buffer[20];
    SDLKey key;

    if((event.type == SDL_KEYDOWN) || (event.type == SDL_KEYUP))
    {
      key = event.key.keysym.sym;
#ifndef ZAURUS
      if((key == SDLK_ESCAPE) && (event.type == SDL_KEYDOWN))
#else
      if((key == SDLK_x) && (event.type == SDL_KEYDOWN))
#endif
      {
        theQuitIndicator = true;
      }

#ifdef 0
      else if((key == SDLK_EQUAL) && (event.type == SDL_KEYDOWN))
      {
        if(theWindowSize == Small)
          theWindowSize = Medium;
        else if(theWindowSize == Medium)
          theWindowSize = Large;
        else
          theWindowSize = Small;

        // Figure out the desired size of the window
        int width = theWidth;
        int height = theHeight;
	if(theWindowSize == Small)
        {
          width *= 2;
        }
        else if(theWindowSize == Medium)
        {
          width *= 4;
          height *= 2;
        }
        else
        {
          width *= 6;
          height *= 3;
        }

        XSizeHints hints;
        hints.flags = PSize | PMinSize | PMaxSize;
        hints.min_width = hints.max_width = hints.width = width;
        hints.min_height = hints.max_height = hints.height = height;
        XSetStandardProperties(theDisplay, theWindow, 0, 0, None, 0, 0, &hints);
        XResizeWindow(theDisplay, theWindow, width, height); 

        theRedrawEntireFrameFlag = true;
      }
#endif
      else
      {
        for(unsigned int i = 0; i < sizeof(list) / sizeof(Switches); ++i)
        { 
          if(list[i].scanCode == key)
          {
            theEvent.set(list[i].eventCode, 
                (event.type == SDL_KEYDOWN) ? 1 : 0);
            keyboardEvent.set(list[i].eventCode, 
                (event.type == SDL_KEYDOWN) ? 1 : 0);
          }
        }
      }
    }
    else if(event.type == SDL_MOUSEMOTION)
    {
      Int32 resistance = 0;

      if(theWindowSize == Small)
      {
#ifndef SIDEWAYS
        int x = (theWidth * 2) - event.motion.x;
#else
        int x = (theWidth * 2) - event.motion.y;
#endif
        resistance = (Int32)((1000000.0 * x) / (theWidth * 2));
      }
      else if(theWindowSize == Medium)
      {
        int x = (theWidth * 4) - event.motion.x;
        resistance = (Int32)((1000000.0 * x) / (theWidth * 4));
      }
      else if(theWindowSize == Large)
      {
        int x = (theWidth * 6) - event.motion.x;
        resistance = (Int32)((1000000.0 * x) / (theWidth * 6));
      }

      // Now, set the event of the correct paddle to the calculated resistance
      if(thePaddleMode == 0)
        theEvent.set(Event::PaddleZeroResistance, resistance);
      else if(thePaddleMode == 1)
        theEvent.set(Event::PaddleOneResistance, resistance);
      else if(thePaddleMode == 2)
        theEvent.set(Event::PaddleTwoResistance, resistance);
      else if(thePaddleMode == 3)
        theEvent.set(Event::PaddleThreeResistance, resistance);
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      if(thePaddleMode == 0)
        theEvent.set(Event::PaddleZeroFire, 1);
      else if(thePaddleMode == 1)
        theEvent.set(Event::PaddleOneFire, 1);
      else if(thePaddleMode == 2)
        theEvent.set(Event::PaddleTwoFire, 1);
      else if(thePaddleMode == 3)
        theEvent.set(Event::PaddleThreeFire, 1);
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
      if(thePaddleMode == 0)
        theEvent.set(Event::PaddleZeroFire, 0);
      else if(thePaddleMode == 1)
        theEvent.set(Event::PaddleOneFire, 0);
      else if(thePaddleMode == 2)
        theEvent.set(Event::PaddleTwoFire, 0);
      else if(thePaddleMode == 3)
        theEvent.set(Event::PaddleThreeFire, 0);
    }
  }

#ifdef LINUX_JOYSTICK
  // Read joystick events and modify event states
  if(theLeftJoystickFd >= 0)
  {
    struct js_event event;

    // Process each joystick event that's queued-up
    while(read(theLeftJoystickFd, &event, sizeof(struct js_event)) > 0)
    {
      if((event.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON)
      {
        if(event.number == 0)
        {
          theEvent.set(Event::JoystickZeroFire, event.value ? 
              1 : keyboardEvent.get(Event::JoystickZeroFire));

          // If we're using real paddles then set paddle event as well
          if(thePaddleMode == 4)
            theEvent.set(Event::PaddleZeroFire, event.value);
        }
        else if(event.number == 1)
        {
          theEvent.set(Event::BoosterGripZeroTrigger, event.value ? 
              1 : keyboardEvent.get(Event::BoosterGripZeroTrigger));

          // If we're using real paddles then set paddle event as well
          if(thePaddleMode == 4)
            theEvent.set(Event::PaddleOneFire, event.value);
        }
      }
      else if((event.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS)
      {
        if(event.number == 0)
        {
          theEvent.set(Event::JoystickZeroLeft, (event.value < -16384) ? 
              1 : keyboardEvent.get(Event::JoystickZeroLeft));
          theEvent.set(Event::JoystickZeroRight, (event.value > 16384) ? 
              1 : keyboardEvent.get(Event::JoystickZeroRight));

          // If we're using real paddles then set paddle events as well
          if(thePaddleMode == 4)
          {
            uInt32 r = (uInt32)((1.0E6L * (event.value + 32767L)) / 65536);
            theEvent.set(Event::PaddleZeroResistance, r);
          }
        }
        else if(event.number == 1)
        {
          theEvent.set(Event::JoystickZeroUp, (event.value < -16384) ? 
              1 : keyboardEvent.get(Event::JoystickZeroUp));
          theEvent.set(Event::JoystickZeroDown, (event.value > 16384) ? 
              1 : keyboardEvent.get(Event::JoystickZeroDown));

          // If we're using real paddles then set paddle events as well
          if(thePaddleMode == 4)
          {
            uInt32 r = (uInt32)((1.0E6L * (event.value + 32767L)) / 65536);
            theEvent.set(Event::PaddleOneResistance, r);
          }
        }
      }
    }
  }

  if(theRightJoystickFd >= 0)
  {
    struct js_event event;

    // Process each joystick event that's queued-up
    while(read(theRightJoystickFd, &event, sizeof(struct js_event)) > 0)
    {
      if((event.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON)
      {
        if(event.number == 0)
        {
          theEvent.set(Event::JoystickOneFire, event.value ? 
              1 : keyboardEvent.get(Event::JoystickOneFire));

          // If we're using real paddles then set paddle event as well
          if(thePaddleMode == 4)
            theEvent.set(Event::PaddleTwoFire, event.value);
        }
        else if(event.number == 1)
        {
          theEvent.set(Event::BoosterGripOneTrigger, event.value ? 
              1 : keyboardEvent.get(Event::BoosterGripOneTrigger));

          // If we're using real paddles then set paddle event as well
          if(thePaddleMode == 4)
            theEvent.set(Event::PaddleThreeFire, event.value);
        }
      }
      else if((event.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS)
      {
        if(event.number == 0)
        {
          theEvent.set(Event::JoystickOneLeft, (event.value < -16384) ? 
              1 : keyboardEvent.get(Event::JoystickOneLeft));
          theEvent.set(Event::JoystickOneRight, (event.value > 16384) ? 
              1 : keyboardEvent.get(Event::JoystickOneRight));

          // If we're using real paddles then set paddle events as well
          if(thePaddleMode == 4)
          {
            uInt32 r = (uInt32)((1.0E6L * (event.value + 32767L)) / 65536);
            theEvent.set(Event::PaddleTwoResistance, r);
          }
        }
        else if(event.number == 1)
        {
          theEvent.set(Event::JoystickOneUp, (event.value < -16384) ? 
              1 : keyboardEvent.get(Event::JoystickOneUp));
          theEvent.set(Event::JoystickOneDown, (event.value > 16384) ? 
              1 : keyboardEvent.get(Event::JoystickOneDown));

          // If we're using real paddles then set paddle events as well
          if(thePaddleMode == 4)
          {
            uInt32 r = (uInt32)((1.0E6L * (event.value + 32767L)) / 65536);
            theEvent.set(Event::PaddleThreeResistance, r);
          }
        }
      }
    }
  }
#endif
}

/**
  Display a usage message and exit the program
*/
void usage()
{
  static const char* message[] = {
    "",
    "Usage: sdl-stella [option ...] file",
    "",
    "Valid options are:",
    "",
    "  -fps <number>           Display the given number of frames per second",
    "  -owncmap                Install a private colormap",
#ifdef LINUX_JOYSTICK
    "  -paddle <0|1|2|3|real>  Indicates which paddle the mouse should emulate",
    "                          or that real Atari 2600 paddles are being used",
#else
    "  -paddle <0|1|2|3>       Indicates which paddle the mouse should emulate",
#endif
    "",
    0
  };

  for(uInt32 i = 0; message[i] != 0; ++i)
  {
    cerr << message[i] << endl;
  }
  exit(1);
}

/**
  Setup the properties set by loading builtin defaults and then a
  set of user specific ones from the file $HOME/.stella.pro

  @param set The properties set to setup
*/
void setupProperties(PropertiesSet& set)
{
  // Try to load the file $HOME/.stella.pro file
  string filename = getenv("HOME");
  filename += "/.stella.pro";

  // See if we can open the file $HOME/.stella.pro
  ifstream stream(filename.c_str()); 
  if(stream)
  {
    // File was opened so load properties from it
    set.load(stream, &Console::defaultProperties());
  }
  else
  {
    // Couldn't open the file so use the builtin properties file
    strstream builtin;
    for(const char** p = defaultPropertiesFile(); *p != 0; ++p)
    {
      builtin << *p << endl;
    }

    set.load(builtin, &Console::defaultProperties());
  }
}

/**
  Should be called to parse the command line arguments

  @param argc The count of command line arguments
  @param argv The command line arguments
*/
void handleCommandLineArguments(int argc, char* argv[])
{
  // Make sure we have the correct number of command line arguments
  if((argc < 2) || (argc > 9))
  {
    usage();
  }

  for(Int32 i = 1; i < (argc - 1); ++i)
  {
    // See which command line switch they're using
    if(string(argv[i]) == "-fps")
    {
      // They're setting the desired frame rate
      Int32 rate = atoi(argv[++i]);
      if((rate < 1) || (rate > 300))
      {
        rate = 60;
      }

      theDesiredFrameRate = rate;
    }
    else if(string(argv[i]) == "-paddle")
    {
      // They're trying to set the paddle emulation mode
      if(string(argv[i + 1]) == "real")
      {
        thePaddleMode = 4;
      }
      else
      {
        thePaddleMode = atoi(argv[i + 1]);
        if((thePaddleMode < 0) || (thePaddleMode > 3))
        {
          usage();
        }
      }
      ++i;
    }
    else
    {
      usage();
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main(int argc, char* argv[])
{
  // Handle the command line arguments
  handleCommandLineArguments(argc, argv);

  // Get a pointer to the file which contains the cartridge ROM
  const char* file = argv[argc - 1];

  // Open the cartridge image and read it in
  ifstream in(file); 
  if(!in)
  {
    cerr << "ERROR: Couldn't open " << file << "..." << endl;
    exit(1);
  }

  uInt8* image = new uInt8[512 * 1024];
  in.read(image, 512 * 1024);
  uInt32 size = in.gcount();
  in.close();

  // Create a properties set for us to use and set it up
  PropertiesSet propertiesSet("Cartridge.Name");
  setupProperties(propertiesSet);

  // Create a sound object for use with the console
  SoundUnix sound;

  // Get just the filename of the file containing the ROM image
  const char* filename = (!strrchr(file, '/')) ? file : strrchr(file, '/') + 1;

  // Create the 2600 game console
  theConsole = new Console(image, size, filename, 
      theEvent, propertiesSet, sound);

  // Free the image since we don't need it any longer
  delete[] image;

#ifdef LINUX_JOYSTICK
  // Open the joystick devices
  theLeftJoystickFd = open("/dev/js0", O_RDONLY | O_NONBLOCK);
  theRightJoystickFd = open("/dev/js1", O_RDONLY | O_NONBLOCK);
#endif

  // Setup X windows
  setupSDL();

  // Get the starting time in case we need to print statistics
  Uint32 startingTime;
  startingTime = SDL_GetTicks();

  uInt32 numberOfFrames = 0;
  for( ; ; ++numberOfFrames)
  {
    // Exit if the user wants to quit
    if(theQuitIndicator)
    {
      break;
    }

    // Remember the current time before we start drawing the frame
    Uint32 before;
    before = SDL_GetTicks();

    // Draw the frame and handle events
    theConsole->mediaSource().update();
    updateDisplay(theConsole->mediaSource());
    SDL_Flip(theWindow);
    handleEvents();

    // Now, waste time if we need to so that we are at the desired frame rate
    Uint32 after;
    for(;;)
    {
      after = SDL_GetTicks();

      uInt32 delta = (uInt32)((after - before));

      if(delta > (1000 / theDesiredFrameRate))
      {
        break;
      }
    }
  }

  Uint32 endingTime;
  endingTime = SDL_GetTicks();
  double executionTime = (endingTime - startingTime) / 1000.0;
  double framesPerSecond = numberOfFrames / executionTime;

  cout << endl;
  cout << numberOfFrames << " total frames drawn\n";
  cout << framesPerSecond << " frames/second\n";
  cout << theConsole->mediaSource().scanlines() << " scanlines in last frame\n";
  cout << endl;

  delete theConsole;
}


void FillRect(SDL_Surface * surf, SDL_Rect * rect, Uint32 color)
{
#ifndef SIDEWAYS
  SDL_FillRect(surf, rect, color);
#else
  SDL_Rect newRect;

  newRect.x = 240 - rect->y - rect->h;
  newRect.y = rect->x;
  newRect.w = rect->h;
  newRect.h = rect->w;

  SDL_FillRect(surf, &newRect, color);
#endif
}

