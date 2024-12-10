#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>


#define PORT 8080
#define BUFFER 1024

void handle_sigint(int sig);
void *message(void *arg);

int running = 1;

void handle_sigint(int sig){

        printf("\nDisconnecting from server\n");
        running = 0;
}

int main(){

        int client_socket;
        struct sockaddr_in server_addr;
        pthread_t thread;
        char buffer[BUFFER];

        signal(SIGINT, handle_sigint);

        //create the client socket
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(client_socket < 0){
                perror("Socket failed");
                exit(EXIT_FAILURE);
        }


        //Configure the address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
                perror("Connection failed");
                close(client_socket);
                exit(EXIT_FAILURE);
        }

        printf("Connected to server\n");

        //create the thread for messaging
        pthread_create(&thread, NULL, message, &client_socket);

        //get and recieve messages during connection
        while(running){
                printf("Enter a city name: ");
                fgets(buffer, BUFFER, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';

                if(send(client_socket, buffer, strlen(buffer), 0) <= 0){
                        perror("Failed to send");
                        break;
                }

                //send(client_socket, buffer, strlen(buffer), 0);
                //printf("Client %d: %s\n", client_socket, buffer);
        }

        pthread_join(thread, NULL);

        close(client_socket);
        return 0;

}

//handle messages
void *message(void *arg){
        int socket = *(int*)arg;

        char buffer[BUFFER];
        int recieved;

        while((recieved = recv(socket, buffer, BUFFER, 0)) > 0){
                buffer[recieved] = '\0';
                printf("Server: %s\n", buffer);
        }

        printf("Disconnected\n");
        pthread_exit(NULL);
}
