/*
  roman.c

  By Steven McCoy
  mcacsmc@afs.mcc.ac.uk
  May 22 1997

  Displays an integer number in roman.

  Call with: "roman [list of numbers]"

  Example:
    There are <!--#exec cmd="./roman 10" --> things
  Result:
    There are X things

  Example:
    This page has been visted
    <!--#exec cmd="./count index.dat NONE | ./roman" --> times!
  Result:
    This page has been visited XXIII times!

*/

#include <stdio.h>
#include <string.h>

void doit(int n);


int main(int argc, char * argv[])
{
  int a, n, once;
  char str[11];
  
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
	      doit(n);
	    }
	}
      while (!feof(stdin));
    }
  else if (argc >= 1)
    {
      for (a = 1; a < argc; a++)
	{
	  n = atoi(argv[a]);
	  doit(n);
	}
    }
}

int arabic[13] = {1000,900,500,400,100,90,50,40,10,9,5,4,1};
char *roman[13] = {"M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I"};

void doit(int n)
{
  int i;
  char out[100];
  
  if (n != 0)
    {
      strcpy(out, "");
      
      i = 0;
      while (n) {
	while (n >= arabic[i]) {
	  n -= arabic[i];
	  strcat(out, roman[i]);
	}
	i++;
      }
	
      printf(out);
    }
  else 
     printf("none");

  printf("\n");
}
      
