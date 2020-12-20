/*
  random.c
  
  By Bill Kendrick
  New Breed Software
  February 14, 1996, May 30, 1996
  
  Randomly picks one line out of a file and displays it.

  Based on the "random" directive from my "counter.cgi" 'CGI-Side-Include'
  program.

  Call with: "random file"

  Example:
    Random quote:  <!--#exec cmd="random quotes.html" -->

*/

#include <stdio.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>

int rnd(int n)
{
  int r;
  struct timeval t;
  
  gettimeofday(&t, NULL);
  r=t.tv_sec + t.tv_usec;
  srand(r);
  
  r=(t.tv_usec+rand())%n;

  return(r);
}

int main(int argc, char * argv[])
{
  int many, pick;
  FILE * fi;
  char the_lines[500][1024], line[1024];
  
  if (argc != 2)
    {
      printf("<!-- Usage: random file -->\n");
      exit(0);
    }
  
  fi = fopen(argv[1], "r");
  if (fi == NULL)
    {
      printf("<!-- random: Can't open \"%s\" file! -->\n", argv[1]);
      exit(0);
    }
  
  many = 0;
  do
    {
      fgets(line, 1024, fi);
      if (!feof(fi))
	{
	  strcpy(the_lines[many], line);
	  many = many + 1;
	}
    }
  while(!feof(fi));
  fclose(fi);

  pick = rnd(many);;
  printf("<!-- Picked: %d / %d -->\n", pick, many);

  printf("%s", the_lines[pick]);
  
  exit(0);
}
