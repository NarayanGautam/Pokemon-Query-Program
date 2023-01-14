/*
 ============================================================================
 Name          : pqc.c
 Author        : Shuva Narayan Gautam
 Description   : Pokemon Query Client, establishes a connection to the Pokemon 
 Property Server, displays a menu for user to either send a pokemon type query 
 to the server, save query results or exit program. 
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
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#include "clientFunctions.h"

// define server IP and sever port to match the pokemon property server
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000


int main() {
    // pokemon structure to store matching pokemon types in memory
    Pokemon *found_pokemons = NULL;

    // server set up variables
    int clientSocket, status, bytesRcv;
    struct sockaddr_in serverAddress;
    pthread_t t1;

    char buffer[80];

    // open client socket
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
        printf("CLIENT ERROR: Could not open socket\n");
        exit(-1);
    } 
    
    // set address info
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

    // connect client to the server
    status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (status < 0) {
        printf("CLIENT ERROR: Unable to establish connection to the PPS! \n");
        // close client socket if unable to establish connection
        close(clientSocket);
        exit(-1);
    }

    int num_pokemon_found = 0;
    char menu_choice, *pokemon_type;
    // loop that repeatedly prints out menu, gets user choice to query, save or quit
    while (1) {
        print_menu();
        scanf("%c", &menu_choice);
        if (menu_choice == 'a') {
            // type search query sent to server
            printf("CLIENT: Requesting a Pokemon type search to server.\n");
            pokemon_type = get_pokemon_type();
            strcpy(buffer, pokemon_type);
            printf("CLIENT: Sending Pokemon Type request of %s \n", buffer);
            // sends a buffer that contains the pokemon type to be queried in the server
            send(clientSocket, buffer, strlen(buffer), 0);
            
            // receives the number of pokemon matching the query
            bytesRcv = recv(clientSocket, buffer, 80, 0);
            buffer[bytesRcv] = 0; 
            printf("CLIENT: Got back response \"%s\" from server.\n", buffer);

            // send confirmation message to server
            printf("CLIENT: Sending \"got %s \" to server \n", buffer);
            send(clientSocket, buffer, strlen(buffer), 0);

            // convert buffer to an intger to malloc enough space to store pokemon of queried type
            num_pokemon_found = atoi(buffer);
            found_pokemons = (Pokemon *) malloc(num_pokemon_found * sizeof(Pokemon));
            // loop through the number of pokemon that match query type
            for(int i=0; i<num_pokemon_found; i++) {
                // receive the pokemon name in buffer
                bytesRcv = recv(clientSocket, buffer, 80, 0);
                buffer[bytesRcv] = 0;
                printf("CLIENT: Got back response \"%s\" from server.\n", buffer);
                printf("CLIENT: Sending \"got %s \" to server \n", buffer);
                send(clientSocket, buffer, strlen(buffer), 0);
                // store the pokemon into the found_pokemons structure 
                strcpy(found_pokemons[i].pokemon_name, buffer);
            }
        } 
        if (menu_choice == 'b') {
            // save results by writing to file
            printf("CLIENT: Saving Query Results! \n");
            
            // create a WriteData structure to store all write data, which gets passed into write thread function
            WriteData *write = (WriteData *)malloc(sizeof(WriteData));
            if (write == NULL) {
                printf("Error allocatting memory for the query");
                exit(EXIT_FAILURE);
            }

            write->queried_pokemons = found_pokemons;
            write->num_pokemons = num_pokemon_found;

            // create write thread, passing in the write data structure as parameter
            // this save operation can run in the background
            pthread_create(&t1, NULL, write_thread_function, (void *)write);
            pthread_join(t1, NULL);
            free(write);
        } 

        if (menu_choice == 'c'){ // user decides to exit client program
            // print number of queries completed
            // display new files created in session
            break;
        }

        // print query results 
        printf("Here are all the pokemon of type %s\n", pokemon_type);
        for (int i = 0; i<num_pokemon_found; i++) {
            printf("%s\n", found_pokemons[i].pokemon_name);
        }
            
    }
    
    // close client socket and any frees
    close(clientSocket);
    printf("CLIENT: Shutting down. \n");
    free(pokemon_type);
    free(found_pokemons);

}