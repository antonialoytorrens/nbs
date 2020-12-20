/*
  hitstats.c
  
  By Bill Kendrick
  New Breed Software
  April 28, 1996 - October 15, 1996
  
  Shows status of hit counts

  Example:
    This week's hits chart:<br>
    <!--#exec cmd="./hitstats index.hits 1 starfile.txt"-->

  Where "index.hits" is a data file as created by the "hit" SSI.

  "1" is how many hits will be represented by one "*".
  (  1 = 10 hits -> 10 stars,
     2 = 10 hits -> 5 stars,
     5 = 10 hits -> 2 stars,
    10 = 10 hits -> 1 star, etc. )

  You may omit the "hits/stars" value and the default will be 5 hits per
  star.

  "starfile.txt" a file which contains one line of text to be shown for each
  unit.

  If no file is provided, or there is a problem, the string is "*".
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

void show(int n, char * day, char * txt);

char * daystr[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
int hits_per_star;

int main(int argc, char * argv[])
{
  int i, today, tomorrow;
  int day[7];
  char fname[512], str[100], todaystr[100], txt[200];
  FILE * fi;
  time_t clock;
  
  time(&clock);
  sscanf(ctime(&clock), "%s", todaystr);
  
  strcpy(txt, "*");
  
  if (argc == 1)
    {
      strcpy(fname, "index.hits");
      hits_per_star = 5;
    }
  else if (argc == 2 || argc == 3 || argc == 4)
    {
      strcpy(fname, argv[1]);
      if (argc == 3 || argc == 4)
	{
	  hits_per_star = atoi(argv[2]);
	  if (hits_per_star == 0)
	    hits_per_star = 5;
	  
	  if (argc == 4)
	    {
	      fi = fopen(argv[3], "r");
	      if (fi != NULL)
		{
		  fgets(txt, 200, fi);
		  txt[strlen(txt) - 1] = '\0';
		  fclose(fi);
		}
	    }
	}
      else
	hits_per_star = 5;
    }
  else
    printf("Usage: %s hits-file {hits/stars}\n", argv[0]);
  
  fi = fopen(fname, "r");
  if (fi == NULL)
    printf("Error opening %s.\n", fname);
  
  today = 0;
  for (i = 0; i < 7; i++)
    {
      day[i] = 0;
      if (strcmp(daystr[i], todaystr) == 0)
	today = i;
    }
  
  do
    {
      fgets(str, 100, fi);
      
      if (!feof(fi))
	{
	  str[strlen(str) - 1] = '\0';
	  for (i = 0; i < 7; i++)
	    if (strcmp(str, daystr[i]) == 0)
	      day[i]++;
	  
	  fgets(str, 100, fi);
	}
    }
  while (!feof(fi));
  fclose(fi);
  
  printf("<h1 align=center>Hit Statistics Up To Today (%s)</h1>\n", todaystr);
  
  tomorrow = today + 1;
  if (tomorrow > 6)
    tomorrow = 0;
  
  for (i = tomorrow; i < 7; i++)
    show(day[i], daystr[i], txt);
  
  if (tomorrow != 0)
    for (i = 0; i <= today; i++)
      show(day[i], daystr[i], txt);

}

void show(int n, char * day, char * txt)
{
  char num[10];
  int i;
  
  sprintf(num, "%-4d", n);
  for (i = 0; i < strlen(num); i++)
    if (num[i] == ' ')
      num[i] = '.';
  
  printf("<code>%s: (%s) ", day, num);
  for (i = 0; i < n; i = i + hits_per_star)
    printf(txt);
  printf("</code><br>\n");
}
