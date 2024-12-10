#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <mysql/mysql.h>

#define PORT 8080
#define MAX_CLIENTS 4
#define BUFFER 1024


//server handling of database


void *handleClient(void *arg);
void messenger(char *message, int fromSender);
void handle_sigint(int sig);
void getWeather(const char *city, char *response);

int clients_sockets[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int running = 1;
MYSQL *conn;

const char *server = "127.0.0.1";
const char *user = "root";
const char *password = "mysql";
const char *database = "weather_db";



//Handles signal interrupt detection
void handle_sigint(int sig){
        printf("\nServer shutting down...\n");
        running = 0;
}

// retrieves weather from data base and returns the response to the client
void getWeather(const char *city, char *response){
        char query[256];
        MYSQL_ROW row;

  
          //Queries the database to find weather in the city selected
        snprintf(response, BUFFER, "SELECT temperature, humidity, description FROM weather WHERE city= '%s'", city);
        //Checks query is successful
        if(mysql_query(conn, query)){
                snprintf(response, BUFFER, "Error: %s\n", mysql_error(conn));
                return;
        }
//stores the result, and checks non null storage      
        MYSQL_RES *result = mysql_store_result(conn);
        if(result == NULL){
                snprintf(response, BUFFER, "Failed to store query");
                mysql_close(conn);
        }
        //Queries the database after fetching rows from the result. Rows populate the output data
        if((row = mysql_fetch_row(result)) != NULL){
                snprintf(response, BUFFER, "Temperature: %sC\nHumidity: %s\nDescription: %s\n", row[0], row[1],row[2]);
        }else{
                snprintf(response, BUFFER, "City not found");
        }

        mysql_free_result(result);
}

int main(){

        int svr_sock, client_sock, *new_sock;
        struct sockaddr_in server_addr, client_addr;

        socklen_t addr_size;
        pthread_t thread;

        //char *response;

        printf("Connected to database\n");


        //Initialize data base and check for failure.
        conn = mysql_init(NULL);
        if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
                fprintf(stderr, "MYSQL connection failed: %s\n", mysql_error(conn));
                mysql_close(conn);
                return EXIT_FAILURE;
        }


        //This is the signal interrupt handler initialization. This handles exiting the program using ctrl-z
        signal(SIGINT, handle_sigint);

        //initialize the client sockets
        memset(clients_sockets, 0, sizeof(clients_sockets));

        svr_sock = socket(AF_INET, SOCK_STREAM, 0);
        if(svr_sock < 0){
                perror("Sockets not created.\n");
                exit(EXIT_FAILURE);
        }
        //configure server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        //bind the socket to the address
        if(bind(svr_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
                perror("failed to bind");
                close(svr_sock);
                exit(EXIT_FAILURE);
        }
        //This checks for server socket connection requests
        if(listen(svr_sock, MAX_CLIENTS) < 0){
                perror("Listen failed");
                close(svr_sock);
                exit(EXIT_FAILURE);
        }

        printf("Chat started. Waiting for clients...\n");


        //While running, this allows clients to join, handles if they cannot join, and is the message hosting point of the server
        while(running){
                addr_size = sizeof(client_addr);
                client_sock = accept(svr_sock, (struct sockaddr*)&client_addr, &addr_size);
                if(client_sock < 0){
                        perror("Client not accepted");
                }

                //adding client to server
                pthread_mutex_lock(&clients_mutex);
                int numClient = 0;
                for(int i = 0 ; i < MAX_CLIENTS ; i++){
                        if(clients_sockets[i] == 0 ){
                                clients_sockets[i] = client_sock;
                                numClient = 1;
                                break;
                        }
                }

                pthread_mutex_unlock(&clients_mutex);
                //Checks that the number of clients is not exceeding the maximum number. If so, the connection to the new client is dropped.
                if(!numClient){
                        printf("Maximum clients reached.\n");
                        close(client_sock);
                }

                //Creates the new socket for the client and begins the detached threading.
                printf("New client connected: Client %d\n", client_sock);
                new_sock = malloc(sizeof(int));
                *new_sock = client_sock;
                pthread_create(&thread, NULL, handleClient, new_sock);
                pthread_detach(thread);
        }
        //closes the server
        close(svr_sock);
        mysql_close(conn);
        return 0;

}

//displays message from a sender to the chat
void messenger(char *message, int fromSender){

        pthread_mutex_lock(&clients_mutex);
        for(int i = 0 ; i < MAX_CLIENTS ; i++){
                if(clients_sockets[i] != 0 && clients_sockets[i] != fromSender){
                        send(clients_sockets[i], message, strlen(message),0);
                }
        }
        pthread_mutex_unlock(&clients_mutex);

}

//Threading function that handles the client interactions
void *handleClient(void *arg){

        int client_socket = *(int*)arg;
        char buffer[BUFFER];
        int bytes_in;
        char response[BUFFER];

        //send message
        while((bytes_in = recv(client_socket, buffer, BUFFER, 0)) > 0){
                buffer[bytes_in] = '\0';
                printf("Client %d: %s", client_socket, buffer);
                messenger(buffer, client_socket);
        }

        getWeather(buffer, response);

        pthread_mutex_lock(&clients_mutex);

        //disconnect clients
        for(int i = 0 ; i < MAX_CLIENTS; i++){
                if(clients_sockets[i] == client_socket){
                        clients_sockets[i] = 0;
                        break;
                }
        }

        pthread_mutex_unlock(&clients_mutex);

        printf("Client %d has disconnected\n", client_socket);
        close(client_socket);
        free(arg);
}
