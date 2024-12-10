#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>



void databaseConnect(MYSQL **conn);
void addBook(MYSQL *conn);
void viewBooks(MYSQL *conn);
void searchBooks(MYSQL *conn);

//declares login info globally
const char *host = "localhost";
const char *user = "root";
const char *password = "mysql";
const char *db_name = "library_db";
int size = 50;

void viewBooks(MYSQL *conn){

      //tests query to select the contents from books in the database
        if(mysql_query(conn, "SELECT * FROM books")){
                fprintf(stderr, "Could not retrieve books");
                return;
        }
        //stores the results and checks to ensure they were collected
        MYSQL_RES *result = mysql_store_result(conn);
        if(result == NULL){
                fprintf(stderr, "Unable to store results");
                return;
        }


        //Iterates over the contents of the book and prints for the user
        MYSQL_ROW row;

        printf("Title | Author | Year\n");
        while(row = mysql_fetch_row(result)){
                printf("%s | %s | %s\n", row[0], row[1], row[2]);
        }

        mysql_free_result(result);

}


//Manage database connection and do error checking
void databaseConnect(MYSQL **conn){

        //Init database and handle failure
        if((*conn = mysql_init(NULL)) == NULL){
                fprintf(stderr, "Cannot initialize database");
                exit(EXIT_FAILURE);
        }
        //connect and check connection to database
        if(mysql_real_connect(*conn, host, user, password, db_name, 0, NULL, 0) == NULL){
                fprintf(stderr, "Connection failed");
                mysql_close(*conn);
                exit(EXIT_FAILURE);
        }


}

void addBook(MYSQL *conn){

        char title[size];
        char author[size];
        int year;

        //Prompts user to enter new books to the library
        printf("Enter book title");
        scanf("%[^\n]", title);
        printf("Enter author");
        scanf("%[^\n]", author);
        printf("Enter year ");
        scanf("%d", &year);

        char query[size * 4];

        //Queries the database, and adds the book specified byu the user
        snprintf(query, sizeof(query), "INSERT INTO books (title, author, year) VALUES ('%s', '%s', %d)", title, author, year);

        if(mysql_query(conn,query)){
                fprintf(stderr, "Error adding book\n");
        }else{
                printf("Book added\n");
        }

}


void searchBooks(MYSQL *conn){

        char title[size];


        //prompts user to enter the name of the book they want to view
        printf("Enter title: ");
        scanf("%[^\n]", title);

        char query[size * 4];


          /// queries database to attempt to find the requested book. Checks results
        snprintf(query, sizeof(query), "SELECT * FROM books WHERE title LIKE '%%%s%%'", title);

        MYSQL_RES *result = mysql_store_result(conn);
        if(result == NULL){
                fprintf(stderr, "Unable to store results");
                return;
        }


        //prints the information of the book
        MYSQL_ROW row;
        printf("Title | Author | Year\n");
        while(row = mysql_fetch_row(result)){
                printf("%s | %s | %s\n", row[0], row[1], row[2]);
        }

        mysql_free_result(result);


}

int main(){

        MYSQL *conn;
        databaseConnect(&conn);


        int choice;
        //display options, and wait for user to make choice.
        printf("\nLibrary System\n\n1. Add book\n2. View Books\n3. Search book by title\n4. Exit");
        scanf("%d", &choice);


        //Handle user choice using switch case, calling the appropriate functions.
        switch(choice){
                case 1:
                        addBook(conn);
                        break;
                case 2:
                        viewBooks(conn);
                        break;
                case 3:
                        //searchBooks(conn);
                        break;
                case 4:
                        printf("Exiting Library\n");
                        break;
                default:
                        printf("Invalid\n");
        }
        //While user does not exit, dont disconnect
        while(choice != 4);

        mysql_close(conn);
        return 0;


}
