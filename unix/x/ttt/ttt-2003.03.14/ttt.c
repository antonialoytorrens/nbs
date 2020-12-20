/* ttt.c
 
   A very simple tic-tac-toe game.
   
   bill kendrick
   bill@newbreedsoftware.com

   2003.Mar.14
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "SDL.h"
#include "SDL_mixer.h"


/* Globals: */

int winning_style, winning_pos;
enum {
  HORIZ,
  VERT,
  DIAG
};


/* Local function prototypes: */

void myabort(char * str);
int who_won(int g[3][3]);
void dupgrid(int src[3][3], int dest[3][3]);
SDL_Surface * my_img_load(char * fname);


/* --- MAIN FUNCTION: --- */

int main(int argc, char * argv[])
{
  SDL_Surface * screen, * img_x, * img_o, * img_grid, * img_title,
    * img_win_x, * img_win_o, * img_win_draw,
    * img_turn_x, * img_turn_o,
    * img_start_1ply, * img_start_2ply;
  Mix_Chunk * snd_intro, * snd_x, * snd_o, * snd_badspot, * snd_win, * snd_draw,
    * snd_start;
  SDL_Rect dest;
  int done, x, y, player, winner, use_sound, game_over, yy, vs_computer,
    want_x, want_y;
  int grid[3][3], temp_grid[3][3];
  SDL_Event event;
  

  /* Initialize: */
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    myabort("SDL_Init");

  screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
  if (screen == NULL)
    myabort("SDL_SetVideoMode");

  use_sound = 1;
  if (SDL_Init(SDL_INIT_AUDIO) < 0)
    use_sound = 0;

  if (use_sound)
    if (Mix_OpenAudio(44100, AUDIO_S16, 2, 512) < 0)
      myabort("Mix_OpenAudio");
  

  /* Load images: */
  
  img_grid = my_img_load("images/grid.bmp");
  if (img_grid == NULL)
    myabort("grid.bmp");

  img_x = my_img_load("images/x.bmp");
  if (img_x == NULL)
    myabort("x.bmp");

  img_o = my_img_load("images/o.bmp");
  if (img_o == NULL)
    myabort("o.bmp");

  img_title = my_img_load("images/title.bmp");
  if (img_title == NULL)
    myabort("title.bmp");

  img_turn_x = my_img_load("images/turn_x.bmp");
  if (img_turn_x == NULL)
    myabort("turn_x.bmp");
  
  img_turn_o = my_img_load("images/turn_o.bmp");
  if (img_turn_o == NULL)
    myabort("turn_o.bmp");
  
  img_win_x = my_img_load("images/win_x.bmp");
  if (img_win_x == NULL)
    myabort("win_x.bmp");

  img_win_o = my_img_load("images/win_o.bmp");
  if (img_win_o == NULL)
    myabort("win_o.bmp");

  img_win_draw = my_img_load("images/win_draw.bmp");
  if (img_win_draw == NULL)
    myabort("win_draw.bmp");

  img_start_1ply = my_img_load("images/start_1ply.bmp");
  if (img_start_1ply == NULL)
    myabort("win_start_1ply.bmp");

  img_start_2ply = my_img_load("images/start_2ply.bmp");
  if (img_start_2ply == NULL)
    myabort("win_start_2ply.bmp");

  
  /* Load sounds: */

  if (use_sound)
  {
    snd_intro = Mix_LoadWAV("sounds/intro.wav");
    if (snd_intro == NULL)
      myabort("intro.wav");
    
    snd_x = Mix_LoadWAV("sounds/x.wav");
    if (snd_x == NULL)
      myabort("x.wav");
    
    snd_o = Mix_LoadWAV("sounds/o.wav");
    if (snd_o == NULL)
      myabort("o.wav");
    
    snd_badspot = Mix_LoadWAV("sounds/badspot.wav");
    if (snd_badspot == NULL)
      myabort("badspot.wav");
    
    snd_win = Mix_LoadWAV("sounds/win.wav");
    if (snd_win == NULL)
      myabort("win.wav");

    snd_draw = Mix_LoadWAV("sounds/draw.wav");
    if (snd_draw == NULL)
      myabort("draw.wav");
    
    snd_start = Mix_LoadWAV("sounds/start.wav");
    if (snd_start == NULL)
      myabort("start.wav");
  }
  else
  {
    snd_intro = NULL;
    snd_x = NULL;
    snd_o = NULL;
    snd_badspot = NULL;
    snd_win = NULL;
    snd_draw = NULL;
    snd_start = NULL;
  }
  


  /* Reset game: */

  for (y = 0; y < 3; y++)
    for (x = 0; x < 3; x++)
      grid[y][x] = -1;

  done = 0;
  player = 0;
  winner = -1;
  game_over = 1;
  vs_computer = 0;


  /* Clear screen: */
  
  SDL_BlitSurface(img_grid, NULL, screen, NULL);
  
  dest.x = 240;
  dest.y = 0;
  SDL_BlitSurface(img_title, NULL, screen, &dest);

  dest.x = 240;
  dest.y = 120;
  SDL_BlitSurface(img_start_1ply, NULL, screen, &dest);
  
  dest.x = 240;
  dest.y = 160;
  SDL_BlitSurface(img_start_2ply, NULL, screen, &dest);

  for (y = 120; y < 200; y = y + 2)
  {
    dest.x = 240;
    dest.y = y;
    dest.w = 80;
    dest.h = 1;

    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));
  }
  
  SDL_Flip(screen);


  /* Play intro sound: */

  if (use_sound)
    Mix_PlayChannel(0, snd_intro, 0);
 

  /* --- MAIN GAME LOOP! --- */

  do
  {
    /* Wait for an event: */
	  
    SDL_WaitEvent(&event);
    

    /* Deal with events: */
    
    if (event.type == SDL_QUIT)
    {
      /* Window close event?  Quit! */
	    
      done = 1;
    }
    else if (event.type == SDL_KEYDOWN)
    {
      /* Key press! */
	    
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
	/* [Escape]?  Quit! */

        done = 1;
      }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
      /* Mouse click! */

      if (event.button.x < 240 && event.button.y < 240)
      {
        x = event.button.x / 84;
        y = event.button.y / 84;


        /* Within the grid, and game not finished? */

        if (x >= 0 && x < 3 && y >= 0 && y < 3 && game_over == 0)
        {
	  /* Grid point not already taken? */

          if (grid[y][x] == -1)
          {
	    /* Assign this player's piece there: */
		
            grid[y][x] = player;


	    /* Draw the piece on the board and play a sound: */
	  
            dest.x = x * 84;
            dest.y = y * 84;
          
            if (player == 0)
	    {
              SDL_BlitSurface(img_x, NULL, screen, &dest);
	      if (use_sound)
	        Mix_PlayChannel(0, snd_x, 0);
	    }
            else if (player == 1)
	    {
              SDL_BlitSurface(img_o, NULL, screen, &dest);
	      if (use_sound)
	        Mix_PlayChannel(0, snd_o, 0);
	    }

	  
	    /* See if anyone won: */
	  
	    winner = who_won(grid);

	    if (winner == -1)
	    {
	      /* Check for a draw: */

	      game_over = 1;
	      for (y = 0; y < 3; y++)
	        for (x = 0; x < 3; x++)
 		  if (grid[y][x] == -1)
	 	    game_over = 0;

	      if (game_over)
	      {
	        /* A draw! */

	        dest.x = 240;
	        dest.y = 200;
	        SDL_BlitSurface(img_win_draw, NULL, screen, &dest);

	        if (use_sound)
		  Mix_PlayChannel(0, snd_draw, 0);
	    
	      
	        /* Shade the board: */

	        for (y = 0; y < 3; y++)
	        {
	          for (x = 0; x < 3; x++)
	          {
		    for (yy = 0; yy < 72; yy = yy + 3)
		    {
		      dest.x = x * 84;
		      dest.y = y * 84 + yy;
		      dest.w = 72;
		      dest.h = 2;

		      SDL_FillRect(screen, &dest,
				   SDL_MapRGB(screen->format, 0, 0, 0));
		    }
		  }
	        }
            
		
		/* Shade the game mode buttons: */
		
		for (y = 120; y < 200; y = y + 2)
                {
                  dest.x = 240;
                  dest.y = y;
                  dest.w = 80;
                  dest.h = 1;

                  SDL_FillRect(screen, &dest,
			       SDL_MapRGB(screen->format, 0, 0, 0));
                }

	        SDL_Flip(screen);
	      }
	      else
	      {
	        /* Swap players: */
		    
                player = !player;
  
	        dest.x = 240;
	        dest.y = 200;

	        if (player == 0)
                  SDL_BlitSurface(img_turn_x, NULL, screen, &dest);
	        else if (player == 1)
                  SDL_BlitSurface(img_turn_o, NULL, screen, &dest);

	        SDL_Flip(screen);


		/* Handle computer's move: */
		
		if (player == 1 && vs_computer)
		{
		  /* Pick a random spot, if all else fails: */
		
		  do
		  {
		    want_x = rand() % 3;
		    want_y = rand() % 3;
		  }
		  while (grid[want_y][want_x] != -1);
		  
		  
		  /* Look for a BLOCKING move: */
			
		  for (y = 0; y < 3; y++)
		  {
		    for (x = 0; x < 3; x++)
		    {
		      if (grid[y][x] == -1)
		      {
		        dupgrid(grid, temp_grid);
		        temp_grid[y][x] = 0;

		        if (who_won(temp_grid) == 0)
		        {
			  /* Player will win if they pick this spot! */
			  /* Let's choose it, to block: */

			  want_x = x;
			  want_y = y;
			}
		      }
		    }
		  }
		  
		  
		  /* Look for a WINNING move: */
			
		  for (y = 0; y < 3; y++)
		  {
		    for (x = 0; x < 3; x++)
		    {
		      if (grid[y][x] == -1)
		      {
		        dupgrid(grid, temp_grid);
		        temp_grid[y][x] = 1;

		        if (who_won(temp_grid) == 1)
		        {
			  /* Computer will win if it picks this spot! */
			  /* Choose it!  We'll win!  Yay! */

			  want_x = x;
			  want_y = y;
		        }
		      }
		    }
		  }


		  /* Place the computer's spot: */
		  
		  grid[want_y][want_x] = 1;


		  /* Pretend to think about it for a random amount of time... */

		  SDL_Delay(1000 + (rand() % 1000));
              
		  
		  /* Place the piece on the screen: */
		  
		  dest.x = want_x * 84;
		  dest.y = want_y * 84;

		  SDL_BlitSurface(img_o, NULL, screen, &dest);
	          if (use_sound)
	            Mix_PlayChannel(0, snd_o, 0);

		  SDL_Flip(screen);


		  /* Switch back to the human player: */

		  player = 0;
	        
		  dest.x = 240;
	          dest.y = 200;
                  SDL_BlitSurface(img_turn_x, NULL, screen, &dest);

		  SDL_Flip(screen);
	    
		  winner = who_won(grid);
		}
	      }
	    }

	    if (winner != -1)
	    {
	      /* A winner! */

	      game_over = 1;

	      if (use_sound)
	        Mix_PlayChannel(0, snd_win, 0);


	      /* Show who won: */

	      dest.x = 240;
	      dest.y = 200;

	      if (winner == 0)
	        SDL_BlitSurface(img_win_x, NULL, screen, &dest);
	      else if (winner == 1)
	        SDL_BlitSurface(img_win_o, NULL, screen, &dest);


	      /* Shade non-winning pieces: */

	      for (y = 0; y < 3; y++)
	      {
	        for (x = 0; x < 3; x++)
	        {
		  if ((winning_style == HORIZ && winning_pos == y) ||
		      (winning_style == VERT && winning_pos == x) ||
		      (winning_style == DIAG && winning_pos == 0 && x == y) ||
		    (  winning_style == DIAG && winning_pos == 1 && x == 2 - y))
		  {
		    /* A winning spot...  Do nothing */
		  }
		  else
		  {
		    for (yy = 0; yy < 72; yy = yy + 3)
		    {
		      dest.x = x * 84;
		      dest.y = y * 84 + yy;
		      dest.w = 72;
		      dest.h = 2;

		      SDL_FillRect(screen, &dest,
				   SDL_MapRGB(screen->format, 0, 0, 0));
		    }
		  }
	        }
	      }
		
	      /* Shade the game mode buttons: */
		
	      for (y = 120; y < 200; y = y + 2)
              {
                dest.x = 240;
                dest.y = y;
                dest.w = 80;
                dest.h = 1;

                SDL_FillRect(screen, &dest,
	      	       SDL_MapRGB(screen->format, 0, 0, 0));
              }
	    
	      SDL_Flip(screen);
	    }
          

	    /* Refresh the screen: */

	    SDL_Flip(screen);
          }
	  else
	  { 
	    /* Spot was taken!  Play a bad noise: */

	    if (use_sound)
	      Mix_PlayChannel(0, snd_badspot, 0);
	  }
	}
      }
      else
      {
	if (event.button.y >= 120 && event.button.y < 200)
	{
	  /* Clicked one of the 'start a new game' buttons! */

	  if (event.button.y >= 160)
	    vs_computer = 0;
	  else
	    vs_computer = 1;

	  srand(SDL_GetTicks());

  
	  /* Reset game: */

          for (y = 0; y < 3; y++)
            for (x = 0; x < 3; x++)
              grid[y][x] = -1;

          player = 0;
          winner = -1;
          game_over = 0;


          /* Clear screen: */
  
          SDL_BlitSurface(img_grid, NULL, screen, NULL);
  
          dest.x = 240;
          dest.y = 0;
          SDL_BlitSurface(img_title, NULL, screen, &dest);

          dest.x = 240;
          dest.y = 200;
          SDL_BlitSurface(img_turn_x, NULL, screen, &dest);

          dest.x = 240;
          dest.y = 120;
          SDL_BlitSurface(img_start_1ply, NULL, screen, &dest);
  
          dest.x = 240;
          dest.y = 160;
          SDL_BlitSurface(img_start_2ply, NULL, screen, &dest);

	  if (vs_computer == 1)
	  {
            for (y = 160; y < 200; y = y + 2)
            {
              dest.x = 240;
              dest.y = y;
              dest.w = 80;
              dest.h = 1;

              SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));
            }
	  }
	  else
	  {
            for (y = 120; y < 160; y = y + 2)
            {
              dest.x = 240;
              dest.y = y;
              dest.w = 80;
              dest.h = 1;

              SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));
            }
	  }

          SDL_Flip(screen);


	  if (use_sound)
	    Mix_PlayChannel(0, snd_start, 0);
	}
      }
    }
  }
  while (!done);

  SDL_Quit();

  return (0);
}


/* Determine if anyone one: */

int who_won(int g[3][3])
{
  int i;


  /* Check for horizontal and vertical lines: */

  for (i = 0; i < 3; i++)
  {
    if (g[i][0] == g[i][1] &&
	g[i][0] == g[i][2] &&
	g[i][0] != -1)
    {
      winning_style = HORIZ;
      winning_pos = i;

      return(g[i][0]);
    }
    
    if (g[0][i] == g[1][i] &&
	g[0][i] == g[2][i] &&
	g[0][i] != -1)
    {
      winning_style = VERT;
      winning_pos = i;
      
      return(g[0][i]);
    }
  }


  /* Check top-left-to-bottom-right diagonal: */

  if (g[0][0] == g[1][1] &&
      g[0][0] == g[2][2] &&
      g[0][0] != -1)
  {
    winning_style = DIAG;
    winning_pos = 0;

    return(g[0][0]);
  }

  
  /* Check top-right-to-bottom-left diagonal: */

  if (g[2][0] == g[1][1] &&
      g[2][0] == g[0][2] &&
      g[2][0] != -1)
  {
    winning_style = DIAG;
    winning_pos = 1;

    return(g[2][0]);
  }

  return -1;
}


/* Nice abort function: */

void myabort(char * str)
{
  fprintf(stderr, "Error: %s: %s\n", str, SDL_GetError());
  exit(1);
}


/* Load and convert a BMP: */

SDL_Surface * my_img_load(char * fname)
{
  SDL_Surface * tmp, * tmp2;

  tmp = SDL_LoadBMP(fname);
  if (tmp == NULL)
    return NULL;

  tmp2 = SDL_DisplayFormatAlpha(tmp);
  SDL_FreeSurface(tmp);

  return tmp2;
}


/* Duplicate a grid: */

void dupgrid(int src[3][3], int dest[3][3])
{
  int x, y;

  for (y = 0; y < 3; y++)
    for (x = 0; x < 3; x++)
      dest[y][x] = src[y][x];
}
