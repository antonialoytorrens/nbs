/*
  hit.c
  
  By Bill Kendrick
  New Breed Software
  April 28, 1996 - April 28, 1996
  Minor, rare bug fix - Dec. 15, 1996
  
  Add a hit to a hits-file.  Use in conjunction with "hitstats" to view a
  weekly chart of recent hits.

  Example:
    <!--#exec cmd="./hit index.hits"-->

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char * argv[])
{
  int i, j, ok;
  int day[7];
  char fname[512], str[100], todaystr[100], cur_monthstr[100],
    cur_daystr[100], cur_monthday[100], monthday[100];
  char days[10000][10], curs[10000][10];
  FILE * fi;
  time_t clock;
  
  time(&clock);
  sscanf(ctime(&clock), "%s %s %s", todaystr, cur_monthstr, cur_daystr);
  
  sprintf(cur_monthday, "%s %s", cur_monthstr, cur_daystr);
  
  if (argc == 1)
    strcpy(fname, "index.hits");
  else if (argc == 2)
    strcpy(fname, argv[1]);
  else
    printf("Usage: %s hits-file\n", argv[0]);
  
  fi = fopen(fname, "r");
  if (fi == NULL)
    {
      printf("Error opening %s.\n", fname);
      exit(0);
    }
  
  i = 0;
  do
    {
      fgets(str, 100, fi);
      
      if (!feof(fi))
	{
	  ok = 1;
	  str[strlen(str) - 1] = '\0';
	  
	  fgets(monthday, 100, fi);
	  monthday[strlen(monthday) - 1] = '\0';
	  
	  if (strcmp(str, todaystr) == 0)
	    if (strcmp(cur_monthday, monthday) != 0)
	      ok = 0;
	  
	  if (ok == 1)
	    {
	      strcpy(days[i], str);
	      strcpy(curs[i], monthday);
	      i++;
	    }
	}
    }
  while (!feof(fi));
  fclose(fi);
  
  strcpy(days[i], todaystr);
  strcpy(curs[i], cur_monthday);
  i++;
  
  fi = fopen(fname, "w");
  if (fi == NULL)
    {
      printf("Error opening %s for write.\n", fname);
      exit(0);
    }
  
  for (j = 0; j < i; j++)
    fprintf(fi, "%s\n%s\n", days[j], curs[j]);
  
  fclose(fi);
}
