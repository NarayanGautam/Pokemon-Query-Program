#include "clientFunctions.h"

// print client menu
void print_menu()
{
    printf("\n|===  Welcome to the POKEMON QUERY PROGRAM Menu ===|");
    printf("\n\nMenu Options: \na. Type Search\nb. Save Results \nc. Exit the Program \n");
    printf("(menu instructions ----> type 'a' if you would like to use option a, etc.)\n\n");
}

// gets the pokemon type to query to the server
char *get_pokemon_type()
{
    char *type;
    printf("What is the Pokemon Type you would like to query? ");
    scanf("%ms", &type);
    return type;
}

// write thread function to save pokemons to a file
void *write_thread_function(void *arg) {
    WriteData *data = (WriteData *)arg;

    if (sem_wait(&data->mutex) < 0) {
        printf("Error on semaphore wait. ");
        exit(1);
    }
    printf("We made it here! ");
    write_pokemons(data->queried_pokemons, data->num_pokemons);    

    if (sem_post(&data->mutex) < 0) {
        printf("Error on semaphore post. ");
        exit(1);
    }
}

// opens a file to write to, and writes pokemon data into file 
void write_pokemons(Pokemon *pokemons, int num_pokemons) {
    FILE *pokemon_write_file;
    char *file_to_write;
    
    while(1) {
        printf("Enter the name of file to write query results to: ");
        scanf("%ms", &file_to_write);

        pokemon_write_file = fopen(file_to_write, "w");

        if(!pokemon_write_file) {
            printf("Unable to create the new file. Please enter the name of the file again.");
        } else {
            break;
        }
    }

    for(int i=0; i<num_pokemons; i++) {
        char line[BUFFER] = "";
        pokemon_to_line(line, &pokemons[i], ",");
        printf("Line %d: %s", i, line);
        // print the formattted line to the write file
        fprintf(pokemon_write_file, "%s\n", line);
    }

    // close the write file 
    fclose(pokemon_write_file);
    printf("Pokemon query results written to: %s\n", file_to_write); 

    free(file_to_write);
}

// concatenates pokemon attributes to a string 
void pokemon_to_line(char *line_to_write, const Pokemon *pokemon_to_write, char *separator) {
    char pokedex_num[4];
    sprintf(pokedex_num, "%d", pokemon_to_write->pokedex_number);
    line_to_write = strcat(line_to_write, pokedex_num);
    line_to_write = strcat(line_to_write, separator);
    line_to_write = strcat(line_to_write, pokemon_to_write->pokemon_name);
    line_to_write = strcat(line_to_write, separator);
    line_to_write = strcat(line_to_write, pokemon_to_write->primary_type);
    line_to_write = strcat(line_to_write, separator);
    line_to_write = strcat(line_to_write, pokemon_to_write->secondary_type);
}

