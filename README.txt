/* Networks Homework 2 - Calvin Banning */

========================
=LIST OF RELEVANT FILES=
========================
README.txt - this file.
checksum.c - checksum function.
DieWithError.c - runs perror and exits with error code.
server.c - server source code file.
client.c - client source code file.
makefile - makefile.
/ClientFiles/ - Subdirectory for holding the downloaded file.
	        The file is named download.txt.
/ServerFiles/ - Subdirectory for files to be uploaded by the server.
	        The two files are:
		example.txt - small one-line file.
		constitution.txt - U.S. Constitution, large text file.

==========================
=COMPILATION INSTRUCTIONS=
==========================
The executables are compiled with -Wall (all warnings) and
-g (to check for memory leaks). If you don't want these, edit
the makefile.

Make commands:
make all - build the client and the server (you probably want this)
make client - build only the client
make server - build only the server
make clean - cleanup all .o files, temporary files, core dumps, executables

other make rules are for the four object files.

=====================
=CONFIGURATION FILES=
=====================
None.

======================
=RUNNING INSTRUCTIONS=
======================
1) You will need two terminal windows (one for the client, one for the server).
2) cd into the folder in each terminal.
3) run 'make' in one terminal, which defaults to make all, which will make both
   the server and the client.
4) In one terminal, run ./server 8080
   In the other terminal, run ./client 127.0.0.1 8080
   In some cases, port 8080 will be in use, so you might want to try another
   Make sure the server has started before you request a file with the client.
5) The client will ask you for a file. Type in 'example.txt' or
   'constitution.txt' (without the quotes) two download one of the two files.
6) The file will download to ./ClientFiles/download.txt. A checksum is sent
   by the server which validates the file. You can also use the diff program
   to prove that download.txt is the same as the file which you requested from
   the server.
