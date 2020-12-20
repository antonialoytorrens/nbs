/*
  cgi-util.h
  
  created by Bill Kendrick
  kendrick@zippy.sonoma.edu
  http://zippy.sonoma.edu/~kendrick/
  
  based on "post-query.c" and "query.c" by NCSA
  
  New Breed Software
  
  April 6, 1996 / July 15, 1996 / September 22, 1996 / January 28, 1997
*/


/* USAGE:
   
   In your CGI C source, "#include" the `cgi-util.h' file:

     #include "cgi-util.h"

     
   In your makefile (or when compiling), compile "cgi-util.c" into an
   Object file and link it to your CGI's sourcecode or Object:

     gcc cgi-util.c -c
     gcc program.o cgi-util.o -o program.cgi

   ...or, compile it along with your CGI sourcecode:

     gcc program.c cgi-util.c -o program.cgi


   Example C source:

     #include <stdio.h>
     #include "cgi-util.h"
     
     main()
     {
       char str[100];
       
       cgiinit();
       printf("Content-type: text/html\n\n");
       
       printf("<h1>%s</h1>\n", entries[0].name);  /* First field received * /
       getentry(str, "name");
       printf("For the `name' field, you typed: %s\n", str);
    }
*/

typedef struct
{
  char name[128];
  char val[1024];
} entry;

entry entries[1000];


int NUM_ENTRIES;

/*
  Number of unique name/value entries found (by cgiinit() below).
*/


int VIA_SHELL;

/*
  1 if called via shell (not via HTTPD)
  0 if called via web (HTTPD)
*/


void cgiinit(void);

/*
  Initializes CGI - receives form data (via either "post" or "get" method).
*/


void getentry(char * buf, char *s);

/*
  Searches for an entry (name) and returns its value or an empty string.
  
  buf = address of string buffer to store value in.
  s = string (name) to search for.
*/


int getentryasint(char *s);

/*
  Searches for an entry (name) and returns its value or 0.
  
  return = value, converted from string to integer.
  s = string (name) to search for.
*/


int getentryyesno(char *s, int def);

/*
  Searches for an entry (name) and returns:
  
  0 if the value is "no" or "off"
  1 if the value is "yes" or "on"
  def if the value is none of those
  
  return = determined by entry's value (yes/on=1, no/off=0, else def)
  s = string (name) to search for.
*/


int dump_no_abort(char * filename);

/*
  Opens "filename" and displays it to stdout (browser).

  return = -1 on error, 0 on ok.
*/


void dump(char * filename);

/*
  Opens "filename" and displays it to stdout (browser).
  
  Displays error message to stdout and aborts on error.
*/


int goodemailaddress(char * addr);

/*
  Makes sure an email address is formatted correctly.
  
  Returns 1 on good address, 0 on badly formatted one.
*/
