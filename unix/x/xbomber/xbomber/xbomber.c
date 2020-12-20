/*
  xbomber.c
  
  XBomber 0.8
  
  A 4-player networked "bombing" game (based loosely on "Super Bomberman")
  for X-Window.
  
  by Bill Kendrick
  New Breed Software
  kendrick@zippy.sonoma.edu
  http://zippy.sonoma.edu/kendrick/nbs/unix/x/xbomber/  <- latest info/download
  
  January 28, 1998 - June 19, 1998
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "window.h"
#include "connect.h"
#include "hints.h"
#include "visual.h"
#include "gc.h"
#include "color.h"
#include "randnum.h"
#include "text.h"


/* Keyboard mapping: */

#define MAIN_LEFT      "Left"
#define MAIN_RIGHT     "Right"
#define MAIN_UP        "Up"
#define MAIN_DOWN      "Down"
#define MAIN_FIRE      " "
#define MAIN_RADIOFIRE "Return"

#define SAME_LEFT      "Z"
#define SAME_RIGHT     "C"
#define SAME_UP        "S"
#define SAME_DOWN      "X"
#define SAME_FIRE      "Tab"
#define SAME_RADIOFIRE "A"


/* Constraints: */

#define MAX_PLAYERS 4           /* DON'T TOUCH! */
#define CHANCE_OF_UPGRADE 3     /* upgrades 1/3rd of the time */
#define MAX_FIRE_SIZE 8         /* explosions max 15x15 (7+1+7,7+1+7) */
#define BOMB_COUNTDOWN 160      /* how long til bombs go off */
#define MUSHROOM_START_TIME 100 /* how long mushrooms last */


/* Object graphics abstractions: */

#define OBJ_DIRT 0
#define OBJ_MAN_DOWN 1
#define OBJ_MAN_RIGHT 2
#define OBJ_MAN_UP 3
#define OBJ_MAN_LEFT 4
#define OBJ_BLOCK 5
#define OBJ_BLOCK2 6
#define OBJ_BOX 7
#define OBJ_BOMB 8
#define OBJ_RADIO_BOMB 9
#define OBJ_EXPLOSION1 10
#define OBJ_EXPLOSION2 11
#define OBJ_MOREFIRE 12       /* all upgrades must be consecutive! */
#define OBJ_TOTALFIRE 13
#define OBJ_PUSHER 14
#define OBJ_KICKER 15
#define OBJ_MOREBOMB 16
#define OBJ_RADIOUPGRADE 17
#define OBJ_NUKE 18
#define OBJ_INVERT 19
#define OBJ_MUSHROOM 20
#define OBJ_SHIELD 21
#define OBJ_INVISIBLE 22      /* --- */
#define OBJ_WARP 23
#define OBJ_TNT 24
#define OBJ_DEATH 25
#define OBJ_DEATH_EATING 26
#define OBJ_BONES 27
#define OBJ_TITLE 28
#define OBJ_YOU_DOWN 29       /* color-specific stuff must be last! */
#define OBJ_YOU_RIGHT 30
#define OBJ_YOU_UP 31
#define OBJ_YOU_LEFT 32       /* --- */
#define OBJ_CHECKER1 33
#define OBJ_CHECKER2 34
#define OBJ_CROSS 35
#define NUM_OBJECTS 36

#define FIRST_UPGRADE OBJ_MOREFIRE
#define LAST_UPGRADE OBJ_INVISIBLE

#define NUM_UPGRADES (LAST_UPGRADE - FIRST_UPGRADE + 1)


/* Object graphics filenames: */

char * object_names[NUM_OBJECTS] = {
  "dirt", "man_down", "man_right", "man_up", "man_left", "block", "block2",
  "box", "bomb", "radio_bomb", "explosion1", "explosion2",
  "morefire", "totalfire", "pusher", "kicker", "morebomb",
  "radioupgrade", "nuke", "invert", "mushroom", "shield", "invisible",
  "warp", "tnt", "death", "death_eating", "bones", "title",
  /* color-only pixmaps: */
  "you_down", "you_right", "you_up", "you_left",
  "checker_dirt1", "checker_dirt2", "cross_dirt"};


/* Object colors: */

#define YOU_COLOR "LightYellow"
#define THEM_COLOR "light goldenrod"
#define GOODIE_COLOR "green"

char * object_colors[NUM_OBJECTS] = {
  "Black", THEM_COLOR, THEM_COLOR, THEM_COLOR, THEM_COLOR,
  "deep sky blue", "blue", 
  "red", "grey", "grey", "yellow", "yellow",
  GOODIE_COLOR, GOODIE_COLOR, GOODIE_COLOR, GOODIE_COLOR, GOODIE_COLOR,
  GOODIE_COLOR, GOODIE_COLOR, GOODIE_COLOR, GOODIE_COLOR, GOODIE_COLOR,
  GOODIE_COLOR, "white", "red",
  "yellow", "yellow", "white", "orange",
  /* color-only pixmaps: */
  "white", "white", "white", "white",
  "black", "black", "black"};


/* Object descriptions (for mouse-help): */

char * object_descs[NUM_OBJECTS] = {
  "(Ground)", "Player", "Player", "Player", "Player", "Block", "Block",
  "Box", "Bomb", "Radio Bomb", "Explosion",
  "Explosion", "More-Fire Upgrade", "Total-Fire Upgrade", "Pusher Upgrade",
  "Kicker Upgrade", "More-Bombs Upgrade", "Radio-Bomb Upgrade",
  "Nuker", "Invert", "Mushroom", "Shields", "Invisible", 
  "Warp Spot", "TNT Box", "Death", "Death", "Bones", ""
  "", "", "", "", "", "", ""};


/* Plain sound abstractions: */

#define SND_EXPLODE 0
#define SND_GOT 1
#define SND_KICKER 2
#define SND_OUCH1 3
#define SND_OUCH2 4
#define SND_OUCH3 5
#define SND_PUSHER 6
#define SND_WARP 7
#define SND_LEVELEND1 8
#define SND_LEVELEND2 9
#define SND_LEVELEND3 10
#define SND_LEVELEND4 11
#define SND_LAUGH 12
#define SND_INVERT 13
#define NUM_SOUNDS 14
#define NUM_LEVELEND_SNDS 4


/* Plain sound filenames: */

char * sound_names[NUM_SOUNDS] = {
  "explode", "got", "kicker", "ouch1", "ouch2", "ouch3", "pusher", "warp",
  "levelend1", "levelend2", "levelend3", "levelend4", "laugh", "invert"};


/* Level number sound abstractions (barely needed): */

#define NSND_LEVEL 0
#define NSND_1 1
#define NSND_2 2
#define NSND_3 3
#define NSND_4 4
#define NSND_5 5
#define NSND_6 6
#define NSND_7 7
#define NSND_8 8
#define NSND_9 9
#define NSND_10 10
#define NSND_11 11
#define NSND_12 12
#define NSND_13 13
#define NSND_14 14
#define NSND_15 15
#define NSND_20 16
#define NSND_30 17
#define NSND_40 18
#define NSND_50 19
#define NSND_60 20
#define NSND_70 21
#define NSND_80 22
#define NSND_90 23
#define NSND_TEEN 24
#define NUM_NSOUNDS 25


/* Level number sound filenames: */

char * nsound_names[NUM_NSOUNDS] = {
  "level", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
  "13", "14", "15", "20", "30", "40", "50", "60", "70", "80", "90", "teen"};


/* Sound space: */

unsigned char au_file_data[NUM_SOUNDS][25000],
  nau_file_data[NUM_NSOUNDS][6000];

int sound_len[NUM_SOUNDS], nsound_len[NUM_NSOUNDS];


/* Global X-Window Variables: */

typedef struct color_type {
  unsigned int red;
  unsigned int green;
  unsigned int blue;
  unsigned long pixel;
  int owner;
} color_type;

int num_seen_colors;
char server[MAX_PLAYERS][512];
Cursor cursor[MAX_PLAYERS];
XColor blackxcolor[MAX_PLAYERS], whitexcolor[MAX_PLAYERS];
Display * display[MAX_PLAYERS];
Colormap colormap[MAX_PLAYERS];
color_type color_list[256];
Window window[MAX_PLAYERS], root[MAX_PLAYERS];
GC whitegc[MAX_PLAYERS], blackgc[MAX_PLAYERS];
GC colorgcs[MAX_PLAYERS][NUM_OBJECTS], yougc[MAX_PLAYERS];
XFontStruct * font[MAX_PLAYERS];
int fh[MAX_PLAYERS];
int screen[MAX_PLAYERS], black[MAX_PLAYERS], white[MAX_PLAYERS],
  has_color[MAX_PLAYERS];
Pixmap object_pixmaps[MAX_PLAYERS][NUM_OBJECTS],
  object_masks[MAX_PLAYERS][NUM_OBJECTS];
Pixmap cursor_pixmap[MAX_PLAYERS], cursor_mask[MAX_PLAYERS];
Visual * temp_visual[MAX_PLAYERS];
char graphicsset[512];


/* Level stats: */

#define LEV_MOD 10

int lev_usebox[LEV_MOD] =  {1, 1, 1, 1, 0, 1, 1, 0, 0, 0};
int lev_usetnt[LEV_MOD] =  {0, 0, 1, 1, 1, 0, 1, 1, 0, 0};
int lev_usewarp[LEV_MOD] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 0};


/* Arena size:  (DON'T TOUCH!) */

#define MAP_WIDTH 17
#define MAP_HEIGHT 17
#define WIDTH (MAP_WIDTH * 32)
#define HEIGHT ((MAP_HEIGHT + 1) * 32)


/* Title size and position: */

#define TITLE_WIDTH 300
#define TITLE_HEIGHT 200
#define TITLE_LEFT ((WIDTH - TITLE_WIDTH) / 2)
#define TITLE_TOP ((HEIGHT - TITLE_HEIGHT) / 2)


/* How many randomly placed boxes to place in a level: */

#define HOW_MANY_BOXES 100
#define HOW_MANY_TNTS 50
#define HOW_MANY_WARPS 25


/* How many bombs and explosions max on the screen? */

#define MAX_BOMBS 20
#define MAX_EXPLOSIONS (MAP_WIDTH * MAP_HEIGHT)


/* How long a player gets after all other players are dead: */

#define FREETIME 400


/* Speed controls: */

#define FRAMERATE 10000


/* Size of sound buffer queue: */

#define SOUND_BUF 40960


/* Background modes: */

#define BKG_NORMAL 0
/* ... normal pops up 3 times as much as others */
#define BKG_CHECKER 3
#define BKG_CROSS 4
#define NUM_BACKGROUND_MODES 5


/* Typedefs: */

typedef struct player_type {
  int bombs_out, bombs_max, fire_size;
  int alive;
  int x, y;
  int score;
  int shape;
  int seenmap[MAP_HEIGHT][MAP_WIDTH];
  int radiobomb, pusher, kicker;
  int radio_detonate;
  int mushroomtime, shieldtime, invisibletime;
  int cmp_time, cmp_dir;
} player_type;

typedef struct note_type {
  int alive, time;
  int x, y;
  char text[128];
} note_type;

typedef struct bomb_type {
  int alive, time;
  int x, y;
  int shape, size;
  int owner;
  int moving, xm, ym;
} bomb_type;

typedef struct explosion_type {
  int alive, time;
  int x, y;
} explosion_type;

typedef struct death_type {
  int alive, time;
  int x, y;
  int seek;
  int shape;
} death_type;


/* Global game Variables: */

int level, use_sound, num_servers, winner, leveltime, cmp_movetoggle, toggle,
  ttoggle, use_color, title_mode;
int background_mode;
int map[MAP_HEIGHT][MAP_WIDTH], seenmap[MAP_HEIGHT][MAP_WIDTH];
int xms[5], yms[5];
char message[128];
struct bomb_type bomb[MAX_BOMBS];
struct explosion_type explosion[MAX_EXPLOSIONS];
struct player_type player[MAX_PLAYERS];
struct note_type notes[MAX_PLAYERS];
struct death_type death;
int same[MAX_PLAYERS];
int sound_position;
int pid;
int pipefds[2];
FILE * sound_fs;
int devaudio_fd;
unsigned char sound_data[SOUND_BUF];


/* Local function prototypes: */

void eventloop(void);
void setup(int argc, char * argv[]);
void Xsetup(int pln);
void Xsetup_windows(int pln);
unsigned long MyAllocNamedColor(Display *display, Colormap colormap,
                                char* colorname, unsigned long default_color,
                                int has_color);
void playsound(int whichfile);
void playnsound(int whichfile);
void drawblock(int pln, int x, int y, int c);
void makeexplosion(int x, int y, int size);
int addexplosion(int x, int y);
int moveok(int pln, int xm, int ym);
void redrawstatus(int pln);
int dropbomb(int pln);
void initlevel(void);
void initplayergoodies(int i);
void addexplosion_atomic(int x, int y);
void control_computer(int pln);
void make_death_seek(void);
int empty(int x, int y);
void kill_bomb(int i);
void loadobject(int pln, int i);
void color_clear(void);
int color_seen(unsigned int red, unsigned int green, unsigned int blue);
int color_add(unsigned int red, unsigned int green, unsigned int blue,
              unsigned long pixel, int owner);
int real(int pln);


/* ---- MAIN FUNCTION ---- */

int main(int argc, char * argv[])
{
  int i;
  
  
  /* Program setup: */
  
  setup(argc, argv);
  randinit();
  
  
  if (strcmp(server[0], "SAME") == 0)
    {
      fprintf(stderr, "Player 1 doesn't have a display!  (SAME as WHAT?)\n");
      exit(1);
    }
  
  
  for (i = 1; i < num_servers; i++)
    {
      if (strcmp(server[i], "SAME") == 0)
	{
	  if (strcmp(server[i - 1], "SAME") == 0)
	    {
	      fprintf(stderr, "Player %d can't use the same display as ",
		      i + 1);
	      fprintf(stderr, "players %d and %d!\n", i, i - 1);
	      fprintf(stderr, "(No two SAME's in a row!)\n");
	      exit(1);
	    }
	  
	  same[i] = 1;
	}
    }
  
  
  /* Connect to the X Servers and draw the program's windows: */
  
  for (i = 0; i < num_servers; i++)
    {
      if (same[i] == 0)
	Xsetup_windows(i);
    }
  
  for (i = 0; i < num_servers; i++)
    {
      if (same[i] == 0)
	{
	  Xsetup(i);
	  same[i] = 0;
	}
      else
	{
	  /* Copy last player's X info. into this player: */
	  
	  display[i] = display[i - 1];
	  font[i] = font[i - 1];
	  fh[i] = fh[i - 1];
	  black[i] = black[i - 1];
	  white[i] = white[i - 1];
	  window[i] = window[i - 1];
	  whitegc[i] = whitegc[i - 1];
	  blackgc[i] = blackgc[i - 1];
	}
    }
  
  /* Run the main loop: */
  
  title_mode = 1;
  
  eventloop();
  
  
  printf("\n\nThanks for playing X-Bomber, by Bill Kendrick!\n\n");
  
  printf("Send comments, donations, questions and bug-reports to:\n\n");
  
  printf("New Breed Software\n");
  printf("c/o Bill Kendrick\n");
  printf("1530 Armstrong Ave. #47\n");
  printf("Novato, CA 94945, USA\n\n");
  
  printf("E-mail: kendrick@zippy.sonoma.edu\n");
  printf("   Web: http://zippy.sonoma.edu/kendrick/\n");
  printf("\n\n");
  
  printf("FINAL SCORES:\n");
  for (i = 0; i < MAX_PLAYERS; i++)
    printf("Player %d: %d\n", i + 1, player[i].score);
  
  printf("\n");
}


/* --- MAIN EVENT LOOP --- */

void eventloop(void)
{
  int i, j, e_loop, pln, done, x, y, found, c, erase_it, killed_one,
    foundblocker, freetime, num_alive, humans_alive, z, best, score,
    mod_counter;
  long time_padding;
  int newblocks_x, newblocks_y, newblocks_dir;
  int newblocks_top, newblocks_bottom, newblocks_left, newblocks_right;
  char string[128];
  XEvent event;
  char key[1024];
  KeySym keysym;
  XComposeStatus composestatus;
  struct timeval now, then;
  
  
  /* Init player stuff: */
  
  for (i = 0; i < MAX_PLAYERS; i++)
    {
      player[i].score = 0;
      
      initplayergoodies(i);
    }
  
  
  initlevel();

  
  /* MAIN LOOP: */
  
  toggle = 0;
  ttoggle = 0;
  cmp_movetoggle = 0;
  mod_counter = 0;
  done = False;
  
  do
    {
      /* Toggle our toggle switches: */
      
      toggle = 1 - toggle;
      
      if (toggle)
	ttoggle = 1 - ttoggle;
      
      cmp_movetoggle = (cmp_movetoggle + 1) % 5;
      
      mod_counter = (mod_counter + 1);
      
      
      gettimeofday(&then, NULL);
      
      
      /* Count how many HUMAN players are currently alive: */
      
      humans_alive = 0;
      
      for (pln = 0; pln < num_servers; pln++)
	if (player[pln].alive)
	  humans_alive++;
      
      
      /* Human controlled players: */
      
      for (pln = 0; pln < num_servers; pln++)
	{
	  /* Get and handle events: */
	  
	  strcpy(key, "");
	  
	  while (XPending(display[pln]))
	    {
	      XNextEvent(display[pln], &event);
	      
	      if (event.type == KeyPress)
		{
		  /* Get the key's name: */
		  
		  XLookupString(&event.xkey, key, 1, &keysym,
				&composestatus);
		  
		  if (XKeysymToString(keysym) != NULL)
		    strcpy(key, XKeysymToString(keysym));
		  
		  
		  /* (Turn off title if it's on): */
		  
		  if (title_mode == 1)
		    {
		      title_mode = 0;
		      
		      for (i = 0; i < num_servers; i++)
			{
			  for (y = TITLE_TOP / 32;
			       y < (TITLE_TOP + TITLE_HEIGHT + fh[i] * 6) /
				 32 + 1; y++)
			    {
			      for (x = TITLE_LEFT / 32;
				   x < (TITLE_LEFT + TITLE_WIDTH) / 32 + 1;
				   x++)
				{
				  player[i].seenmap[y][x] = -1;
				}
			    }
			}
		    }
		}
	      else if (event.type == Expose)
		{
		  for (y = event.xgraphicsexpose.y / 32;
		       y <= (event.xgraphicsexpose.y +
			     event.xgraphicsexpose.height) / 32 &&
			 y < MAP_HEIGHT; y++)
		    {
		      for (x = event.xgraphicsexpose.x / 32;
			   x <= (event.xgraphicsexpose.x +
				 event.xgraphicsexpose.width) / 32 &&
			     x < MAP_WIDTH; x++)
			{
			  player[pln].seenmap[y][x] = -1;
			}
		    }
		  
		  redrawstatus(pln);
		}
	      else if (event.type == ButtonPress)
		{
		  if (event.xbutton.y < MAP_HEIGHT * 32)
		    {
		      c = player[pln].seenmap[event.xbutton.y / 32]
			[event.xbutton.x / 32];
		      
		      if (notes[pln].alive == 1)
			{
			  player[pln].seenmap[notes[pln].y / 32]
			    [notes[pln].x / 32] = -1;
			  player[pln].seenmap[notes[pln].y / 32]
			    [notes[pln].x / 32 + 1] = -1;
			}
		      
		      notes[pln].alive = 1;
		      notes[pln].time = 50;
		      notes[pln].x = (event.xbutton.x / 32) * 32 + 15;
		      notes[pln].y = (event.xbutton.y / 32) * 32 + 15 +
			fh[pln] / 2;
		      
		      strcpy(notes[pln].text, object_descs[c]);
		    }
		}
	    }
	  
	  
	  if (strcasecmp(key, "Q") == 0)
	    {
	      /* Q: Quit: */
	      
	      done = True;
	    }
	  
	  
	  /* These controls only work if the player is actually alive still! */
	  
	  if (same[pln] == 0)
	    {
	      if (player[pln].alive && player[pln].mushroomtime == 0 &&
		  title_mode == 0)
		{
		  if (strcasecmp(key, MAIN_UP) == 0 ||
		      strcasecmp(key, "KP_8") == 0)
		    {
		      /* Up: Move up */
		      
		      player[pln].shape = OBJ_MAN_UP;
		      
		      if (moveok(pln, 0, -1))
			{
			  player[pln].y--;
			  
			  if (player[pln].y < 0)
			    player[pln].y = MAP_HEIGHT - 1;
			}
		    }
		  else if (strcasecmp(key, MAIN_DOWN) == 0 ||
			     strcasecmp(key, "F31") == 0 ||
			     strcasecmp(key, "KP_5") == 0 ||
			     strcasecmp(key, "KP_2") == 0)
		    {
		      /* Down: Move down */
		      
		      player[pln].shape = OBJ_MAN_DOWN;
		      
		      if (moveok(pln, 0, 1))
			{
			  player[pln].y++;
			  
		      if (player[pln].y > MAP_HEIGHT - 1)
			player[pln].y = 0;
			}
		    }
		  else if (strcasecmp(key, MAIN_LEFT) == 0 ||
			   strcasecmp(key, "KP_4") == 0)
		    {
		      /* Left: Move left */
		      
		      player[pln].shape = OBJ_MAN_LEFT;
		      
		      if (moveok(pln, -1, 0))
			{
			  player[pln].x--;
		      
			  if (player[pln].x < 0)
			    player[pln].x = MAP_WIDTH -1;
			}
		    }
		  else if (strcasecmp(key, MAIN_RIGHT) == 0 ||
			   strcasecmp(key, "KP_6") == 0)
		    {
		      /* Right: Move right */
		      
		      player[pln].shape = OBJ_MAN_RIGHT;
		      
		      if (moveok(pln, 1, 0))
			{
			  player[pln].x++;
			  
			  if (player[pln].x > MAP_WIDTH - 1)
			    player[pln].x = 0;
			}
		    }
		  else if ((strcasecmp(key, MAIN_FIRE) == 0 ||
			    strcasecmp(key, "Space") == 0) &&
			   winner == -1)
		    {
		      /* Space: Drop bomb */
		      
		      dropbomb(pln);
		    }
		  else if (strcasecmp(key, MAIN_RADIOFIRE) == 0 ||
			   strcasecmp(key, "Enter") == 0)
		    {
		      /* Return: Explode radio bomb */
		      
		      player[pln].radio_detonate = 1;
		    }
		}
	    }
	  
	  if (pln < MAX_PLAYERS - 1 && same[pln + 1] == 1)
	    {
	      if (player[pln + 1].alive && player[pln + 1].mushroomtime == 0 &&
		  title_mode == 0)
		{
		  if (strcasecmp(key, SAME_UP) == 0)
		    {
		      /* S: Move up */
		      
		      player[pln + 1].shape = OBJ_MAN_UP;
		      
		      if (moveok(pln + 1, 0, -1))
			{
			  player[pln + 1].y--;
			  
			  if (player[pln + 1].y < 0)
			    player[pln + 1].y = MAP_HEIGHT - 1;
			}
		    }
		  else if (strcasecmp(key, SAME_DOWN) == 0)
		    {
		      /* X: Move down */
		      
		      player[pln + 1].shape = OBJ_MAN_DOWN;
		      
		      if (moveok(pln + 1, 0, 1))
			{
			  player[pln + 1].y++;
			  
		      if (player[pln + 1].y > MAP_HEIGHT - 1)
			player[pln + 1].y = 0;
			}
		    }
		  else if (strcasecmp(key, SAME_LEFT) == 0)
		    {
		      /* Z: Move left */
		      
		      player[pln + 1].shape = OBJ_MAN_LEFT;
		      
		      if (moveok(pln + 1, -1, 0))
			{
			  player[pln + 1].x--;
		      
			  if (player[pln + 1].x < 0)
			    player[pln + 1].x = MAP_WIDTH -1;
			}
		    }
		  else if (strcasecmp(key, SAME_RIGHT) == 0)
		    {
		      /* C: Move right */
		      
		      player[pln + 1].shape = OBJ_MAN_RIGHT;
		      
		      if (moveok(pln + 1, 1, 0))
			{
			  player[pln + 1].x++;
			  
			  if (player[pln + 1].x > MAP_WIDTH - 1)
			    player[pln + 1].x = 0;
			}
		    }
		  else if ((strcasecmp(key, SAME_FIRE) == 0) &&
			   winner == -1)
		    {
		      /* Tab: Drop bomb */
		      
		      dropbomb(pln + 1);
		    }
		  else if (strcasecmp(key, SAME_RADIOFIRE) == 0)
		    {
		      /* A: Explode radio bomb */
		      
		      player[pln + 1].radio_detonate = 1;
		    }
		}
	    }
	}
      
      
      /* Copy main ("underneath") map into seen map: */
      
      for (y = 0; y < MAP_HEIGHT; y++)
	for (x = 0; x < MAP_WIDTH; x++)
	  seenmap[y][x] = map[y][x];
      
      
      /* Draw bombs onto main seen map: */
      
      for (i = 0; i < MAX_BOMBS; i++)
	if (bomb[i].alive == 1)
	  seenmap[bomb[i].y][bomb[i].x] = bomb[i].shape;
      
      
      /* Draw death onto main seen map: */
      
      if (death.alive)
	seenmap[death.y][death.x] = death.shape;
      
      
      /* Handle bombs: */
      
      for (i = 0; i < MAX_BOMBS; i++)
	{
	  if (bomb[i].alive == 1)
	    {
	      if (bomb[i].moving == 0)
		{
		  if (bomb[i].shape != OBJ_RADIO_BOMB)
		    bomb[i].time--;
		  else if (bomb[i].shape == OBJ_RADIO_BOMB)
		    {
		      if (player[bomb[i].owner].radio_detonate == 1)
			bomb[i].time = 0;
		    }
		}
	      else
		{
		  /* Move bomb until it finds land: */
		  
		  if (ttoggle)
		    {
		      bomb[i].x = bomb[i].x + bomb[i].xm;	
		      if (bomb[i].x < 0)
			bomb[i].x = MAP_WIDTH - 1;
		      if (bomb[i].x > MAP_WIDTH - 1)
			bomb[i].x = 0;
		      
		      bomb[i].y = bomb[i].y + bomb[i].ym;	
		      if (bomb[i].y < 0)
			bomb[i].y = MAP_HEIGHT - 1;
		      if (bomb[i].y > MAP_HEIGHT - 1)
			bomb[i].y = 0;
		    }
		  
		  if (seenmap[bomb[i].y][bomb[i].x] == OBJ_DIRT)
		    bomb[i].moving = 0;
		}
	      
	      if (bomb[i].time <= 0)
		{
		  /* Explode the bomb: */
		  
		  kill_bomb(i);
		  makeexplosion(bomb[i].x, bomb[i].y, bomb[i].size);
		}
	    }
	}
      
      
      /* Handle explosions: */
      
      for (i = 0; i < MAX_EXPLOSIONS; i++)
	{
	  if (explosion[i].alive == 1)
	    {
	      explosion[i].time--;
	      
	      if (explosion[i].time <= 0)
		explosion[i].alive = 0;
	    }
	}
      
      
      /* Draw explosions onto main seen map: */
      
      killed_one = 0;
      
      for (i = 0; i < MAX_EXPLOSIONS; i++)
	{
	  if (explosion[i].alive == 1)
	    {
	      seenmap[explosion[i].y][explosion[i].x] = (OBJ_EXPLOSION2 -
							 (explosion[i].time /
							  8));
	      
	      for (j = 0; j < MAX_PLAYERS; j++)
		{
		  if (player[j].alive &&
		      player[j].x == explosion[i].x &&
		      player[j].y == explosion[i].y)
		    {
		      if (player[j].shieldtime > 0)
			player[j].shieldtime = 5;
		      else
			{
			  player[j].alive = 0;
			  initplayergoodies(j);
			  killed_one = 1;
			  
			  z = randnum(3);
			  
			  if (z == 0)
			    playsound(SND_OUCH1);
			  else if (z == 1)
			    playsound(SND_OUCH2);
			  else if (z == 3)
			    playsound(SND_OUCH3);
			}
		    }
		}
	    }
	}
      
      
      /* Make players pick up things: */
      
      for (pln = 0; pln < MAX_PLAYERS; pln++)
	{
	  if (player[pln].alive)
	    {
	      erase_it = 0;
	      c = map[player[pln].y][player[pln].x];
	      
	      if (c == OBJ_MOREFIRE)
		{
		  /* Now we have more fire: */
		  
		  player[pln].fire_size++;
		  if (player[pln].fire_size > MAX_FIRE_SIZE)
		    player[pln].fire_size = MAX_FIRE_SIZE;
		  
		  erase_it = 1;
		}
	      else if (c == OBJ_TOTALFIRE)
		{
		  /* Now we have the max. fire!: */
		  
		  player[pln].fire_size = MAX_FIRE_SIZE;
		  erase_it = 1;
		}
	      else if (c == OBJ_PUSHER)
		{
		  /* Now we can push blocks: */
		  
		  player[pln].pusher = 1;
		  erase_it = 1;
		}
	      else if (c == OBJ_KICKER)
		{
		  /* Now we can kick bombs:*/
		  
		  player[pln].kicker = 1;
		  erase_it = 1;
		}
	      else if (c == OBJ_MOREBOMB)
		{
		  /* Now we can drop more bombs: */
		     
		  player[pln].bombs_max++;
		  if (player[pln].bombs_max > MAX_BOMBS)
		    player[pln].bombs_max = MAX_BOMBS;
		  
		  player[pln].radiobomb = 0;
		  erase_it = 1;
		}
	      else if (c == OBJ_RADIOUPGRADE)
		{
		  /* Pick up a radio bomb (now we can only drop one bomb
		     at a time: */
		  
		  player[pln].radiobomb = 1;
		  player[pln].bombs_max = 1;
		  erase_it = 1;
		}
	      else if (c == OBJ_NUKE)
		{
		  /* Nuker causes all bombs to go off NOW, including
		     radiobombs! */
		  
		  for (i = 0; i < MAX_BOMBS; i++)
		    bomb[i].time = 1;
		  
		  for (i = 0; i < MAX_PLAYERS; i++)
		    player[i].radio_detonate = 1;
		  
		  erase_it = 1;
		}
	      else if (c == OBJ_INVERT)
		{
		  /* Invert causes blocks to turn to bricks and vice-versa */
		  
		  for (y = 0; y < MAP_HEIGHT; y++)
		    {
		      for (x = 0; x < MAP_WIDTH; x++)
			{
			  if (map[y][x] == OBJ_BOX)
			    map[y][x] = OBJ_BLOCK;
			  else if (map[y][x] == OBJ_BLOCK)
			    map[y][x] = OBJ_BOX;
			  else if (map[y][x] == OBJ_TNT)
			    map[y][x] = OBJ_BLOCK2;
			  else if (map[y][x] == OBJ_BLOCK2)
			    map[y][x] = OBJ_TNT;
			}
		    }
		  
		  playsound(SND_INVERT);
		  
		  erase_it = 1;
		}
	      else if (c == OBJ_MUSHROOM)
		{
		  /* Mushroom makes you turn into a computer for a while! */
		  
		  player[pln].mushroomtime = MUSHROOM_START_TIME;
		  player[pln].cmp_dir = 0;

		  erase_it = 1;
		}
	      else if (c == OBJ_WARP)
		{
		  /* Pick a random place to warp: */
		  
		  do
		    {
		      player[pln].x = randnum(MAP_WIDTH);
		      player[pln].y = randnum(MAP_HEIGHT);
		    }
		  while (map[player[pln].y][player[pln].x] != OBJ_DIRT);

		  playsound(SND_WARP);
		}
	      else if (c == OBJ_BONES)
		{
		  /* Pick up bones... */
		  
		  if (randnum(10) < 5)
		    {
		      /* and sometimes get cool stuff: */
		      
		      player[pln].kicker = 1;
		      player[pln].pusher = 1;
		      player[pln].fire_size = MAX_FIRE_SIZE;
		    }
		  else
		    {
		      /* Other times get a mushroom: */
		      
		      player[pln].mushroomtime = MUSHROOM_START_TIME;
		      player[pln].cmp_dir = 0;
		    }
		  
		  erase_it = 1;
		}
	      else if (c == OBJ_SHIELD)
		{
		  /* Pick up shields... */
		  
		  player[pln].shieldtime = player[pln].shieldtime + 300;
		  erase_it = 1;
		}
	      else if (c == OBJ_INVISIBLE)
		{
		  /* Pick up invisible... */
		  
		  player[pln].invisibletime = player[pln].invisibletime + 200;
		  erase_it = 1;
		}
	      
	      
	      /* If it's a pick-up item, remove it from the map and
		 play the "picked-up-an-item" sound: */
	      
	      if (erase_it == 1)
		{
		  playsound(SND_GOT);
		  
		  map[player[pln].y][player[pln].x] = OBJ_DIRT;
		  
		  redrawstatus(pln);
		}
	    }
	}
      
      
      /* Draw players onto main seen map: */
      
      for (pln = 0; pln < MAX_PLAYERS; pln++)
	{
	  if (player[pln].alive && player[pln].invisibletime <= 0)
	    seenmap[player[pln].y][player[pln].x] = player[pln].shape;
	}
      
      
      /* Computer controlled players: */
      
      if (ttoggle && title_mode == 0)
	{
	  for (pln = 0; pln < num_servers; pln++)
	    {
	      if (player[pln].alive && player[pln].mushroomtime > 0)
		{
		  control_computer(pln);
		}
	    }
	  
	  for (pln = num_servers; pln < MAX_PLAYERS; pln++)
	    {
	      if (player[pln].alive)
		control_computer(pln);
	    }
	}
      

      /* ALL players: */
      
      for (pln = 0; pln < MAX_PLAYERS; pln++)
	{
	  if (player[pln].mushroomtime > 0)
	    player[pln].mushroomtime--;
      
	  if (player[pln].shieldtime > 0)
	    {
	      player[pln].shieldtime--;
	      
	      if (player[pln].shieldtime <= 0)
		{
		  for (i = 0; i < num_servers; i++)
		    player[i].seenmap[player[pln].y][player[pln].x] = -1;
		}
	    }

	  if (player[pln].invisibletime > 0)
	    player[pln].invisibletime--;
	}
      
      
      /* Redraw the changed part of player's windows: */
      
      for (y = 0; y < MAP_HEIGHT; y++)
	{
	  for (x = 0; x < MAP_WIDTH; x++)
	    {
	      for (pln = 0; pln < num_servers; pln++)
		{
		  if (seenmap[y][x] != player[pln].seenmap[y][x])
		    {
		      if (use_color == 1)
			{
			  /* drawblock(pln, x, y, map[y][x]); */
			  drawblock(pln, x, y, OBJ_DIRT);
			}
		      
		      drawblock(pln, x, y, seenmap[y][x]);
		      player[pln].seenmap[y][x] = seenmap[y][x];
		    }
		}
	    }
	}
      
      
      if (title_mode == 1 && (mod_counter % 20) == 0)
	{
	  for (pln = 0; pln < num_servers; pln++)
	    {
	      if (use_color == 1)
		{
		  XSetClipOrigin(display[pln],
				 colorgcs[real(pln)][OBJ_TITLE],
				 (WIDTH - TITLE_WIDTH) / 2,
				 (HEIGHT - TITLE_HEIGHT) / 2);
		  
		  XCopyArea(display[pln], object_pixmaps[real(pln)][OBJ_TITLE],
			    window[pln], colorgcs[real(pln)][OBJ_TITLE], 0, 0,
			    TITLE_WIDTH, TITLE_HEIGHT, 
			    (WIDTH - TITLE_WIDTH) / 2,
			    (HEIGHT - TITLE_HEIGHT) / 2);
		}
	      else
		{
		  XCopyPlane(display[pln],
			     object_pixmaps[real(pln)][OBJ_TITLE],
			     window[pln], colorgcs[real(pln)][OBJ_TITLE], 0, 0,
			     TITLE_WIDTH, TITLE_HEIGHT,
			     (WIDTH - TITLE_WIDTH) / 2,
			     (HEIGHT - TITLE_HEIGHT) / 2, 1);
		}
	    }
	}
      
      
      /* Draw notes: */
      
      for (pln = 0; pln < num_servers; pln++)
	{
	  if (notes[pln].alive == 1)
	    {
	      drawtext(display[pln], window[pln],
		       whitegc[pln], notes[pln].x, notes[pln].y,
		       notes[pln].text);
	      
	      notes[pln].time--;
	      
	      if (notes[pln].time <= 0)
		{
		  notes[pln].alive = 0;
		  
		  player[pln].seenmap[notes[pln].y / 32]
		    [notes[pln].x / 32] = -1;
		  player[pln].seenmap[notes[pln].y / 32]
		    [notes[pln].x / 32 + 1] = -1;
		}
	    }
	}
      
      
      /* Handle end-of-level stuff: */
      
      if (winner != -1)
	{
	  freetime--;
	  
	  if (freetime <= 0)
	    {
	      level = (level + 1) % 100;
	      
	      initlevel();
	      
	      for (i = 0; i < num_servers; i++)
		redrawstatus(i);
	    }
	}
      
      
      /* Keep track of level's time: */
      
      if (leveltime > 0 && title_mode == 0)
	{
	  leveltime--;
	  
	  if (leveltime <= 0)
	    {
	      newblocks_x = 0;
	      newblocks_y = 0;
	      newblocks_dir = 1;
	      
	      newblocks_top = 0;
	      newblocks_bottom = MAP_HEIGHT - 1;
	      newblocks_left = 0;
	      newblocks_right = MAP_WIDTH - 1;
	      
	      strcpy(message, "WATCH OUT!");
	      
	      for (i = 0; i < num_servers; i++)
		redrawstatus(i);
	    }
	}
      
      
      /* Add new blocks if level is "over!" */
      
      if (leveltime <= 0 && ttoggle == 0 && winner == -1)
	{
	  addexplosion_atomic(newblocks_x, newblocks_y);
	  
	  map[newblocks_y][newblocks_x] = OBJ_BLOCK2;
	  
	  
	  if (newblocks_x + xms[newblocks_dir] >= newblocks_left &&
	      newblocks_x + xms[newblocks_dir] <= newblocks_right &&
	      newblocks_y + yms[newblocks_dir] >= newblocks_top &&
	      newblocks_y + yms[newblocks_dir] <= newblocks_bottom)
	    {
	      newblocks_x = newblocks_x + xms[newblocks_dir];
	      newblocks_y = newblocks_y + yms[newblocks_dir];
	    }
	  else
	    {
	      /* Cause inward motion: */
	      
	      if (newblocks_dir == 1)
		newblocks_top++;
	      else if (newblocks_dir == 2)
		newblocks_right--;
	      else if (newblocks_dir == 3)
		newblocks_bottom--;
	      else if (newblocks_dir == 0)
		newblocks_left++;
	      
	      
	      /* Cause spiral motion: */
	      
	      newblocks_dir = (newblocks_dir + 1) % 4;
	    }
	}
      
      
      /* Turn death on randomly: */
      
      if (((level > 10 && randnum(1000) < 1) || randnum(20000) < 1) &&
	  death.alive == 0 && winner == -1 && title_mode == 0)
	{
	  playsound(SND_LAUGH);
	  
	  death.alive = 1;
	  death.x = randnum(MAP_WIDTH);
	  death.y = randnum(MAP_HEIGHT);
	  death.shape = OBJ_DEATH_EATING;
	  
	  /* Pick a sucker to chase (but not a dead one): */
	  
	  make_death_seek();

	  death.time = randnum(200) + 100;
	}
      
      
      /* Handle death: */
      
      if (death.alive && cmp_movetoggle == 0 && ttoggle == 0 && toggle == 0)
	{
	  /* No more, if someone won.. */
	  
	  if (winner != -1)
	    death.alive = 0;
	  
	  
	  /* Chase the person we're seeking (if we're not eating): */
	  
	  if (death.shape == OBJ_DEATH)
	    {
	      if (death.x < player[death.seek].x &&
		  empty(death.x + 1, death.y))
		death.x = death.x + 1;
	      else if (death.x > player[death.seek].x &&
		       empty(death.x - 1, death.y))
		death.x = death.x - 1;
	      else if (death.y < player[death.seek].y &&
		       empty(death.x, death.y + 1))
		death.y = death.y + 1;
	      else if (death.y > player[death.seek].y &&
		       empty(death.x, death.y - 1))
		death.y = death.y - 1;
	      else
		{
		  i = randnum(4);
		  
		  if (empty(death.x + xms[i], death.y + yms[i]))
		    {
		      death.x = death.x + xms[i];
		      death.y = death.y + yms[i];
		    }
		}
	      
	      
	      /* Eat bombs: */
	      
	      for (i = 0; i < MAX_BOMBS; i++)
		{
		  if (bomb[i].y == death.y && bomb[i].x == death.x &&
		      bomb[i].alive)
		    {
		      death.shape = OBJ_DEATH_EATING;
		      
		      kill_bomb(i);
		      
		      addexplosion_atomic(death.x, death.y);
		    }
		}
	      
	      
	      /* Eat players: */
	      
	      for (i = 0; i < MAX_PLAYERS; i++)
		{
		  if (player[i].y == death.y && player[i].x == death.x &&
		      player[i].alive)
		    {
		      death.shape = OBJ_DEATH_EATING;
		      
		      player[i].alive = 0;
		      initplayergoodies(i);

		      addexplosion_atomic(death.x, death.y);
		      
		      killed_one = 1;
		      
		      map[death.y][death.x] = OBJ_BONES;
		    }
		}
	      
	      
	      /* Eat upgrades: */
	      
	      if (map[death.y][death.x] >= FIRST_UPGRADE &&
		  map[death.y][death.x] < FIRST_UPGRADE + NUM_UPGRADES)
		{
		  death.shape = OBJ_DEATH_EATING;
		  
		  map[death.y][death.x] = OBJ_DIRT;
		}
	      
	      
	      /* Make death warp: */
	      
	      if (map[death.y][death.x] == OBJ_WARP)
		{
		  /* Pick a random place to warp: */
		  
		  do
		    {
		      death.x = randnum(MAP_WIDTH);
		      death.y = randnum(MAP_HEIGHT);
		    }
		  while (map[death.y][death.x] != OBJ_DIRT);
		  
		  playsound(SND_WARP);
		}
	      
	      
	      /* Randomly "yell" :) */
	      
	      if (randnum(100) < 1)
		death.shape = OBJ_DEATH_EATING;
	    }
	  else
	    death.shape = OBJ_DEATH;
	  
	  
	  /* If that guy died, choose another guy: */
	  
	  if (player[death.seek].alive == 0)
	    make_death_seek();
	  
	  
	  /* Count death down: */
	  
	  if (death.time <= 0)
	    death.alive = 0;
	}
      

      /* Do stuff is someone died: */
      
      if (killed_one && winner == -1)
	{
	  /* See how many people are alive: */
	  
	  num_alive = 0;
	  found = -1;
	  
	  for (i = 0; i < MAX_PLAYERS; i++)
	    {
	      if (player[i].alive)
		{
		  num_alive++;
		  found = i;
		}
	    }
	  
	  
	  /* Results of one person (or no people) alive: */
	  
	  if (num_alive == 0 || num_alive == 1)
	    {
	      if (num_alive == 1)
		{
		  winner = found;
		  player[found].score++;
		  sprintf(message, "Player %d won!", found + 1);
		}
	      else if (num_alive == 0)
		{
		  winner = -2;
		  strcpy(message, "A draw!");
		}
	      
	      
	      freetime = FREETIME;
	      
	      
	      /* Turn off all bombs: */
	      
	      for (i = 0; i < MAX_BOMBS; i++)
		bomb[i].alive = 0;
	      
	      
	      /* Explode all boxes and warps (may get some goodies!): */
	      
	      for (y = 0; y < MAP_HEIGHT; y++)
		{
		  for (x = 0; x < MAP_WIDTH; x++)
		    {
		      if (map[y][x] == OBJ_BOX || map[y][x] == OBJ_TNT ||
			  map[y][x] == OBJ_WARP)
			{
			  addexplosion_atomic(x, y);
			  
			  if (randnum(CHANCE_OF_UPGRADE) < 1)
			    map[y][x] = randnum(NUM_UPGRADES) + FIRST_UPGRADE;
			  else
			    map[y][x] = 0;
			}
		    }
		}
	      
	      z = randnum(NUM_LEVELEND_SNDS) + SND_LEVELEND1;
	      playsound(z);
	    }

	  
	  
	  /* Update everyone's status: */
	  
	  for (i = 0; i < num_servers; i++)
	    redrawstatus(i);
	}
      
      
      /* for (i = 0; i < num_servers; i++)
	XSync(display[i], 0); */
      
      
      /* Keep framerate exact: */
      
      gettimeofday(&now, NULL);
      
      time_padding = FRAMERATE - ((now.tv_sec - then.tv_sec) * 1000000 +
				  (now.tv_usec - then.tv_usec));
      
      if (time_padding > 0)
	usleep(time_padding);
    }
  while (done == False);
}


/* Program set-up (check usage, load data, etc.): */

void setup(int argc, char * argv[])
{
  FILE * fi;
  char temp[512], color[512], file[1024];
  int i, len, z, zc, cntl, want_length, sound_pos_tmp, which_sound;
  char c;
  struct timeval now, then;


  /* Get -color switch: */

  use_color = 0;
  strcpy(graphicsset, "./pixmaps");
  
  if (argc >= 2)
    {
      if (strcmp(argv[argc - 2], "-color") == 0 ||
	  strcmp(argv[argc - 2], "-c") == 0)
	{
	  strcpy(graphicsset, argv[argc - 1]);
	  argc = argc - 1;
	  
	  fprintf(stderr, "Will attempt to use %s graphics set\n",
		  graphicsset);
	}
      
      if (strcmp(argv[argc - 1], "-color") == 0 ||
	  strcmp(argv[argc - 1], "-c") == 0)
	{
	  use_color = 1;
	  argc = argc - 1;
	}
    }
  
  
  /* Get -sound switch: */
  
  use_sound = 0;
  
  if (argc >= 2)
    {
      if (strcmp(argv[argc - 1], "-sound") == 0 ||
	  strcmp(argv[argc - 1], "-s") == 0)
	{
	  use_sound = 1;
	  

	  /* Open audio device: */
	  
	  devaudio_fd = open("/dev/audio", O_RDWR);
	  
	  if (devaudio_fd == -1)
	    {
	      fprintf(stderr, "Can't open /dev/audio!");
	      exit(0);
	    }
	  
	  
	  /* Fork off into a sound manager process: */
	  
	  if (pipe(pipefds) < 0)
	    {
	      perror("pipe");
	      exit(1);
	    }
	  
	  pid = fork();
	  
	  
	  /* Sound manager routine: */
	  
	  if (pid == 0)
	    {
	      /* Figure out our file descriptors: */
	      
	      close(0);
	      dup(pipefds[0]);
	      close(pipefds[0]);
	      close(pipefds[1]);
	      
	      cntl = fcntl(0, F_GETFL, 0);
	      cntl = cntl | O_NONBLOCK;
	      fcntl(0, F_SETFL, cntl);
	      
	      len = 0;
	      
	      sound_position = 0;
	      
	      for (i = 0; i < SOUND_BUF; i++)
		sound_data[i] = 0;
	      
	      
	      /* Load game sounds: */
	      
	      for (i = 0; i < NUM_SOUNDS; i++)
		{
		  sprintf(file, "sounds/%s.au", sound_names[i]);
		  
		  fi = fopen(file, "r");
		  if (fi != NULL)
		    {
		      sound_pos_tmp = 0;
		      
		      do
			{
			  zc = fgetc(fi);
			  
			  if (zc != EOF)
			    {
			      au_file_data[i][sound_pos_tmp] =
				(unsigned char) zc;
			      
			      sound_pos_tmp++;
			    }
			}
		      while (zc != EOF);
		      
		      sound_len[i] = sound_pos_tmp;
		      
		      fclose(fi);
		    }
		  else
		    {
		      perror(file);
		      exit(0);
		    }
		}
	      

	      /* Load number sounds: */
	      
	      for (i = 0; i < NUM_NSOUNDS; i++)
		{
		  sprintf(file, "sounds/numbers/%s.au", nsound_names[i]);
		  
		  fi = fopen(file, "r");
		  if (fi != NULL)
		    {
		      sound_pos_tmp = 0;
		      
		      do
			{
			  zc = fgetc(fi);
			  
			  if (zc != EOF)
			    {
			      nau_file_data[i][sound_pos_tmp] =
				(unsigned char) zc;
			      
			      sound_pos_tmp++;
			    }
			}
		      while (zc != EOF);
		      
		      nsound_len[i] = sound_pos_tmp;
		      
		      fclose(fi);
		    }
		  else
		    {
		      perror(file);
		      exit(0);
		    }
		}
	      
	      printf("(Sound manager: running!)\n");
	      
	      
	      /* Loop: */
	      
	      do
		{
		  gettimeofday(&then, NULL);
		  
		  
		  /* Read from parent: */
		  
		  z = read(0, &c, 1);
		  
		  if (z > 0)
		    {
		      if (c != '\n')
			{
			  file[len] = c;
			  len++;
			}
		      else
			{
			  /* Copy sound into queue: */
			  
			  file[len] = '\0';
			  len = 0;
			  
			  if (file[0] != 'N')
			    {
			      which_sound = atoi(file);
			      
			      for (i = 0; i < sound_len[which_sound]; i++)
				{
				  sound_data[(i + sound_position) % SOUND_BUF]
				    = au_file_data[which_sound][i];
				}
			    }
			  else
			    {
			      which_sound = atoi(file + 1);
			      
			      for (i = 0; i < nsound_len[which_sound]; i++)
				{
				  sound_data[(i + sound_position) % SOUND_BUF]
				    = nau_file_data[which_sound][i];
				}
			    }
			}
		    }
		  
		  
		  /* Play sound: */
		  
		  want_length = 10;
		  
		  if (sound_data[sound_position] != 0)
		    {
		      write(devaudio_fd, &sound_data[sound_position],
			    want_length);
		      
		      for (sound_pos_tmp = 0; sound_pos_tmp < want_length;
			   sound_pos_tmp++)
			sound_data[sound_position + sound_pos_tmp] = 0;
		    }
		  
		  sound_position = (sound_position + want_length) %
		    SOUND_BUF;

		  do
		    {
		      gettimeofday(&now, NULL);
		    }
		  while (now.tv_sec == then.tv_sec &&
			 now.tv_usec < then.tv_usec + 500);
		}
	      while (z != 0);
	      
	      printf("(Sound manager: quitting!)\n");
	      
	      exit(0);
	    }
	  else if (pid == -1)
	    {
	      perror("fork");
	      exit(1);
	    }
	  else
	    {
	      close(pipefds[0]);
	      
	      sound_fs = fdopen(pipefds[1], "w");
	      
	      fprintf(stderr, "Connected to sound manager process\n");
	    }
	  
	  argc = argc - 1;
	}
    }
  
  
  /* Get "-L" level switch: */
  
  level = 1;
  
  if (argc >= 2)
    {
      if (strstr(argv[argc - 1], "-l") == argv[argc - 1] ||
	  strstr(argv[argc - 1], "-L") == argv[argc - 1])
	{
	  level = atoi(argv[argc - 1] + 2);
	  
	  if (level < 1 || level > 99)
	    level = 1;
	  
	  argc--;
	}
    } 
  
  
  if (argc == 2)
    {
      if (strcmp(argv[1], "-version") == 0 ||
	  strcmp(argv[1], "-v") == 0)
	{
	  /* Check for "-version": */
	  
	  printf("\nxbomber version 0.8\n\n");
	  
	  printf("by Bill Kendrick\n");
	  printf("kendrick@zippy.sonoma.edu\n");
	  printf("http://zippy.sonoma.edu/kendrick/nbs/unix/x/xbomber/\n\n");
	  exit(0);
	}
      else if (strcmp(argv[1], "-help") == 0 ||
	       strcmp(argv[1], "-h") == 0)
	{
	  /* Check for "-help": */
	  
	  fprintf(stderr, "\n");
	  fprintf(stderr, "Usage: %s ", argv[0]);
	  fprintf(stderr, "server1 [server2 [server3 [server4]]] [-Llevel]\n");
	  fprintf(stderr, "       [-sound] [-color [graphics-set]]");
	  fprintf(stderr, " | -help | -version\n");
	  fprintf(stderr, "\n");
	  fprintf(stderr, "server2 through server4 can be \"SAME\"");
	  fprintf(stderr, " (if previous server isn't)\n");
	  fprintf(stderr, "  for next player to play on the same keyboard.\n");
	  fprintf(stderr, "\n");
	  fprintf(stderr, "Controls:\n");
	  fprintf(stderr, "  [Arrow] - Move                     [S/Z/X/C]\n");
	  fprintf(stderr, "  [Space] - Drop bomb                  [Tab]\n");
	  fprintf(stderr, "  [Enter] - Detonate radio bomb         [A]\n");
	  fprintf(stderr, "    [Q]   - Quit\n");
	  fprintf(stderr, "\n");
	  
	  exit(0);
	}
    }
  
  if (argc == 1)
    {
      if (getenv("DISPLAY") != NULL)
	{
	  strcpy(server[0], getenv("DISPLAY"));
	  num_servers = 1;
	}
      else
	{
	  fprintf(stderr, "Can't determine your display!\n");
	  fprintf(stderr, "Please specify it on the command line.\n");
	  exit(1);
	}
    }
  else if (argc <= MAX_PLAYERS + 1)
    {
      num_servers = argc - 1;
      
      for (i = 0; i < argc - 1; i++)
	strcpy(server[i], argv[i + 1]);
    }
  else
    {
      fprintf(stderr, "Too many servers specified.\n");
      fprintf(stderr, "Max. players: %d\n", MAX_PLAYERS);
      exit(1);
    }
  
  
  /* Init. dir -> xm/ym table: */
  
  xms[0] = 0;
  yms[0] = -1;

  xms[1] = 1;
  yms[1] = 0;

  xms[2] = 0;
  yms[2] = 1;

  xms[3] = -1;
  yms[3] = 0;
  
  xms[4] = 0;
  yms[4] = 0;
}



/* Connect, make window, and display "please wait" */

void Xsetup_windows(int pln)
{
  char title[1024];
  int done;
  XEvent event;
  
  
  /* Connect to display: */
  
  fprintf(stderr, "Connecting to: %s\n", server[pln]);
  
  display[pln] = ConnectToServer(server[pln], &screen[pln], &root[pln]);
  if (display[pln] == NULL)
    {
      perror(server[pln]);
      exit(1);
    }
  
  
  /* Load font: */
  
  font[pln] = LoadFont(display[pln], "variable", "fixed");
  fh[pln] = FontHeight(font[pln]);
  
  
  /* Get our primitve colors: */
  
  black[pln] = BlackPixel(display[pln], screen[pln]);
  white[pln] = WhitePixel(display[pln], screen[pln]);
  
  
  /* Open window: */
  
  if (pln >= MAX_PLAYERS + 1 || same[pln + 1] == 0)
    {
      window[pln] = OpenWindow(display[pln], root[pln], 0, 0, WIDTH, HEIGHT,
			       CopyFromParent, black[pln],
			       (KeyPressMask | ExposureMask | ButtonPressMask),
			       (Visual *)CopyFromParent);

      sprintf(title, "X-Bomber 0.8 by Bill Kendrick");
      SetStandardHints(display[pln], window[pln], "XBomber", title, 10, 10,
		       WIDTH, HEIGHT);
    }
  else
    {
      window[pln] = OpenWindow(display[pln], root[pln], 0, 0,
			       WIDTH, HEIGHT + 32,
			       CopyFromParent, black[pln],
			       (KeyPressMask | ExposureMask | ButtonPressMask),
			       (Visual *)CopyFromParent);

      sprintf(title, "X-Bomber 0.8 by Bill Kendrick");
      SetStandardHints(display[pln], window[pln], "XBomber", title, 10, 10,
		       WIDTH, HEIGHT + 32);
    }
  

  /* Create Primitive GC's: */
  
  whitegc[pln] = CreateGC(display[pln], window[pln],
			  white[pln], black[pln]);

  blackgc[pln] = CreateGC(display[pln], window[pln],
			  black[pln], black[pln]);
  
  
  
  /* Put us up!: */
  
  XMapWindow(display[pln], window[pln]);
  XMapRaised(display[pln], window[pln]);
  XSync(display[pln], 0);


  /* Wait for map event: */
  
  done = 0;
  
  while (done == 0)
    {
      if (XPending(display[pln]))
	{
	  XNextEvent(display[pln], &event);
	  
	  if (event.type == Expose || event.type == MapNotify)
	    {
	      done = 1;
	    }
	}
    }

  drawcenteredtext(display[pln], window[pln],
		   whitegc[pln], 0, WIDTH, HEIGHT / 2,
		   "X-Bomber Setting Up: Please Wait", font[pln]);
  
  XSync(display[pln], 0);
}


/* Setup the application: */

void Xsetup(int pln)
{
  int i, tempint1, tempint2, tempint3, tempint4, ret;
  char file[128];
  int status, temp_depth;
  FILE * fi;
  XGCValues gcvalues;
  
  
  /* Set up visual: */
  
  has_color[pln] = 0;
  
  if (SetUpVisual(display[pln], screen[pln], &temp_visual[pln], &temp_depth))
    {
      if (!SetUpColormap(display[pln], screen[pln], window[pln],
			 temp_visual[pln], &colormap[pln]))
        {
          fprintf(stderr, "Could not create a colormap!\n");
        }
      else
        has_color[pln] = 1;
    }
  else
    {
      fprintf(stderr, "Could not find a PseudoColor visual!\n");
    }
  
  
  /* Make cursor: */
  
  ret = XReadBitmapFile(display[pln], window[pln], "bitmaps/cursor.xbm",
			&tempint1, &tempint2, &cursor_pixmap[pln],
			&tempint3, &tempint4);
  
  if (ret != BitmapSuccess)
    {
      perror("bitmaps/cursor.xbm");
      exit(1);
    }
  
  ret = XReadBitmapFile(display[pln], window[pln], "bitmaps/cursor-mask.xbm",
			&tempint1, &tempint2, &cursor_mask[pln],
			&tempint3, &tempint4);
  
  if (ret != BitmapSuccess)
    {
      perror("bitmaps/cursor-mask.xbm");
      exit(1);
    }
  
  XLookupColor(display[pln], colormap[pln], "black", &blackxcolor[pln],
	       &blackxcolor[pln]);
  XLookupColor(display[pln], colormap[pln], "white", &whitexcolor[pln],
	       &whitexcolor[pln]);
  
  cursor[pln] = XCreatePixmapCursor(display[pln], cursor_pixmap[pln],
				    cursor_mask[pln], 
				    &blackxcolor[pln], &whitexcolor[pln],
				    0, 0);
  
  XDefineCursor(display[pln], window[pln], cursor[pln]);
  
  
  /* Load object pixmaps: */
  
  for (i = 0; i < NUM_OBJECTS; i++)
    {
      if (use_color == 0)
	{
	  if (i < OBJ_YOU_DOWN)
	    {
	      colorgcs[pln][i] = CreateGC(display[pln], window[pln],
					  MyAllocNamedColor(display[pln],
							    colormap[pln],
							    object_colors[i],
							    white[pln],
							    has_color[pln]),
					  MyAllocNamedColor(display[pln],
							    colormap[pln],
							    object_colors[0],
							    black[pln],
							    has_color[pln]));
	      
	      sprintf(file, "bitmaps/%s.xbm", object_names[i]);
	      
	      ret = XReadBitmapFile(display[pln], window[pln], file, 
				    &tempint1, &tempint2,
				    &object_pixmaps[pln][i],
				    &tempint3, &tempint4);
	      
	      if (ret != BitmapSuccess)
		{
		  perror(file);
		  exit(1);
		}
	    }
	}
      else
	{
	  loadobject(pln, i);
	}
      
      if (i == OBJ_TITLE)
	{
	  if (use_color == 1)
	    {
	      XSetClipOrigin(display[pln],
			     colorgcs[pln][OBJ_TITLE],
			     (WIDTH - TITLE_WIDTH) / 2,
			     (HEIGHT - TITLE_HEIGHT) / 2);
	      
	      XCopyArea(display[pln], object_pixmaps[pln][OBJ_TITLE],
			window[pln], colorgcs[pln][OBJ_TITLE], 0, 0,
			TITLE_WIDTH, TITLE_HEIGHT, 
			(WIDTH - TITLE_WIDTH) / 2,
			(HEIGHT - TITLE_HEIGHT) / 2);
	    }
	  else
	    {
	      XCopyPlane(display[pln], object_pixmaps[pln][OBJ_TITLE],
			 window[pln], colorgcs[pln][OBJ_TITLE], 0, 0,
			 TITLE_WIDTH, TITLE_HEIGHT,
			 (WIDTH - TITLE_WIDTH) / 2,
			 (HEIGHT - TITLE_HEIGHT) / 2, 1);
	    }
	}
    }
  
  printf("\n");
  
  yougc[pln] = CreateGC(display[pln], window[pln],
			MyAllocNamedColor(display[pln], colormap[pln],
					  YOU_COLOR, white[pln],
					  has_color[pln]),
			MyAllocNamedColor(display[pln], colormap[pln],
					  object_colors[0], black[pln],
					  has_color[pln]));
  
  /* Put us up!: */
  
  XMapWindow(display[pln], window[pln]);
  XMapRaised(display[pln], window[pln]);
  XSync(display[pln], 0);
}


/* Allocate a color (or white): */

unsigned long MyAllocNamedColor(Display *display, Colormap colormap,
                                char* colorname, unsigned long default_color,
                                int has_color)
{
  if (has_color == 0)
    return(default_color);
  else
    return(AllocNamedColor(display, colormap, colorname, default_color));
}


/* Play a sound (if we can): */

void playsound(int whichfile)
{
  if (use_sound == 1)
    {
      fprintf(sound_fs, "%d\n", whichfile);
      fflush(sound_fs);
    }
}


/* Play a sound (if we can): */

void playnsound(int whichfile)
{
  if (use_sound == 1)
    {
      fprintf(sound_fs, "N%d\n", whichfile);
      fflush(sound_fs);
    }
}


/* (Re)draw a tile at some point on the player's window: */

void drawblock(int pln, int x, int y, int c)
{
  int i, draw_shield, realpln;
  
  realpln = real(pln);
  
  draw_shield = 0;
  
  for (i = 0; i < MAX_PLAYERS; i++)
    {
      if (c >= OBJ_MAN_DOWN && c <= OBJ_MAN_LEFT &&
	  player[i].x == x && player[i].y == y)
	{
	  if (player[i].shieldtime > 0)
	    draw_shield = 1;
	}
    }
  
  
  if (use_color == 0)
    {
      if ((player[pln].alive && player[pln].x == x && player[pln].y == y) ||
	  (pln > 0 && player[pln - 1].alive && player[pln - 1].x == x &&
	   player[pln - 1].y == y && same[pln] == 1))
	{
	  XCopyPlane(display[realpln], object_pixmaps[realpln][c],
		     window[realpln], yougc[realpln], 0, 0, 32, 32,
		     x * 32, y * 32, 1);
	  
	  if (same[pln] == 1 && player[pln].x == x && player[pln].y == y)
	    drawtext(display[realpln], window[realpln], whitegc[realpln],
		     x * 32 + 16, y * 32 + 20, "S");
	}
      else
	XCopyPlane(display[realpln], object_pixmaps[realpln][c],
		   window[realpln], colorgcs[realpln][c],
		   0, 0, 32, 32, x * 32, y * 32, 1);
    }
  else
    {
      if (y >= MAP_HEIGHT && c == OBJ_DIRT)
	{
	  XFillRectangle(display[realpln], window[realpln], blackgc[realpln],
			 x * 32, y * 32, 32, 32);
	}
      else
	{
	  if (c == OBJ_DIRT)
	    {
	      if (background_mode == BKG_CHECKER)
		c = OBJ_CHECKER1 + ((x + y) % 2);
	      else if (background_mode == BKG_CROSS)
		c = OBJ_CROSS;
	    }
	  else
	    {
	      if (((player[pln].alive &&
		    player[pln].x == x && player[pln].y == y) ||
		   (pln > 0 && player[pln - 1].alive &&
		    player[pln - 1].x == x && player[pln - 1].y == y &&
		    same[pln] == 1)) && 
		  c >= OBJ_MAN_DOWN && c <= OBJ_MAN_LEFT)
		c = (c - OBJ_MAN_DOWN) + OBJ_YOU_DOWN;
	    }
	  
	  XSetClipOrigin(display[realpln], colorgcs[realpln][c],
			 x * 32, y * 32);
	  
	  XCopyArea(display[realpln], object_pixmaps[realpln][c],
		    window[realpln], colorgcs[realpln][c], 0, 0, 32, 32,
		    x * 32, y * 32);
	  
	  if (c >= OBJ_YOU_DOWN && c <= OBJ_YOU_LEFT && same[pln] == 1
	      && player[pln].x == x && player[pln].y == y)
	    drawtext(display[realpln], window[realpln], whitegc[realpln],
		     x * 32 + 16, y * 32 + 20, "S");
	}
    }

  if (draw_shield)
    XDrawRectangle(display[realpln], window[realpln], whitegc[realpln],
		   x * 32 + 2, y * 32 + 2, 28, 28);
  
  /* XSync(display[pln], 0); */
  XFlush(display[realpln]);
}


/* Make an explosion (as much of it as we can, depending on where walls
   are: */

void makeexplosion(int x, int y, int size)
{
  int done, i, ii;
  
  playsound(SND_EXPLODE);
  
  done = 0;
  for (i = x; i >= x - size + 1 && done == 0; i--)
    {
      ii = i;
      if (ii < 0)
	ii = ii + MAP_WIDTH;
      
      done = addexplosion(ii, y);
    }
  
  done = 0;
  for (i = x + 1; i < x + size && done == 0; i++)
    {
      ii = i;
      if (ii > MAP_WIDTH - 1)
	ii = ii - MAP_WIDTH;
      
      done = addexplosion(ii, y);
    }

  done = 0;
  for (i = y; i >= y - size + 1 && done == 0; i--)
    {
      ii = i;
      if (ii < 0)
	ii = ii + MAP_HEIGHT;
      
      done = addexplosion(x, ii);
    }
  
  done = 0;
  for (i = y + 1; i < y + size && done == 0; i++)
    {
      ii = i;
      if (ii > MAP_HEIGHT - 1)
	ii = ii - MAP_HEIGHT;
      
      done = addexplosion(x, ii);
    }
}


/* Add one explosion to the screen (if it can).  Returns 1 when an
   obstacle is found: */

int addexplosion(int x, int y)
{
  int ret, i;
  
  
  ret = 0;
  
  if (map[y][x] == OBJ_BLOCK || map[y][x] == OBJ_BLOCK2)
    {
      /* Stop if we hit a block (they're indestructable): */
      
      ret = 1;
    }
  else
    {
      /* Add an explosion at this spot: */
      
      addexplosion_atomic(x, y);
      
      if (map[y][x] == OBJ_BOX)
	{
	  /* If it's a box, stop, and turn the box into an upgrade (maybe): */
	  
	  ret = 1;
	  
	  if (randnum(CHANCE_OF_UPGRADE) < 1)
	    map[y][x] = randnum(NUM_UPGRADES) + FIRST_UPGRADE;
	  else
	    map[y][x] = OBJ_DIRT;
	}
      else if (map[y][x] == OBJ_TNT)
	{
	  /* If it's TNT, stop, turn it into an upgrade (maybe) and
	     explode some more!: */
	  
	  ret = 1;
	  
	  map[y][x] = OBJ_DIRT;
	  
	  makeexplosion(x, y, 2);
	  
	  if (randnum(CHANCE_OF_UPGRADE) < 1)
	    map[y][x] = randnum(NUM_UPGRADES) + FIRST_UPGRADE;
	  else
	    map[y][x] = OBJ_DIRT;
	}
      else
	{
	  /* Other things (ie, upgrades, warps, etc.) turn into dirt: */
	  
	  map[y][x] = OBJ_DIRT;
	}
    }
  
  
  /* Blow up any bombs we touched: */
  
  for (i = 0; i < MAX_BOMBS; i++)
    {
      if (bomb[i].alive)
	{
	  if (bomb[i].x == x && bomb[i].y == y)
	    {
	      kill_bomb(i);
	      makeexplosion(bomb[i].x, bomb[i].y, bomb[i].size);
	      ret = 1;
	    }
	}
    }
  
  return (ret);
}


/* Returns whether a player's move is valid (not off the screen, and
   not into some object) */

int moveok(int pln, int xm, int ym)
{
  int c, x, y, ok, i, found, xx, yy;
  
  
  ok = 1;
  
  x = player[pln].x + xm;
  y = player[pln].y + ym;
  
  if (x < 0)
    x = MAP_WIDTH - 1;
  if (x > MAP_WIDTH - 1)
    x = 0;
  if (y < 0)
    y = MAP_HEIGHT - 1;
  if (y > MAP_HEIGHT - 1)
    y = 0;
  
  c = seenmap[y][x];
  
  if (c == OBJ_BLOCK || c == OBJ_BLOCK2 || c == OBJ_MAN_UP ||
      c == OBJ_MAN_DOWN || c == OBJ_MAN_LEFT || c == OBJ_MAN_RIGHT)
    {
      /* You can't walk through indestructable blocks or through
	 other players: */
      
      ok = 0;
    }
  else if (c == OBJ_BOX || c == OBJ_TNT)
    {
      /* You can't walk through boxes unless you have the pusher: */
      
      ok = 0;
      
      if (player[pln].pusher == 1)
	{
	  /* See if there's room to shove the block out of the way: */
	  
	  xx = x + xm;

	  if (xx < 0)
	    xx = xx + MAP_WIDTH;
	  if (xx > MAP_WIDTH - 1)
	    xx = xx - MAP_WIDTH;

	  yy = y + ym;
	  
	  if (yy < 0)
	    yy = yy + MAP_HEIGHT;
	  if (yy > MAP_HEIGHT - 1)
	    yy = yy - MAP_HEIGHT;


	  if (seenmap[yy][xx] == OBJ_DIRT)
	    {
	      /* Shove the block: */
	      
	      map[yy][xx] = c;
	      map[y][x] = OBJ_DIRT;
	      
	      ok = 1;
	      
	      playsound(SND_PUSHER);
	    }
	}
    }
  else if (c == OBJ_BOMB || c == OBJ_RADIO_BOMB)
    {
      /* You can't walk through bombs unless you have the kicker: */
      
      ok = 0;
      
      if (player[pln].kicker == 1)
	{
	  found = -1;
	  
	  for (i = 0; i < MAX_BOMBS; i++)
	    if (bomb[i].x == x && bomb[i].y == y)
	      found = i;
	  
	  if (found != -1)
	    {
	      bomb[found].moving = 1;
	      bomb[found].xm = xm;
	      bomb[found].ym = ym;
	      
	      ok = 1;
	      
	      playsound(SND_KICKER);
	    }
	}
    }
  
  return(ok);
}


/* Updates the status line at the bottom: */

void redrawstatus(int pln)
{
  int i, which, realpln, x;
  char text[128];
  
  
  realpln = real(pln);
  
  x = 32 * same[pln];
  
  if (pln < num_servers)
    {
      /* Show who's alive: */
      
      for (i = 0; i < MAX_PLAYERS; i++)
	{
	  /* Draw white guy for you, tan guy for other players, or explosion
	     for dead players: */
	  
	  if (player[i].alive)
	    {
	      if (use_color == 0)
		{
		  if (i == pln)
		    {
		      XCopyPlane(display[realpln],
				 object_pixmaps[realpln][OBJ_MAN_DOWN],
				 window[realpln], yougc[realpln],
				 0, 0, 32, 32, i * 32, MAP_HEIGHT * 32 + x, 1);
		    }
		  else
		    {
		      XCopyPlane(display[realpln],
				 object_pixmaps[realpln][OBJ_MAN_DOWN],
				 window[realpln],
				 colorgcs[realpln][OBJ_MAN_DOWN],
				 0, 0, 32, 32, i * 32, MAP_HEIGHT * 32 + x, 1);
		    }
		}
	      else
		{
		  if (i == pln)
		    {
		      XCopyArea(display[realpln],
				object_pixmaps[realpln][OBJ_YOU_DOWN],
				window[realpln], whitegc[realpln],
				0, 0, 32, 32, i * 32, MAP_HEIGHT * 32 + x);
		    }
		  else
		    {
		      XFillRectangle(display[realpln], window[realpln],
				     blackgc[realpln],
				     i * 32, MAP_HEIGHT * 32 + x,
				     32, 32);
		      
		      drawblock(pln, i, MAP_HEIGHT + (x / 32), OBJ_MAN_DOWN);
		    }
		}
	    }
	  else
	    {
	      XFillRectangle(display[realpln], window[realpln],
			     blackgc[realpln], i * 32, MAP_HEIGHT * 32 + x,
			     32, 32);
	      
	      drawblock(pln, i, MAP_HEIGHT + (x / 32), OBJ_EXPLOSION1);
	    }
	  
	  
	  /* Draw their score on their dude icon: */
	  
	  sprintf(text, "%d", player[i].score);
	  
	  drawtext(display[realpln], window[realpln], whitegc[realpln], 
		   i * 32 + 13, (MAP_HEIGHT + 1) * 32 - fh[realpln] / 2 + x,
		   text);
	}
      
      
      /* Show what type of bomb(s) you have and how many: */
      
      if (player[pln].alive)
	{
	  if (player[pln].radiobomb == 0)
	    {
	      which = OBJ_BOMB;
	      sprintf(text, "x%d ", player[pln].bombs_max);
	    }
	  else if (player[pln].radiobomb == 1)
	    {
	      which = OBJ_RADIO_BOMB;
	      strcpy(text, "   ");
	    }
	}
      else
	{
	  which = OBJ_DIRT;
	  strcpy(text, "   ");
	}
      
      if (use_color == 0)
	XCopyPlane(display[realpln], object_pixmaps[realpln][which],
		   window[realpln], colorgcs[realpln][which],
		   0, 0, 32, 32,
		   (MAX_PLAYERS + 1) * 32, MAP_HEIGHT * 32 + x, 1);
      else
	{
	  if (which == OBJ_DIRT)
	    XFillRectangle(display[realpln], window[realpln], blackgc[realpln],
			   (MAX_PLAYERS + 1) * 32, MAP_HEIGHT * 32 + x,
			   32, 32);
	  else
	    XCopyArea(display[realpln], object_pixmaps[realpln][which],
		      window[realpln], whitegc[realpln],
		      0, 0, 32, 32,
		      (MAX_PLAYERS + 1) * 32, MAP_HEIGHT * 32 + x);
	}
      
      drawtext(display[realpln], window[realpln], whitegc[realpln], 
	       (MAX_PLAYERS + 2) * 32,
	       (MAP_HEIGHT + 1) * 32 - fh[realpln] / 2 + x, text);
      
      
      /* Show how big their bombs are: */
      
      if (player[pln].alive)
	{
	  if (player[pln].fire_size < MAX_FIRE_SIZE)
	    which = OBJ_MOREFIRE;
	  else
	    which = OBJ_TOTALFIRE;
	  
	  sprintf(text, "x%d ", player[pln].fire_size);
	}
      else
	{
	  which = OBJ_DIRT;
	  strcpy(text, "   ");
	}
      
      if (use_color == 0)
	XCopyPlane(display[realpln], object_pixmaps[realpln][which],
		   window[realpln], colorgcs[realpln][which],
		   0, 0, 32, 32,
		   (MAX_PLAYERS + 3) * 32, MAP_HEIGHT * 32 + x, 1);
      else
	drawblock(pln, MAX_PLAYERS + 3, MAP_HEIGHT + (x / 32), which);
      
      drawtext(display[realpln], window[realpln], whitegc[realpln], 
	       (MAX_PLAYERS + 4) * 32,
	       ((MAP_HEIGHT + 1) * 32 - fh[realpln] / 2 + x),
	       text);
      
      
      /* Show pusher, if you have it: */
      
      if (player[pln].pusher == 1 && player[pln].alive)
	which = OBJ_PUSHER;
      else
	which = OBJ_DIRT;
      
      if (use_color == 0)
	XCopyPlane(display[realpln], object_pixmaps[realpln][which],
		   window[realpln], colorgcs[realpln][which],
		   0, 0, 32, 32,
		   (MAX_PLAYERS + 5) * 32, MAP_HEIGHT * 32 + x, 1);
      else
	drawblock(pln, MAX_PLAYERS + 5, MAP_HEIGHT + (x / 32), which);
      
      
      /* Show kicker, if you have it: */
      
      if (player[pln].kicker == 1 && player[pln].alive)
	which = OBJ_KICKER;
      else
	which = OBJ_DIRT;
      
      
      if (use_color == 0)
	XCopyPlane(display[realpln], object_pixmaps[realpln][which],
		   window[realpln], colorgcs[realpln][which],
		   0, 0, 32, 32,
		   (MAX_PLAYERS + 7) * 32, MAP_HEIGHT * 32 + x, 1);
      else
	drawblock(pln, MAX_PLAYERS + 7, MAP_HEIGHT + (x / 32), which);
      
      
      /* Draw current status message: */
      
      drawtext(display[realpln], window[realpln], whitegc[realpln],
	       (MAX_PLAYERS + 9) * 32,
	       (MAP_HEIGHT + 1) * 32 - fh[realpln] / 2 + x, 
	       ".................................");
      drawtext(display[realpln], window[realpln], whitegc[realpln],
	       (MAX_PLAYERS + 9) * 32,
	       (MAP_HEIGHT + 1) * 32 - fh[realpln] / 2 + x,
	       message);
    }
}


/* Drop a bomb if we can: */

int dropbomb(int pln)
{
  int i, found, ret;
  
  if (player[pln].bombs_out < player[pln].bombs_max)
    {
      player[pln].bombs_out++;
      
      found = -1;
      
      for (i = 0; i < MAX_BOMBS; i++)
	if (bomb[i].alive == 0)
	  found = i;
      
      if (found != -1)
	{
	  bomb[found].alive = 1;
	  bomb[found].x = player[pln].x;
	  bomb[found].y = player[pln].y;
	  if (player[pln].radiobomb == 0)
	    bomb[found].shape = OBJ_BOMB;
	  else
	    bomb[found].shape = OBJ_RADIO_BOMB;
	  bomb[found].owner = pln;
	  bomb[found].time = BOMB_COUNTDOWN - (level / 10) * 10;
	  bomb[found].size = player[pln].fire_size;
	  bomb[found].moving = 0;
	  
	  player[pln].radio_detonate = 0;
	}
      
      ret = 1;
    }
  else
    ret = 0;
  
  return(ret);
}


void initlevel(void)
{
  int i, x, y, pln, c;
  FILE * fi;
  char filename[128];
  
  
  /* Init bombs: */
  
  for (i = 0; i < MAX_BOMBS; i++)
    bomb[i].alive = 0;

  
  /* Init explosions: */
  
  for (i = 0; i < MAX_EXPLOSIONS; i++)
    explosion[i].alive = 0;
  
  
  /* Init map: */
  
  /* (Clear) */
  for (y = 0; y < MAP_HEIGHT; y++)
    for (x = 0; x < MAP_WIDTH; x++)
      map[y][x] = OBJ_DIRT;
  
  if (lev_usetnt[(level - 1) % LEV_MOD])
    {
      /* (Random TNT) */
      for (i = 0; i < HOW_MANY_TNTS; i++)
	map[randnum(MAP_HEIGHT)][randnum(MAP_WIDTH)] = OBJ_TNT;
    }
  
  if (lev_usebox[(level - 1) % LEV_MOD])
    {
      /* (Random boxes) */
      for (i = 0; i < HOW_MANY_BOXES; i++)
	map[randnum(MAP_HEIGHT)][randnum(MAP_WIDTH)] = OBJ_BOX;
    }
  
  if (lev_usewarp[(level - 1) % LEV_MOD])
    {
      /* (Random warp spots) */
      for (i = 0; i < HOW_MANY_WARPS; i++)
	map[randnum(MAP_HEIGHT)][randnum(MAP_WIDTH)] = OBJ_WARP;
    }
  
  
  /* Load level layout on top of this: */
  
  sprintf(filename, "levels/level%.2d.dat", level);
  
  fi = fopen(filename, "r");
  if (fi == NULL)
    {
      perror(filename);
      exit(1);
    }
  
  
  for (y = 0; y < MAP_HEIGHT; y++)
    {
      for (x = 0; x < MAP_WIDTH; x++)
	{
	  c = fgetc(fi);
	  
	  if (c == '#')
	    map[y][x] = OBJ_BLOCK;
	  else if (c == '@')
	    map[y][x] = OBJ_BLOCK2;
	}
      fgetc(fi);
    }
  
  fclose(fi);
  
  
  /* (Clear spaces next to players, to keep them from being trapped) */
  
  map[1][1] = OBJ_DIRT;
  map[2][1] = OBJ_DIRT;
  map[1][2] = OBJ_DIRT;
  
  map[MAP_WIDTH - 2][1] = OBJ_DIRT;
  map[MAP_WIDTH - 3][1] = OBJ_DIRT;
  map[MAP_WIDTH - 2][2] = OBJ_DIRT;
  
  map[MAP_WIDTH - 2][MAP_HEIGHT - 2] = OBJ_DIRT;
  map[MAP_WIDTH - 3][MAP_HEIGHT - 2] = OBJ_DIRT;
  map[MAP_WIDTH - 2][MAP_HEIGHT - 3] = OBJ_DIRT;
  
  map[1][MAP_HEIGHT - 2] = OBJ_DIRT;
  map[2][MAP_HEIGHT - 2] = OBJ_DIRT;
  map[1][MAP_HEIGHT - 3] = OBJ_DIRT;
  
  
  /* Copy entire map onto main seen map: */
  
  for (y = 0; y < MAP_HEIGHT; y++)
    for (x = 0; x < MAP_WIDTH; x++)
      seenmap[y][x] = map[y][x];
  
  
  /* Clear each player's seen map: */
  
  for (pln = 0; pln < num_servers; pln++)
    for (y = 0; y < MAP_HEIGHT; y++)
      for (x = 0; x < MAP_WIDTH; x++)
	player[pln].seenmap[y][x] = -1;
  
  
  /* Init player stuff: */
  
  for (i = 0; i < MAX_PLAYERS; i++)
    {
      if (i == 0)
	{
	  player[i].x = 1;
	  player[i].y = 1;
	}
      else if (i == 1)
	{
	  player[i].x = MAP_WIDTH - 2;
	  player[i].y = MAP_HEIGHT - 2;
	}
      else if (i == 2)
	{
	  player[i].x = MAP_WIDTH - 2;
	  player[i].y = 1;
	}
      else if (i == 3)
	{
	  player[i].x = 1;
	  player[i].y = MAP_HEIGHT - 2;
	}

      player[i].alive = 1;
      player[i].bombs_out = 0;
      player[i].mushroomtime = 0;
      player[i].shieldtime = 0;
      player[i].invisibletime = 0;
      
      notes[i].alive = 0;
    }
  
  
  /* Init level text: */
  
  sprintf(message, "LEVEL: %d", level);
  winner = -1;
  
  
  /* Init level countdown time: */
  
  leveltime = 8000;
  
  
  /* Init death: */
  
  death.alive = 0;
  
  
  /* Play sound: */
  
  playnsound(NSND_LEVEL);
  if (use_sound)
    sleep(1);
  
  if (level < 16)
    playnsound(level);
  else
    {
      if (level >= 16 && level < 20)
	{
	  playnsound(level - 10);
	  if (use_sound)
	    sleep(1);
	  playnsound(NSND_TEEN);
	}
      else
	{
	  if (level >= 20 && level < 30)
	    playnsound(NSND_20);
	  else if (level >= 30 && level < 40)
	    playnsound(NSND_30);
	  else if (level >= 40 && level < 50)
	    playnsound(NSND_40);
	  else if (level >= 50 && level < 60)
	    playnsound(NSND_50);
	  else if (level >= 60 && level < 70)
	    playnsound(NSND_60);
	  else if (level >= 70 && level < 80)
	    playnsound(NSND_70);
	  else if (level >= 80 && level < 90)
	    playnsound(NSND_80);
	  else if (level >= 90)
	    playnsound(NSND_90);

	  if ((level % 10) != 0)
	    {
	      if (use_sound)
		sleep(1);
	      playnsound(level % 10);
	    }
	}
    }
  
  background_mode = randnum(NUM_BACKGROUND_MODES);
}


/* Init. player goodies (beginning of game, and when you die): */

void initplayergoodies(int i)
{
  player[i].bombs_max = 1;
  player[i].fire_size = 2;
  player[i].radiobomb = 0;
  player[i].pusher = 0;
  player[i].kicker = 0;
  
  player[i].cmp_time = 0;
  player[i].cmp_dir = 4;
  player[i].shape = OBJ_MAN_DOWN;
}


/* Actually looks for an explosion slot and places an explosion at x,y: */

void addexplosion_atomic(int x, int y)
{
  int i, found;
  
  
  found = -1;
  
  for (i = 0; i < MAX_EXPLOSIONS; i++)
    {
      if (explosion[i].alive == 0)
	found = i;
    }
  
  if (found != -1)
    {
      explosion[found].alive = 1;
      explosion[found].time = 16;
      explosion[found].x = x;
      explosion[found].y = y;
    }
}


/* Control a computer player: */

void control_computer(int pln)
{
  int pick_new_dir, c, count, i, old_dir;
  
  pick_new_dir = 0;
  
  if (cmp_movetoggle == 0)
    {
      if (player[pln].cmp_time >= 0)
	{
	  /* Move (if we still can): */
	  
	  if (moveok(pln, xms[player[pln].cmp_dir], yms[player[pln].cmp_dir]))
	    {
	      player[pln].x = player[pln].x + xms[player[pln].cmp_dir];
	      
	      if (player[pln].x < 0)
		player[pln].x = MAP_WIDTH - 1;
	      if (player[pln].x > MAP_WIDTH - 1)
		player[pln].x = 0;	
	      
	      player[pln].y = player[pln].y + yms[player[pln].cmp_dir];
	      
	      if (player[pln].y < 0)
		player[pln].y = MAP_HEIGHT - 1;
	      if (player[pln].y > MAP_HEIGHT - 1)
		player[pln].y = 0;
	    }
	  else
	    {
	      if (player[pln].x > 0 && player[pln].x < MAP_WIDTH - 1 &&
		  player[pln].y > 0 && player[pln].y < MAP_HEIGHT - 1)
		{
		  c = (map[player[pln].y + yms[player[pln].cmp_dir]]
		       [player[pln].x + xms[player[pln].cmp_dir]]);
		  
		  if (((c == OBJ_BOX || c == OBJ_TNT) && randnum(10) < 3) ||
		      randnum(50) < 1)
		    {
		      if (winner == -1)
			dropbomb(pln);
		    }
		}
	    }
	  
	  player[pln].cmp_time--;
	}
      else
	{
	  pick_new_dir = 1;
	}
      
      
      /* Check for bombs and explosions!: */
      
      for (i = 0; i < MAX_BOMBS; i++)
	{
	  if (bomb[i].alive && (bomb[i].x == player[pln].x ||
				bomb[i].y == player[pln].y))
	    pick_new_dir = 2;
	}
      
      for (i = 0; i < MAX_EXPLOSIONS; i++)
	{
	  if (explosion[i].alive && (explosion[i].x == player[pln].x ||
				     explosion[i].y == player[pln].y))
	    pick_new_dir = 2;
	}
      
      
      /* Pick a new direction(?): */
      
      if (pick_new_dir != 0)
	{
	  count = 0;
	  
	  if (pick_new_dir == 1)
	    old_dir = (player[pln].cmp_dir + 2) % 4;
	  else
	    old_dir = -1;
	  
	  
	  /* Get the hell away from bombs right next to you! */
	  
	  player[pln].cmp_dir = -1;
	  
	  if (player[pln].y > 0)
	    {
	      if (map[player[pln].y - 1][player[pln].x] == OBJ_BOMB ||
		  map[player[pln].y - 1][player[pln].x] == OBJ_RADIO_BOMB)
		{
		  player[pln].cmp_dir = 2;
		}
	    }
	  
	  if (player[pln].y < MAP_HEIGHT - 1)
	    {
	      if (map[player[pln].y + 1][player[pln].x] == OBJ_BOMB ||
		  map[player[pln].y + 1][player[pln].x] == OBJ_RADIO_BOMB)
		{
		  player[pln].cmp_dir = 0;
		}
	    }
	  
	  if (player[pln].x > 0)
	    {
	      if (map[player[pln].y][player[pln].x - 1] == OBJ_BOMB ||
		  map[player[pln].y][player[pln].x - 1] == OBJ_RADIO_BOMB)
		{
		  player[pln].cmp_dir = 1;
		}
	    }
	  
	  if (player[pln].x < MAP_WIDTH - 1)
	    {
	      if (map[player[pln].y][player[pln].x + 1] == OBJ_BOMB ||
		  map[player[pln].y][player[pln].x + 1] == OBJ_RADIO_BOMB)
		{
		  player[pln].cmp_dir = 3;
		}
	    }
	  
	  
	  if (player[pln].cmp_dir == -1)
	    {
	      /* Look for a random direction: */
	      
	      do
		{
		  player[pln].cmp_dir = randnum(6 - pick_new_dir);
		  count++;
		}
	      while (moveok(pln, xms[player[pln].cmp_dir],
			    yms[player[pln].cmp_dir]) == 0 && count < 20 &&
		     player[pln].cmp_dir == old_dir);
	    }
	  
	  
	  /* Point the dude in that direction: */
	  
	  if (player[pln].cmp_dir == 0)
	    player[pln].shape = OBJ_MAN_UP;
	  else if (player[pln].cmp_dir == 1)
	    player[pln].shape = OBJ_MAN_RIGHT;
	  else if (player[pln].cmp_dir == 2)
	    player[pln].shape = OBJ_MAN_DOWN;
	  else if (player[pln].cmp_dir == 3)
	    player[pln].shape = OBJ_MAN_LEFT;
	  
	  
	  /* Make them walk for a while: */
	  
	  player[pln].cmp_time = randnum(30) + 5;
	}
    }
}


/* Make death find another player to chase, or turn death off if everyone's
   dead: */

void make_death_seek(void)
{
  int count;
  
  
  death.seek = -1;
  count = 0;
  
  do
    {
      count = count + 1;
      
      death.seek = randnum(MAX_PLAYERS);
    }
  while (player[death.seek].alive == 0 && count < 10);
  
  
  if (death.seek == -1)
    death.alive = 0;
}


/* 1 if a spot is empty, 0 if not: */

int empty(int x, int y)
{
  if (y < 0 || y > MAP_HEIGHT - 1 || x < 0 || x > MAP_WIDTH - 1)
    return(0);
  
  if (map[y][x] == OBJ_BLOCK || map[y][x] == OBJ_BLOCK2 ||
      map[y][x] == OBJ_BOX || map[y][x] == OBJ_TNT)
    return(0);
  else
    return(1);
}


/* Kill a bomb and give it back to the player: */

void kill_bomb(int i)
{
  bomb[i].alive = 0;
  player[bomb[i].owner].bombs_out--;
}


/* Load color objects: */

void loadobject(int pln, int i)
{
  int x, y, red, green, blue, color_pos, width, height;
  char c;
  char file[512], line[512];
  FILE * fi;
  GC colorgc[256], color0gc, color1gc;
  XColor xcol;
  XGCValues xgcv, penxgcv;
  unsigned long color_want;
  
  c = '.';
  
  fi = NULL;
  
  if (strlen(graphicsset) != 0)
    {
      sprintf(file, "%s/%s.ppm", graphicsset, object_names[i]);
      fi = fopen(file, "r");
      
      c = graphicsset[0];
    }
  
  if (fi == NULL)
    {
      c = '.';
      
      sprintf(file, "pixmaps/%s.ppm", object_names[i]);
      
      fi = fopen(file, "r");
      if (fi == NULL)
	{
	  perror(file);
	  exit(1);
	}
    }
  
  
  fprintf(stderr, "%c", c);
  fflush(stderr);
  
  
  fscanf(fi, "%s %d %d %s", line, &width, &height, &line);
  fgetc(fi);
  
  
  color_clear();
  
  object_pixmaps[pln][i] =
    XCreatePixmap(display[pln], window[pln], width, height, 
                  DefaultDepthOfScreen(DefaultScreenOfDisplay(display[pln])));
  
  XFillRectangle(display[pln], object_pixmaps[pln][i], whitegc[pln], 0, 0,
		 width, height);
  
  object_masks[pln][i] = 
    XCreatePixmap(display[pln], window[pln], width, height, 1);
  
  xgcv.foreground = 0;
  color0gc = XCreateGC(display[pln], object_masks[pln][i], GCForeground,
		       &xgcv);
  
  xgcv.foreground = 1;
  color1gc = XCreateGC(display[pln], object_masks[pln][i], GCForeground,
		       &xgcv);
  
  XFillRectangle(display[pln], object_masks[pln][i], color0gc,
		 0, 0, width, height);
  
  if (!SetUpColormap(display[pln], screen[pln], object_pixmaps[pln][i],
                     temp_visual[pln], &colormap[pln]))
    {
      fprintf(stderr, "Error: Could not set up colormap!\n");
      exit(1);
    }
  
  
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
	  red = fgetc(fi);
	  green = fgetc(fi);
	  blue = fgetc(fi);
          
          if (red != 255 || green != 255 || blue != 255)
            {
              color_pos = color_seen(red, green, blue);
              
              if (color_pos == -1)
                {
                  xcol.red = red * 256;
                  xcol.green = green * 256;
                  xcol.blue = blue * 256;
                  xcol.flags = DoRed | DoGreen | DoBlue;
                  
                  XAllocColor(display[pln], colormap[pln], &xcol);
                  
                  color_pos = color_add(red, green, blue, 
                                        xcol.pixel, i);
                  color_want = xcol.pixel;
                  
                  xgcv.foreground = color_want;
                  colorgc[color_pos] = XCreateGC(display[pln], window[pln],
                                                 GCForeground, &xgcv);
                }
              else
                {
                  color_want = color_list[color_pos].pixel;
                  
                  if (color_list[color_pos].owner != i)
                    {
                      xgcv.foreground = color_want;
                      
                      XFreeGC(display[pln], colorgc[color_pos]);
                      colorgc[color_pos] =
                        XCreateGC(display[pln], object_pixmaps[pln][i],
                                  GCForeground, &xgcv);
                      
                      color_list[color_pos].owner = i;
                    }
                }
              
              XDrawPoint(display[pln], object_pixmaps[pln][i],
			 colorgc[color_pos], x, y);
              XDrawPoint(display[pln], object_masks[pln][i], color1gc, x, y);
            }
	}
    }
  
  colorgcs[pln][i] =
    CreateGC(display[pln], window[pln], white[pln], white[pln]);

  XSetClipMask(display[pln], colorgcs[pln][i], object_masks[pln][i]);
  
  fclose(fi);
}


void color_clear(void)
{
  num_seen_colors = 0;
}


int color_seen(unsigned int red, unsigned int green, unsigned int blue)
{
  int i, found;
  
  found = -1;
  
  for (i = 0; i < num_seen_colors; i++)
    {
      if (color_list[i].red == red &&
          color_list[i].green == green &&
          color_list[i].blue == blue)
        found = i;
    }
  
  return found;
}


int color_add(unsigned int red, unsigned int green, unsigned int blue,
               unsigned long pixel, int owner)
{
  color_list[num_seen_colors].red = red;
  color_list[num_seen_colors].green = green;
  color_list[num_seen_colors].blue = blue;
  color_list[num_seen_colors].pixel = pixel;
  color_list[num_seen_colors].owner = owner;
  
  num_seen_colors++;
  
  return(num_seen_colors - 1);
}


/* Real player's window (our own, or the one we're leeching off of by using
   a SAME server): */

int real(int pln)
{
  return(pln - same[pln]);
}
