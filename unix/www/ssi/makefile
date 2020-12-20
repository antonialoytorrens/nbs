# Makefile for Bill Kendrick's Server Side Include programs

# By Bill Kendrick
# kendrick@zippy.sonoma.edu
# New Breed Software
# http://zippy.sonoma.edu/kendrick/nbs/
# February 14, 1996 - June 26, 1997


usage:	version
	@echo "Usage: make {command}"
	@echo "       make all"
	@echo "       make version"
	@echo "       make usage    (this display)"
	@echo "       make help"
	@echo "       make credits"
	@echo " "

help:	usage
	@echo "Include these Server Side Includes (SSI's) in your HTML"
	@echo "document with this syntax (may vary depending on server)"
	@echo "  <!--#exec cmd=\"{command} {paramters}\"-->"
	@echo " "

credits:	version
	@echo "By Bill Kendrick"
	@echo "New Breed Software, 1996/1997"
	@echo " "
	@echo "kendrick@zippy.sonoma.edu"
	@echo "http://zippy.sonoma.edu/kendrick/nbs/"
	@echo " "
	@echo "Some routines by Steven McCoy"
	@echo "mcacsmc@afs.mcc.ac.uk"
	@echo " "
	@echo "Support page:"
	@echo "http://zippy.sonoma.edu/kendrick/nbs/unix/www/ssi/"
	@echo " "

all:	version ifcount count random iftime changedate nicedate textnum \
	hitstats hit roman

version:
	@echo " "
	@echo "Makefile for Bill Kendrick's Server Side Include programs"
	@echo "Last change: June 26, 1996"
	@echo " "

ifcount:	ifcount.c
	gcc ifcount.c -o ifcount
	strip ifcount
	chmod 755 ifcount

count:	count.c
	@echo "Edit the line containing \"#define SYSV\" in count.c if" \
		"an error occurs"
	gcc count.c -o count
	strip count
	chmod 755 count

# -liberty added to let 'random', 'srandom' and 'ftime' functions work under
# System V

random:	random.c
	gcc random.c -o random
	strip random
	chmod 755 random

iftime:	iftime.c
	gcc iftime.c -o iftime
	strip iftime
	chmod 755 iftime

changedate:	changedate.c
	gcc changedate.c -o changedate
	strip changedate
	chmod 755 changedate

nicedate:	nicedate.c
	gcc nicedate.c -o nicedate
	strip nicedate
	chmod 755 nicedate

textnum:	textnum.c
	gcc textnum.c -o textnum
	strip textnum
	chmod 755 textnum

hitstats:	hitstats.c
	gcc hitstats.c -o hitstats
	strip hitstats
	chmod 755 hitstats

hit:	hit.c
	gcc hit.c -o hit
	strip hit
	chmod 755 hit

roman:	roman.c
	gcc roman.c -o roman
	strip roman
	chmod 755 roman
