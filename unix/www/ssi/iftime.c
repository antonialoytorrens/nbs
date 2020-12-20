/*
  iftime.c
  
  By Bill Kendrick
  New Breed Software
  February 14, 1996
  
  If the current time is later than or the same as one of the times in a data
  file, then the line corresponding to that time in the data file is displayed.
  If the time is earlier than any of the times in the data file, then the
  line corresponding to the latest time is displayed.

  Based on the "iftime" directive from my "counter.cgi" 'CGI-Side-Include'
  program.

  Call with: "iftime data-file"

  Example: 
    Right now, it's: <!--#exec cmd="iftime times.dat" -->  .

  Example data file:
    06:30
    Daytime
    18:30
    Nighttime

*/

#include <stdio.h>

int main(int argc, char * argv[])
{
  FILE * fi;
  char line[500], show_line[500], latest_line[500];
  int a, minutes, check_minutes, latest_minutes;
  
  if (argc != 2)
    {
      printf("<!-- Usage: iftime data-file -->\n");
      exit(0);
    }
  
  fi = popen("/bin/date", "r");
  if (fi == NULL)
    {
      printf("<!-- ifcount: Can't open \"/bin/date\" program! -->\n");
      exit(0);
    }
  
  fgets(line, 40, fi);
  pclose(fi);
  
  minutes = 0;
  
  for (a=0; a<5; a++)
    if (line[a+11]!=':')
      {
	minutes = minutes * 10;
	minutes = minutes + (line[a+11]-48);
      }
  
  fi = fopen(argv[1], "r");
  if (fi == NULL)
    {
      printf("<!-- iftime: Can't open \"%s\" iftime data file! -->\n",
	     argv[1]);
      exit(0);
    }
  
  latest_minutes = 0;
  strcpy(show_line, "");
  strcpy(latest_line, "");

  do
    {
      fgets(line, 40, fi);
      
      check_minutes = 0;
      for (a=0; a<5; a++)
	if (line[a]!=':')
	  {
	    check_minutes = check_minutes * 10;
	    check_minutes = check_minutes + (line[a]-48);
	  }
      
      fgets(line, 500, fi);
      
      if (minutes >= check_minutes)
	strcpy(show_line, line);
      
      if (check_minutes >= latest_minutes)
	{
	  latest_minutes = check_minutes;
	  strcpy(latest_line, line);
	}
    }
  while (!feof(fi));
  fclose(fi);
  
  if (strcmp(show_line, "")==0)
    strcpy(show_line, latest_line);
  
  printf("%s", show_line);
  
  exit(0);
}
