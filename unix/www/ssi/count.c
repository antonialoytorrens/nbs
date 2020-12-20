/*
  count.c
  
  By Bill Kendrick
  New Breed Software
  February 14, 1996 / September 30, 1996 / December 18, 1996
  
  Update's a page's hit counter and displays the value (unless "QUIET" is
  used as the image prefix).  Displays ALTed images (unless "NONE" is used
  as the image prefix, or it's missing altogether).

  Based on the "count" and "nocount" directives from my "counter.cgi"
  'CGI-Side-Include' program.

  Call with: "count count-file [image prefix] {width height}"

  Examples:
    This page has been visited:
      <!--#exec cmd="count counter.dat counter-gifs/blue" -->  times.

   This page has been visited:
     <!--#exec cmd="count counter.dat counter-gifs/blue 16 16" -->  times.

    This page has been visited:
      <!--#exec cmd="count counter.dat NONE" -->  times.

    This page has been visited:
      <!--#exec cmd="count counter.dat" -->  times.
      
    <!--#exec cmd="count counter.dat QUIET" -->

*/


/* -- If you're running on a System V Unix system, uncomment this: -- */

#define SYSV


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char * argv[])
{
  FILE * fi;
  int hits, power, digit, c, width, height;
  char imageprefix[100];
  int fd;
  
  
  if ((argc > 3 && argc != 5) || argc < 2)
    {
      printf("<!-- Usage: count count-file {image-prefix} -->\n");
      exit(0);
    }
  
  if (argc == 3 || argc == 5)
    strcpy(imageprefix, argv[2]);
  else
    strcpy(imageprefix, "");
  
  if (argc == 5)
    {
      width = atoi(argv[3]);
      height = atoi(argv[4]);
    }
  else
    {
      width = -1;
      height = -1;
    }
  
  fd = open(argv[1], O_RDONLY);
#ifdef SYSV
  lockf(fd, F_LOCK, 0);
#else
  flock(fd, LOCK_EX);
#endif
  
  fi = fdopen(fd, "r");
  if (fi == NULL)
    {
      printf("<!-- count: Can't open \"%s\" counter data file! Check permissions-->\n",
	     argv[1]);
      exit(0);
    }
  
  fscanf(fi, "%d", &hits);
  fclose(fi);

  hits = hits + 1;
  
  fd = open(argv[1], O_WRONLY);
#ifdef SYSV
  lockf(fd, F_LOCK, 0);
#else
  flock(fd, LOCK_EX);
#endif
  
  fi = fdopen(fd, "w");
  if (fi == NULL)
    {
      printf("<!-- ifcount: Can't open \"%s\" counter data file", argv[1]);
      printf(" for write! -->\n");
      exit(0);
    }
  
  fprintf(fi, "%d\n", hits);
  fclose(fi);

  if (strcmp(imageprefix,"")==0 || strcmp(imageprefix,"NONE")==0)
    printf("%d", hits);
  else if (strcmp(imageprefix,"QUIET")!=0)
    {
      power=10000;
      for (digit = 0; digit <= 4; digit++)
	{
	  c = hits / power;
	  hits = hits - (c * power);
	  printf("<img src=\"%s%c.gif\" alt=\"%c\"", imageprefix,
		 c + 48, c + 48);
	  if (width > 0 && height > 0)
	    printf(" width=%d height=%d", width, height);
	  printf(">");
	  power = power / 10;
	}
    }
  
  exit(0);
}
