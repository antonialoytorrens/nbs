/*
  flerror.cxx
  
  Original author unknown.  From irobexd-0.1 from Agenda Computing.
  
  Updated by Bill Kendrick <bill@newbreedsoftware.com>
  Last update 2001.10.13 13:58 PDT
    * No longer segfaults when no string sent
    * String can be made of multiple arguments (eg, "flerror Hello there")
    * Can run in various modes:
      -message  (default, same as original - just Ok)
      -ask      (Ok/Cancel - sent via return value)
      -input    (text input field - sent via stdout)
      -password (cleartext input - sent via stdout)
    * Displays usage when called inappropriately or with -usage or -help
    * Plain "-" causes args to stop being interpreted as options
      (ie, you can do: "flerror - -help shows help")
*/


#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* Modes in which this program can run: */

enum {
  MODE_MESSAGE,     /* Default */
  MODE_ASK,
  //  MODE_CHOICE,
  MODE_INPUT,
  MODE_PASSWORD
};


/* Local function prototypes: */

void usage(char * cmd, int val);



/* --- MAIN --- */

int main(int argc, char* argv[])
{
  int i, j, size, mode, ret;
  char * str;
  const char * outstr;
  
  
  /* Deal with options: */
  
  mode = MODE_MESSAGE;

  for (i = 1; i < argc && argv[i][0] == '-'; i++)
    {
      if (strcmp(argv[i], "-message") == 0)
	mode = MODE_MESSAGE;
      else if (strcmp(argv[i], "-ask") == 0)
	mode = MODE_ASK;
      // else if (strcmp(argv[i], "-choice") == 0)
      // mode = MODE_CHOICE;
      else if (strcmp(argv[i], "-input") == 0)
	mode = MODE_INPUT;
      else if (strcmp(argv[i], "-password") == 0)
	mode = MODE_PASSWORD;
      else if (strcmp(argv[i], "-usage") == 0 ||
	       strcmp(argv[i], "-help") == 0)
	usage(argv[0], 0);
      else if (strcmp(argv[i], "-") == 0)
        {
          /* End of args!  They must want text starting with "-" */
	  
	  i++;
          break;
        }
      else
	{
	  fprintf(stderr, "Unknown option: %s\n", argv[i]);
	  usage(argv[0], 1);
	}
    }
  
  
  if (i < argc)
    {
      /* Allocate space for remaining args (text): */
      
      size = 0;
      for (j = i; j < argc; j++)
	size = size + strlen(argv[j]) + 1;
      str = (char *) malloc(sizeof(char) * size);
      
      
      /* Create the text: */
      
      str[0] = '\0';
      for (j = i; j < argc; j++)
	{
	  strcat(str, argv[j]);
	  
	  if (j < argc - 1)
	    strcat(str, " ");
	}
    }
  else
    {
      usage(argv[0], 1);
    }
  
  
  /* Display the window!: */
  
  fl_message_size_ = 18;
  ret = 0;
  outstr = "";
  
  if (mode == MODE_MESSAGE)
    fl_message(str);
  else if (mode == MODE_ASK)
    ret = !fl_ask(str);
  //else if (mode == MODE_CHOICE)
    // FIXME: How should this be handled!?
    // fl_choice(str);
  else if (mode == MODE_INPUT)
    outstr = fl_input(str);
  else if (mode == MODE_PASSWORD)
    outstr = fl_password(str);
  
  Fl::flush();

  if (outstr == NULL)
    ret = 1;
  else
  {
    if (strlen(outstr) > 0)
      printf("%s\n", outstr);
  }
  
  return(ret);
}


/* Usage message/error: */

void usage(char * cmd, int val)
{
  FILE * f;
  
  /* stdout for message; stderr for error: */
  
  if (val == 0)
    f = stdout;
  else
    f = stderr;
  
  
  fprintf(f, "\nUsage: %s [(-message) | -ask "/*| -choice */"| -input | -password] text ...\n", cmd);
  fprintf(f, "       %s -usage\n\n", cmd);

  fprintf(f, "  -message  - Display text and an 'Ok' button\n");
  fprintf(f, "  -ask      - Display text and 'Ok' and 'Cancel' buttons\n"
	     "              Return value will be 0 for Ok, 1 for Cancel\n");
  //  fprintf(f, "-choice   - NOT IMPLEMENTED YET\n");
  fprintf(f, "  -input    - Display text and a text-input field\n"
	     "              Inputted text will be echoed to stdout\n"
             "              Returns -1 if Cancel clicked\n");
  fprintf(f, "  -password - Display text and a cleartext text-input field\n"
	     "              Inputted text will be echoed to stdout\n"
             "              Returns -1 if Cancel clicked\n");
  fprintf(f, "  -usage    - Display this usage\n\n");
  
  
  /* Exit (with the error code) */
  
  exit(val);
}
