/*
  ifcount.c
  
  By Bill Kendrick
  New Breed Software
  February 14, 1996
  
  Check's the count of a page (from a file), and if that count matches one
  of the numbers in a data file, then an HTML file described in the data file
  is displayed.

  Based on the "ifcount" directive from my "counter.cgi" 'CGI-Side-Include'
  program.

  Call with: "ifcount count-file data-file"

  Example:
    <!--#exec cmd="ifcount counter.dat ifcount.dat" -->
 
  Example data file:
    1
    congratulate-1st.html
    100
    congratulate-100th.html
    200
    congratulate-200th.html

*/

#include <stdio.h>

int main(int argc, char * argv[])
{
  FILE * fi;
  int hits, needed, c;
  char file[100], the_file[100];
  
  if (argc != 3)
    {
      printf("<!-- Usage: ifcount count-file data-file -->\n");
      exit(0);
    }
  
  fi = fopen(argv[1], "r");
  if (fi == NULL)
    {
      printf("<!-- ifcount: Can't open \"%s\" counter data file! -->\n",
	     argv[1]);
      exit(0);
    }
  
  fscanf(fi, "%d", &hits);
  fclose(fi);

  fi = fopen(argv[2], "r");
  if (fi == NULL)
    {
      printf("<!-- ifcount: Can't open \"%s\" ifcount data file! -->\n",
	     argv[2]);
      exit(0);
    }
  
  strcpy(the_file, "");
  
  do
    {
      fscanf(fi, "%d%s", &needed, file);
      if (needed == hits)
	strcpy(the_file, file);
    }
  while (!feof(fi));
  fclose(fi);

  if (strcmp(the_file, "")==0)
    {
      printf("<!-- ifcount: no match -->\n");
      exit(0);
    }

  fi = fopen(the_file, "r");
  if (fi == NULL)
    {
      printf("<!-- ifcount: Can't open \"%s\" HTML file! -->\n", the_file);
      exit(0);
    }
  
  do
    {
      c=fgetc(fi);
      if (c!=EOF)
	printf("%c", c);
    }
  while (c!=EOF);
  fclose(fi);
  
  /* printf("<!--#include \"%s\"-->\n", the_file); */
  
  exit(0);
}
