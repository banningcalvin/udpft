/* Networks Homework 2 - Calvin Banning */
/* Uses go-back-n with a windowsize dictated by the client */
/* and a biterror probability chosen by the server to simulate */
/* real-world conditions for reliable data transfer over udp */

!!!~ READ THIS ENTIRE FILE BEFORE RUNNING ~!!!

========================
=LIST OF RELEVANT FILES=
========================
README.txt - this file.
checksum.c - checksum function.
DieWithError.c - runs perror and exits with error code.
server.c - server source code file.
client.c - client source code file.
makefile - makefile.
filesize.dat - timing data for gnuplot graphs with filesize being the variable
windowsize.dat - timing data for gnuplot graphs with windowsize being the variable
errorprob.dat - timing data for gnuplot graphs with error probability being the variable
ClientFiles/ - Subdirectory for holding the downloaded file.
	        The file is named download.txt.
ServerFiles/ - Subdirectory for files to be uploaded by the server.
	        The two files are:
		example.txt - small one-line file.
		constitution.txt - U.S. Constitution, large text file.
.git/ - git folder
.gitignore - git ignore file

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
4) In one terminal, run:
        ./server 8080 .1
        The arguments are the port and the biterror probability
   In the other terminal, run:
        ./client 127.0.0.1 8080 1
        The arguments are the server IP, the port, and the window size 
   In some cases, port 8080 will be in use, so you might want to try another.
   Counting up from 8080 usually finds an open port pretty soon.
   Make sure the server has started before you request a file with the client.
5) The client will ask you for a file. Type in 'example.txt' or
   'constitution.txt' (without the quotes) two download one of the two files.
   You must restart the client for every download, and every download will
   overwrite "download.txt". This is intentional, to allow for different window
   sizes without having to restart the server. Sending "EXIT" as the filename
   will cause both the client and server to exit.
6) The file will download to ./ClientFiles/download.txt. A checksum is sent
   by the server which validates the file. You can also use the diff program
   to prove that download.txt is the same as the file which you requested from
   the server.

===============
=TESTING NOTES=
===============
There are two testing files included, constitution.txt and example.txt, which
are 44KB and 45 bytes, respectively. Because these files sent so fast, very
large files (which are not included because of their size) were used in the
test graphed with gnuplot for part 3.
The filesize graph used a windowsize of size 1 and error probability of 0.
It can be seen in filesizeplot.png.
The errorprob plot used a filesize of 20MB and window size of 1.
It can be seen in errorprobplot.png.
The windowsize plot uses a filesize of 20mb and errorprob of .1.
It can be seen in windowsizeplot.png.
