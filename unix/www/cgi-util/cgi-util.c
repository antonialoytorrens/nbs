/*
  cgi-util.c
  
  created by Bill Kendrick
  kendrick@zippy.sonoma.edu
  http://zippy.sonoma.edu/~kendrick/
  
  based on "post-query.c" and "query.c" by NCSA
  
  New Breed Software
  
  April 6, 1996 - January 28, 1997
*/


#include <stdio.h>

#ifndef NO_STDLIB_H
#include <stdlib.h>
#else
char *getenv();
#endif

#include <string.h>
#include "cgi-util.h"

#define LF 10
#define CR 13


/* THE FOLLOWING ROUTINES ARE (MORE OR LESS) DIRECTLY FROM NCSA'S
   "util.c", "post-query.c" AND "query.c" SOURCE CODE: */


void getword(char *word, char *line, char stop)
{
  int x, y;
  
  for(x = 0; ((line[x]) && (line[x] != stop)); x++)
    word[x] = line[x];
  
  word[x] = '\0';
  if (line[x])
    ++x;
  y=0;
  
  while (line[y++] = line[x++]);
}

char *makeword(char *line, char stop)
{
  int x,y;
  char *word = (char *) malloc(sizeof(char) * (strlen(line) + 1));
  
  for(x = 0; ((line[x]) && (line[x] != stop)); x++)
    word[x] = line[x];

  word[x] = '\0';
  if (line[x])
    ++x;
  y=0;
  
  while (line[y++] = line[x++]);
  
  return word;
}

char *fmakeword(FILE *f, char stop, int *cl)
{
  int wsize;
  char *word;
  int ll;
  
  wsize = 102400;
  ll=0;
  word = (char *) malloc(sizeof(char) * (wsize + 1));
  
  while (1)
    {
      word[ll] = (char)fgetc(f);
      if (ll==wsize)
	{
	  word[ll+1] = '\0';
	  wsize += 102400;
	  word = (char *)realloc(word,sizeof(char)*(wsize+1));
        }
      --(*cl);
      if ((word[ll] == stop) || (feof(f)) || (!(*cl)))
	{
	  if (word[ll] != stop)
	    ll++;
	  word[ll] = '\0';
	  return word;
        }
      ++ll;
    }
}

char x2c(char *what)
{
  register char digit;
  
  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return (digit);
}

void unescape_url(char *url)
{
  register int x,y;
  
  for (x=0, y=0; url[y]; ++x, ++y)
    {
      if ((url[x] = url[y]) == '%')
	{
	  url[x] = x2c(&url[y+1]);
	  y+=2;
        }
    }
  url[x] = '\0';
}

void plustospace(char *str)
{
  register int x;
  
  for (x=0; str[x]; x++)
    if (str[x] == '+')
      str[x] = ' ';
}

int rind(char *s, char c)
{
  register int x;
  
  for (x = strlen(s) - 1; x != -1; x--)
    if(s[x] == c) return x;
  return -1;
}

int getline(char *s, int n, FILE *f)
{
  register int i=0;
  
  while(1)
    {
      s[i] = (char)fgetc(f);
      
      if (s[i] == CR)
	s[i] = fgetc(f);
      
      if ((s[i] == 0x4) || (s[i] == LF) || (i == (n-1)))
	{
	  s[i] = '\0';
	  return (feof(f) ? 1 : 0);
        }
      ++i;
    }
}

void send_fd(FILE *f, FILE *fd)
{
  int num_chars=0;
  char c;
  
  while (1)
    {
      c = fgetc(f);
      if(feof(f))
	return;
      fputc(c,fd);
    }
}

int ind(char *s, char c)
{
  register int x;
  
  for (x=0; s[x]; x++)
    if(s[x] == c)
      return x;
  
  return -1;
}

void escape_shell_cmd(char *cmd)
{
  register int x,y,l;
  
  l=strlen(cmd);
  for (x=0; cmd[x]; x++)
    {
      if (ind("&;`'\"|*?~<>^()[]{}$\\",cmd[x]) != -1)
	{
	  for(y = l + 1; y > x; y--)
	    cmd[y] = cmd[y-1];
	  l++;
	  cmd[x] = '\\';
	  x++;
        }
    }
}


/* CALL cgiinit() AT THE BEGINNING OF YOUR PROGRAM TO COLLECT THE FORM
   OR URL-ENCODED VALUES SENT TO THE CGI.  IT TRANSPARENTLY CAN TELL
   IF YOUR PROGRAM WAS ACCESSED VIA "POST" OR "GET" METHODS, OR EVEN
   VIA THE SHELL! */

void cgiinit(void)
{
  register int x;
  int cl;
  char * qcl;
  
  
  if (getenv("REQUEST_METHOD") == NULL)
    {
      VIA_SHELL = 1;
      NUM_ENTRIES = 0;
    }
  else
    {
      VIA_SHELL = 0;

      if (strcmp(getenv("REQUEST_METHOD"),"POST") == 0)
	{
	  if(strcmp(getenv("CONTENT_TYPE"),
		    "application/x-www-form-urlencoded"))
	    {
	      printf("Content-type: text/html\n\n");
	      printf("<h1>Incorrect Type</h1>\n");
	      printf("Type must be 'application/x-www-form-urlencoded'.\n");
	      
	      exit(0);
	    }
	  
	  cl = atoi(getenv("CONTENT_LENGTH"));
	  
	  for (x = 0; cl && (!feof(stdin)); x++)
	    {
	      NUM_ENTRIES = x;
	      strcpy(entries[x].val, fmakeword(stdin,'&',&cl));
	      plustospace(entries[x].val);
	      unescape_url(entries[x].val);
	      strcpy(entries[x].name, makeword(entries[x].val,'='));
	    }
	}
      else if (strcmp(getenv("REQUEST_METHOD"),"GET") == 0)
	{
	  qcl = getenv("QUERY_STRING");
	  if (qcl == NULL)
	    exit(0);
	  
	  for (x = 0; qcl[0] != '\0'; x++)
	    {
	      NUM_ENTRIES = x;
	      getword(entries[x].val, qcl, '&');
	      plustospace(entries[x].val);
	      unescape_url(entries[x].val);
	      getword(entries[x].name, entries[x].val, '=');
	    }
	}
      else
	{
	  printf("Content-type: text/html\n\n");
	  printf("<h1>Unknown method</h1>\n");
	  printf("The form was submitted with an unknown method!\n");
	  
	  exit(0);
	}
    }
}


/* THIS SEARCHES THE LIST OF FORM ENTRIES COLLECTED BY "cgiinit" FOR A
   PARTICULARLY NAMED FIELD ("s").  IT RETURNS THE STRING SENT IN
   (through "buf").  IF THE FIELD IS NOT FOUND, AN EMPTY STRING IS RETURNED. */

void getentry(char * buf, char *s)
{
  int x;
  
  strcpy(buf,"");
  
  if (VIA_SHELL == 0)
    {
      for (x = 0; x <= NUM_ENTRIES; x++)
	if (strcmp(entries[x].name,s) == 0)
	  strcpy(buf,entries[x].val);
    }
  else
    {
      printf("CGI-UTIL: \"%s\" ? ", s);
      fgets(buf, 512, stdin);
      buf[strlen(buf) - 1] = '\0';
    }
}


/* THIS IS THE SAME AS "getentry", BUT IT RETURNS THE INTEGER VALUE
   (using atoi) OF THE STRING FOUND. */

int getentryasint(char *s)
{
  char buf[512];
  int x, v;
  
  v = 0;
  
  if (VIA_SHELL == 0)
    {
      for (x = 0; x <= NUM_ENTRIES; x++)
	if (strcmp(entries[x].name,s) == 0)
	  v = atoi(entries[x].val);
    }
  else
    {
      printf("CGI-UTIL: \"%s\" ? ", s);
      fgets(buf, 512, stdin);
      buf[strlen(buf) - 1] = '\0';
      v = atoi(buf);
    }
  
  return(v);
}


/* THIS IS THE SAME AS "getentry", EXCEPT IT RETURNS "1" IF THE VALUE FOUND
   IS "on" or "yes" (CASE-INSENSITIVE), "0" IF THE VALUE FOUND IS "off" or
   "no" (CASE-INSENSITIVE), OR "def" OTHERWISE. */

int getentryyesno(char *s, int def)
{
  char temp[512];
  int x, v;
 
  v = def;
  
  getentry(temp, s);
  
  if (strcasecmp(temp, "yes") == 0 ||
      strcasecmp(temp, "on") == 0)
    v = 1;
  else if (strcasecmp(temp, "no") == 0 ||
	   strcasecmp(temp, "off") == 0)
    v = 0;
  
  return(v);
}


/* THIS OPENS A FILE AND DUMPS IT DIRECTLY TO "stdout" (IN A CGI'S CASE,
   THE BROWSER).  IF THE FILE CAN'T BE OPENED, "-1" IS RETURNED, ELSE "0". */

int dump_no_abort(char * filename)
{
  FILE * fi;
  int c;
  
  fi = fopen(filename, "r");
  if (fi == NULL)
    return(-1);
  else
    {
      do
	{
	  c = fgetc(fi);
	  if (c != EOF)
	    fputc(c, stdout);
	}
      while (c != EOF);
      fclose(fi);
      
      return(0);
    }
}


/* THIS IS THE SAME AS "dump_no_abort", EXCEPT IF THE FILE CANNOT BE OPENED,
   A MESSAGE IS DISPLAYED AND THE PROGRAM IS ABORTED. */

void dump(char * filename)
{
  if (dump_no_abort(filename) == -1)
    {
      printf("Can't open %s\n", filename);
      exit(0);
    }
}


/* THIS DISPLAYS AN EXTREMELY SIMPLY "ERROR" MESSAGE IN HTML */

void error(char * reason)
{
  printf("<h1>Error</h1>\n");
  printf("%s\n", reason);
  
  exit(0);
}


/* THIS CHECKS AN E-MAIL ADDRESS TO SEE IF IT'S FORMATTED CORRECTLY */
/* Returns "1" on good, "0" on bad. */
/* E-mail should be in "name@site.ext" format.  You'd be surprised how
   many people DON'T know this! <sigh!> */

int goodemailaddress(char * addr)
{
  if (strchr(addr, '@') == NULL)
    return 0;
  
  if (addr[strlen(addr - 1)] == '@' ||
      addr[strlen(addr - 1)] == '.')
    return 0;
  
  if (strchr(strchr(addr, '@'), '.') == NULL)
    return 0;
  
  if (strchr(addr, ' ') != NULL)
    return 0;
  
  if (addr[0] == '@' || addr[0] == '.')
    return 0;
  
  return 1;
}
