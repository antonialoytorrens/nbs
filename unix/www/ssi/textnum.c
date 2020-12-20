/*
  textnum.c

  By Bill Kendrick
  New Breed Software
  March 21, 1996 - March 30, 1996 / Type fixed April 29, 1997

  S McCoy - added 'and' 22 May 1997
  Clift <sirecho@webecho.com> - Pointed out "fourty" bug - 10 Apr 2000


  Displays an integer number in `ordninal' english.
  
  
  Call with: "changedate [-st] [list of numbers]"
  
  
  Options:
    -st - Display number(s) ordinally (one becomes first, twenty one becomes
          twenty first, etc.)


  Example:
    There are <!--#exec cmd="textnum 101" --> Dalmations
  Result:
    There are one hundred and one Dalmations

  Example:
    This page has been visted
    <!--#exec cmd="count index.dat NONE | textnum" --> times!
  Result:
    This page has been visited one hundred and twenty three times!

  Example:
    You are the <!--#exec cmd="count index.dat NONE | textnum -st" --> visitor!
  Result:
    You are the one hundred and twenty third visitor!

*/

#include <stdio.h>
#include <string.h>

char * itoenglish(int n, int st);
void doit(int n, int st);


int main(int argc, char * argv[])
{
  int a, n, once, st;
  char str[11];
  
  st = 0;
  if (argc >= 2)
    if (strcmp(argv[1], "-st") == 0)
      {
	st = 1;
	for (a = 1; a < argc - 1; a++)
	  argv[a] = argv[a + 1];
	argc = argc - 1;
      }
  if (argc == 1)
    {
      once = 0;
      do
	{
	  fgets(str, 10, stdin);
	  if (!feof(stdin) || once == 0)
	    {
	      once = 1;
	      n = atoi(str);
	      doit(n, st);
	    }
	}
      while (!feof(stdin));
    }
  else if (argc >= 1)
    {
      for (a = 1; a < argc; a++)
	{
	  n = atoi(argv[a]);
	  doit(n, st);
	}
    }
}

void doit(int n, int st)
{
  int z, showed;
  char str[100], out[100];
  
  if (n != 0)
    {
      strcpy(out, "");
      showed = 0;
      
      if ((n / 1000) != 0)
	{
	  z = n / 1000;
	  n = n - z * 1000;
	  sprintf(str, "%s thousand ", itoenglish(z, 0));
	  strcat(out, str);
	  
	  showed = 1000;
	}
      
      if ((n / 100) != 0)
	{
	  z = n / 100;
	  n = n - z * 100;
	  sprintf(str, "%s hundred ", itoenglish(z, 0));
	  strcat(out, str);
	  
	  showed = 100;
	}
      
      if (n != 0)
	{
	  z = n;
	  if (showed == 100) strcat(out, "and");
	  sprintf(str, "%s", itoenglish(z, st));
	  strcat(out, str);
	  
	  showed = 10;
	}
      
      if (st == 1 && showed > 10)
	strcat(out, "TH");
      
      if (strstr(out, " TH") != NULL)
	strcpy(strstr(out, " TH"), "TH");
      
      if (strstr(out, "tyTH") != NULL)
	strcpy(strstr(out, "tyTH"), "tieTH");
      
      if (strstr(out, "TH") != NULL)
	strcpy(strstr(out, "TH"), "th");
      
      printf("%s", out);
    }
  else
    {
      if (st == 0)
	printf("zero");
      else
	printf("zeroeth");
    }
  
  printf("\n");
}

char * itoenglish(int n, int st)
{
  char str[100];
  char th[2][3];
  
  strcpy(th[0], "");
  strcpy(th[1], "TH");
  
  if (n == 0)
    strcpy(str, th[st]);
  else if (n == 1)
    {
      if (st == 0)
	strcpy(str, " one");
      else
	strcpy(str, " first");
    }
  else if (n == 2)
    {
      if (st == 0)
	strcpy(str, " two");
      else
	strcpy(str, " second");
    }
  else if (n == 3)
    {
      if (st == 0)
	strcpy(str, " three");
      else
	strcpy(str, " third");
    }
  else if (n == 4)
    sprintf(str, " four%s", th[st]);
  else if (n == 5)
    {
      if (st == 0)
	strcpy(str, " five");
      else
	strcpy(str, " fifth");
    }
  else if (n == 6)
    sprintf(str, " six%s", th[st]);
  else if (n == 7)
    sprintf(str, " seven%s", th[st]);
  else if (n == 8)
    sprintf(str, " eight%s", th[st]);
  else if (n == 9)
    sprintf(str, " nine%s", th[st]);
  else if (n == 10)
    sprintf(str, " ten%s", th[st]);
  else if (n == 11)
    sprintf(str, " eleven%s", th[st]);
  else if (n == 12)
    sprintf(str, " twelve%s", th[st]);
  else if (n == 13)
    sprintf(str, " thirteen%s", th[st]);
  else if (n == 14)
    sprintf(str, " fourteen%s", th[st]);
  else if (n == 15)
    sprintf(str, " fifteen%s", th[st]);
  else if (n >= 16 && n <= 17)
    sprintf(str, " %steen%s", itoenglish(n - 10, 0), th[st]);
  else if (n == 18)
    sprintf(str, " eighteen%s", th[st]);
  else if (n == 19)
    sprintf(str, " nineteen%s", th[st]);
  else if (n >= 20 && n <= 29)
    sprintf(str, " twenty%s", itoenglish(n - 20, st));
  else if (n >= 30 && n <= 39)
    sprintf(str, " thirty%s", itoenglish(n - 30, st));
  else if (n >= 40 && n <= 49)
    sprintf(str, " forty%s", itoenglish(n - 40, st));
  else if (n >= 50 && n <= 59)
    sprintf(str, " fifty%s", itoenglish(n - 50, st));
  else if (n >= 60 && n <= 79)
    sprintf(str, " %sty%s", itoenglish(n / 10, 0),
	    itoenglish(n - (n / 10) * 10, st));
  else if (n >= 80 && n <= 89)
    sprintf(str, " eighty%s", itoenglish(n - 80, st));
  else if (n >= 90 && n <= 99)
    sprintf(str, " %sty%s", itoenglish(n / 10, 0),
	    itoenglish(n - (n / 10) * 10, st));
  else
    strcpy(str, "");
  
  if (strstr(str, "tTH") != NULL)
    strcpy(strstr(str, "tTH"), "TH");
  
  return(strdup(str));
}
