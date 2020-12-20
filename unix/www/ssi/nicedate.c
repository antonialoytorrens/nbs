/*
  nicedate.c
  
  By Bill Kendrick
  New Breed Software
  February 16, 1996  (Written)
  April 15, 1996     (Fixed 00:00:00 - 00:59:59 AM/PM problem)
  
  Displays the last-modified date of a file.

  Call with: "{command} | nicedate {format}"

  Format variables:
    +dow+    - Day of week (Monday, Tuesday, etc.)
    +sdow+   - Short day of week (Mon, Tue, etc.)
    +month+  - Month (January, February, etc.
    +smonth+ - Short month (Jan, Feb, etc.)
    +nmonth+ - Number of month (01, 02, 12, etc.)
    +year+   - Year (1996, 1997, etc.)
    +syear+  - Short year (96, 97, etc.)
    +day+    - Day of month (1, 2, 3, etc.)
    +dayth+  - Day of month (1st, 2nd, 3rd, etc.)
    +24hour+ - Hour of day, 24-hour time (00, 01, 02, 12, 13, etc.)
    +hour+   - Hour of day (12, 01, 02, 12, 01, etc.
    +ampm+   - Time of day (am, pm)
    +min+    - Minute of hour (00, 01, 59, etc.)
    +sec+    - Second of minute (00, 01, 59,etc.)
    +zone+   - Time zone (PST, GMT, etc.)

  Examples: 
   1. The current date is:
      <!--#exec cmd="date | nicedate +dow+, +month+ +day+, +year+" --> !
   2. The current date is:
      <!--#exec cmd="date | nicedate +nmonth+ / +day+ / +syear+" --> ...

  Results:
   1. The current date is: Thursday, February 15, 1996 !
   2. The current date is: 02 / 15 / 96 ...

  Default:
    +dow+, +month+ +day+, +year+

*/

#include <stdio.h>
#include <string.h>

void replace(char * line, char * macro, char * value);


void main (int argc, char * argv[])
{
  char format[500], time[10], dow[20], sdow[5], month[20], smonth[5];
  char nmonth[5], year[5], syear[3], day[3], dayth[5], _24hour[3], hour[3];
  char ampm[3], min[3], sec[3], zone[4];
  int a;
  
  if (argc == 1)
    {
      strcpy(format, "+dow+, +month+ +day+, +year+");
    }
  else
    {
      strcpy(format, argv[1]);
      for (a = 2; a < argc; a++)
	{
	  strcat(format, " ");
	  strcat(format, argv[a]);
	}
    }
  
  scanf("%s%s%s%s%s%s", sdow, smonth, day, time, zone, year);
  
  strcpy(dow, sdow);
  if (strcmp(sdow, "Tue") == 0)
    strcat(dow, "s");
  else if (strcmp(sdow, "Wed") == 0)
    strcat(dow, "nes");
  else if (strcmp(sdow, "Thu") == 0)
    strcat(dow, "rs");
  else if (strcmp(sdow, "Sat") == 0)
    strcat(dow, "ur");
  strcat(dow, "day");
  
  if (strcmp(smonth, "Jan") == 0)
    {
      strcpy(month, "January");
      strcpy(nmonth, "01");
    }
  else if (strcmp(smonth, "Feb") == 0)
    {
      strcpy(month, "February");
      strcpy(nmonth, "02");
    }
  else if (strcmp(smonth, "Mar") == 0)
    {
      strcpy(month, "March");
      strcpy(nmonth, "03");
    }
  else if (strcmp(smonth, "Apr") == 0)
    {
      strcpy(month, "April");
      strcpy(nmonth, "04");
    }
  else if (strcmp(smonth, "May") == 0)
    {
      strcpy(month, "May");
      strcpy(nmonth, "05");
    }
  else if (strcmp(smonth, "Jun") == 0)
    {
      strcpy(month, "June");
      strcpy(nmonth, "06");
    }
  else if (strcmp(smonth, "Jul") == 0)
    {
      strcpy(month, "July");
      strcpy(nmonth, "07");
    }
  else if (strcmp(smonth, "Aug") == 0)
    {
      strcpy(month, "August");
      strcpy(nmonth, "08");
    }
  else if (strcmp(smonth, "Sep") == 0)
    {
      strcpy(month, "September");
      strcpy(nmonth, "09");
    }
  else if (strcmp(smonth, "Oct") == 0)
    {
      strcpy(month, "October");
      strcpy(nmonth, "10");
    }
  else if (strcmp(smonth, "Nov") == 0)
    {
      strcpy(month, "November");
      strcpy(nmonth, "11");
    }
  else if (strcmp(smonth, "Dec") == 0)
    {
      strcpy(month, "December");
      strcpy(nmonth, "12");
    }
  
  strcpy(dayth, day);
  if (strcmp(day, "11") == 0 || strcmp(day, "12") == 0 ||
      strcmp(day, "13") == 0 || day[1] >= '4')
    strcat(dayth, "th");
  else if (day[1] == '1')
    strcat(dayth, "st");
  else if (day[1] == '2')
    strcat(dayth, "nd");
  else if (day[1] == '3')
    strcat(dayth, "rd");
  
  _24hour[0] = time[0];
  _24hour[1] = time[1];
  _24hour[2] = '\0';
  
  strcpy(ampm, "am");
  a = atoi(_24hour);
  if (a > 11)
    strcpy(ampm, "pm");
  if (a == 0)
    a = 12;
  if (a > 12)
    a = a - 12;
  
  hour[0] = (a/10) + 48;
  hour[1] = a - (a/10) * 10 + 48;
  hour[2] = '\0';
  
  min[0] = time[3];
  min[1] = time[4];
  min[2] = '\0';
  
  sec[0] = time[6];
  sec[1] = time[7];
  sec[2] = '\0';
  
  syear[0] = year[2];
  syear[1] = year[3];
  syear[2] = '\0';
  
  replace(format, "+dow+", dow);
  replace(format, "+sdow+", sdow);
  replace(format, "+month+", month);
  replace(format, "+smonth+", smonth);
  replace(format, "+nmonth+", nmonth);
  replace(format, "+year+", year);
  replace(format, "+syear+", syear);
  replace(format, "+day+", day);
  replace(format, "+dayth+", dayth);
  replace(format, "+24hour+", _24hour);
  replace(format, "+hour+", hour);
  replace(format, "+ampm+", ampm);
  replace(format, "+min+", min);
  replace(format, "+sec+", sec);
  replace(format, "+zone+", zone);
  
  printf("%s", format);
}

void replace(char * line, char * macro, char * value)
{
  char startline[500], endline[500];
  
  do
    {
      if (strstr(line, macro) != NULL)
	{
	  strcpy(startline, line);
	  strcpy(strstr(startline, macro), "\0");
	  strcpy(endline, strstr(line, macro) + strlen(macro));
	  sprintf(line, "%s%s%s", startline, value, endline);
	}
    }
  while (strstr(line, macro) != NULL);
}
