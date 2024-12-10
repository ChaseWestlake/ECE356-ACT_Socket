# ECE356-ACT_Socket



Client.c, server.c, and weatherdb.sql are part of Part 1. I attempted to make the server access the sql database, rather than have the C program manage a struct of some kind containing the data.



library.c and library_db.sql are part 2. 

The username and password for both parts on sql are root, and mysql.

to run the .sql files, type mysql -u root -p < "file".sql

There are issues in the code, and they don't run perfectly. 


to compile and run the .c files for library.c and server.c, use gcc "file.c" -o "File" -lmysqlclient 

gcc "file.c" -o "file" For the other .c files.
