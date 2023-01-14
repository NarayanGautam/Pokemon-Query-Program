/*
 ============================================================================
 Name          : pps.c
 Author        : Shuva Narayan Gautam
 Description   : Pokemon Property Server, program will listen on localhost port 6000
 accepting connection with Pokemon Query Client, and handling any query requests that 
 are made by the client, by sending over the pokemons that match the pokemon type query 
 sent by the client. 
 ============================================================================
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#include "serverFunctions.h"

// define server port to be 6000 (port 80 did not work)
#define SERVER_PORT 6000


int main() {
    // File type to store the csv file for reading
    FILE *pokemon_csv;
    Pokemon *pokemons = NULL;
    char *filename;
    int menu_choice, num_pokemons;

    // ask user for file name until file can be opened for reading
    while(1) {
        printf("Enter the name of the pokemon descriptions file: ");
        scanf("%ms", &filename);
        // filename to be freed

        pokemon_csv = fopen(filename, "r");
        if (!pokemon_csv)
            printf("Pokemon file is not found. Please enter the name of the file again. \n");
        else
            break;        
    }

    // variables for server setup
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddr;
    int status, addrSize, bytesRcv;
    char buffer[30];
    char response[10];

    // setup a stream socket, using AF_INET, SOCK_STREAM, and TCP protocol as parameters
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket < 0) {
    printf("SERVER ERROR: Could not open socket.\n");
    exit(-1);
    }

    // setup the server address info
    memset(&serverAddress, 0, sizeof(serverAddress)); 
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

    // assign IP address to the socket by binding the socket 
    status = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    if (status < 0) {
    printf("SERVER ERROR: Could not bind socket.\n");
    exit(-1);
    }    

    // listen for incoming requests (with a backlog of 5)
    status = listen(serverSocket, 5);
    if (status < 0) {
    printf("SERVER ERROR: Could not listen on socket.\n");
    exit(-1);
    }

    // infinite loop to accept incoming client requests
    while (1) {
        addrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);

        if (clientSocket < 0) {
        printf("SERVER ERROR: Could not accept incoming client connection.\n");
        exit(-1);
        }
        printf("SERVER: Received client connection.\n");

        // infinite loop that receives client requests and sends responses
        while(1) {
            // first receive will be the pokemon type to query 
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;
            printf("SERVER: Received client request to query for Pokemon of type: %s\n", buffer);
            // if client types "done", the client socket closes, and "stop closes the server also 
            if ((strcmp(buffer, "done") == 0) || (strcmp(buffer, "stop") == 0))
                break;

            // get the number of pokemon matching the buffer (pokemon type), using the read_pokemon function
            int num_pokemons = read_pokemon(pokemon_csv, &pokemons, buffer);
            sprintf(response, "%d", num_pokemons);
            // send the client the number of pokemon matching the type query
            printf("SERVER: Sending message that there are \"%s\" %s Type Pokemon to client\n", response, buffer);
            send(clientSocket, response, strlen(response), 0);

            // receove client confirmation of receiving and storing enough memory for query pokemon
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;
            printf("SERVER: Received client message: \"Stored %s in memory\" \n", buffer);

            // for each pokemon matching query type, send to client the pokemon name, and receive confirmation from client
            for (int i=0; i<num_pokemons; i++) {
                printf("SERVER: Sending Pokemon %s to client\n", pokemons[i].pokemon_name);
                send(clientSocket, pokemons[i].pokemon_name, strlen(pokemons[i].pokemon_name),0);
                bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesRcv] = 0;
                printf("SERVER: Received client message: \"Stored %s in memory\" \n", buffer);
            }

            free(pokemons);
            pokemons = NULL;
            //send(clientSocket, response, strlen(response), 0);
        }
        printf("SERVER: Closing client connection.\n");
        close(clientSocket); // close clients socket
        // if the client enters stop, then server will stop
        if (strcmp(buffer, "stop") == 0)
            break;
    }
    // close server socket, and close csv file, and any frees
    close(serverSocket);
    printf("SERVER: Shutting down. \n");
    fclose(pokemon_csv);
    free(filename);
}