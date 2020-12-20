README.txt for Tux Paint Savedir directory

ftp://ftp.tuxpaint.org/unix/x/tuxpaint/savedir/

Bill Kendrick
bill@newbreedsoftware.com
http://www.tuxpaint.org/

2005-Feb-09 - 2011-Dec-14

What's this?
------------
This area contains various versions of a small tool that can be used to
prompt the user for their name, or some other unique information, and
then use their choice as the value for a "--savedir" option sent to
Tux Paint on the command-line.

Background:
-----------
Many schools give students space for their files on a remote,
network-accessible file server.

Some of these schools use software to write-protect their PCs hard drives,
meaning the remote file server is the ONLY place they can store files.

In most cases, there is a login procedure, and each student has their
own "home directory" on the file server, which is 'mounted' to a particular
location on the PC.  For example, an "H:" drive under Windows.

For "Billy", his H: drive actually points to a "billy" directory somewhere
on the file server, separate from the other students.  "Suzy" also has an
H: drive on her PC, but it points to a "suzy" directory on the file server,
keeping her files separate from "Billy"'s.

The problem:
------------
HOWEVER, some schools do not have a login procedure, or all students
(at least in a particular class) share the same login.

In this case, multiple PCs end up 'mounting' the same directory off of the
remote file server.  (All of their "H:" drives look the same, because
they are the same!)

Since Tux Paint currently does not ask for filenames or directories when
saving, if a "savedir" setting such as "H:\tuxpaint\" were used,
all users would end up saving their pictures in the same place on the
remote file server.

This can lead to users clobbering each other's work.  NOTE: some may argue
this isn't a bad thing, and that children can learn sharing and responsibility
by not putting up barriers -- I won't argue for or against this...  I'm just
here to make the tools that work best for everyone's needs! :^)

More importantly, this can make it difficult (and slow) for a student to
find a particular picture, to work on it again later.

Rather than give each student access to the powerful "Tux Paint Config"
tool, and have them go into its interface and change the "savedir" setting,
it seemed reasonable to SIMULATE the login procedure that other schools
use.

The solution:
-------------
This tool simply shows a small dialog containing a list of student's names
(or class rooms, or other identifying information), each of which correspond
to a particular directory.

Using the students from the example above, even though "Billy" and "Suzy"
both have the same directory on the file server mounted as their PC's "H:"
drive, this tool can cause Tux Paint to save its files in either
"H:\billy\" or "H:\suzy\", depending on which name they pick from a list.

It does this simply by calling the Tux Paint executable (e.g., "tuxpaint.exe")
with the "--savedir" command-line argument.  (It's the same as the directory
choice found in the "Tux Paint Config" GUI tool, except it's not coming
out of a configuration file.)

Versions:
---------
  tuxpaint-savedir-2005-02-11_1.zip -- First attempt at converting the wxPython
                         version (see below) into a Windows executable...
                         Variables (BASE_DIR, CMD, etc.) are all hard-coded;
                         I should make them loaded from a config file,
                         at the least.

                         NOTE: This will soon be superceded by a build based
                         on tuxpaint-savedir-v3.py, from December 2011.

  tuxpaint-savedir.sh -- Linux BASH shell script that invokes KDE's "kdialog"
                         tool to display a list of names.  Upon clicking "OK",
                         "tuxpaint --savedir {directory}" is called.

                         It pulls the names from a plain ASCII text file
                         (directory, then name/label to display in the GUI),
                         separated by carriage return characters.

                         A "base" save directory can be specified
                         (e.g., "/mnt/remoteserver/tuxpaint/"), so that it
                         can be changed in one fell swoop, and so that the
                         user list text file doesn't have to specify it for
                         every entry.

                         See the contents of the ".sh" file for the lines
                         where you must set these options, and for lots of
                         comments.

  tuxpaint-savedir-v3.py -- Python script utilizing wxPython (wxWidgets module)
                         to provide a GUI that displays a list of names.
                         Upon clicking "Launch!", Tux Paint is executed with
                         the "--savedir" command-line argument.

                         The script pulls the names and directories from a
                         plain ASCII text file.  The file's format is:
                         name/label to display in the GUI, and then
                         corresponding directory, separated by carriage
                         return characters.  (USERS_FILE)

                         A "base" save directory can be specified
                         (e.g., "/mnt/remoteserver/tuxpaint/"), so that it
                         can be changed in one fell swoop, and so that the
                         user list text file doesn't have to specify it for
                         every entry.  (BASEDIR)

                         You can tell the script where to create directories
                         ("mkdir") before trying to run Tux Paint.  (MAKE_DIR)

                         See the contents of the ".py" file for the lines
                         where you can set these options.

                         The path to, and filename of, the Tux Paint
                         executable can be specified, as well. (CMD)

-bill!

