/*
  changedate.c
  
  By Bill Kendrick
  New Breed Software
  February 14, 1996 - April 28, 1996
  
  Displays the last-modified date of a file.

  Call with: "changedate file"

  Example: 
    This file last changed: <!--#exec cmd="changedate index.html" -->
 
  You can emulate a display of the last-access date of a file like this:

  Example:
    This file last accessed: <!--#exec cmd="changedate index.access" -->
    <!--#exec cmd="/bin/touch index.access" -->

  Where "index.access" is fully read/writable.

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char * argv[])
{
  FILE * fi;
  char line[100], dow[10], mon[10], day[10], time[10], year[10];
  struct stat buf;
  
  if (argc != 2)
    {
      printf("<!-- Usage: changedate file -->\n");
      exit(0);
    }

  stat(argv[1],&buf);
  
  sscanf(ctime(&buf.st_mtime), "%s%s%s%s%s", dow, mon, day, time, year);

  printf("%s %s %s %s - %s ", dow, mon, day, time, year);
}
