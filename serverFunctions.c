#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "serverFunctions.h"

// read from csv, and parse the data into pokemon structure
int read_pokemon(FILE *pokemon_csv, Pokemon **pokemons, char *pokemon_type)
{
    int num_lines, num_pokemons, found_pokemon;
    num_lines = num_pokemons = 0;
    char line[100];
    // loop through each line of file
    while (fscanf(pokemon_csv, "%[^\n]\n", line) != EOF)
    {
        //printf("line: %s\n", line);
        if (num_lines > 0)
        {
            // dynamically increase the size as the file gets read and stores more data
            if (num_lines == 1)
            {
                *pokemons = calloc(1, sizeof(Pokemon));
                if (pokemons == NULL)
                {
                    printf("Error while trying to allocate memory for pokemons.");
                    exit(EXIT_FAILURE);
                }
            }
            if (num_pokemons >= 1)
            {
                *pokemons = realloc(*pokemons, (num_pokemons+1) * sizeof(Pokemon));
                if (pokemons == NULL)
                {
                    printf("Error while trying to allocate memory for pokemons.");
                    exit(EXIT_FAILURE);
                }
            } 
            // parse line data into the pokemon structure
            found_pokemon = line_to_pokemon(line, &(*pokemons)[num_pokemons], ",", pokemon_type);
            if (found_pokemon){ 
                num_pokemons++;
            }
        }
        num_lines++;
    }
    return num_pokemons;
}

// parse the line and convert the data back into the pokemon structure
int line_to_pokemon(char *line, Pokemon *pokemon, char *separator, char *pokemon_type)
{
    // separate line into the different data, by separating the commas using strsep
    char *pokedexNum = strsep(&line, separator);
    char *pokemonName = strsep(&line, separator);
    char *primaryType = strsep(&line, separator);
    char *secondaryType = strsep(&line, separator);
    char *total = strsep(&line, separator);
    char *hp = strsep(&line, separator);
    char *attack = strsep(&line, separator);
    char *defense = strsep(&line, separator);
    char *specialAttack = strsep(&line, separator);
    char *specialDefense = strsep(&line, separator);
    char *speed = strsep(&line, separator);
    char *generation = strsep(&line, separator);
    char *legendary = strsep(&line, separator);

    // if query pokemon type = primaryType, parse the pokemons data into memory
    if (strcmp(pokemon_type, primaryType) == 0)
    {
        pokemon->pokedex_number = strcmp(pokedexNum, "") ? atoi(pokedexNum) : -1;
        strcpy(pokemon->pokemon_name, pokemonName);
        strcpy(pokemon->primary_type, primaryType);
        strcpy(pokemon->secondary_type, secondaryType);
        pokemon->total = strcmp(total, "") ? atoi(total) : -1;
        pokemon->hp = strcmp(hp, "") ? atoi(hp) : -1;
        pokemon->attack = strcmp(attack, "") ? atoi(attack) : -1;
        pokemon->defense = strcmp(defense, "") ? atoi(defense) : -1;
        pokemon->special_attack = strcmp(specialAttack, "") ? atoi(specialAttack) : -1;
        pokemon->special_defence = strcmp(specialDefense, "") ? atoi(specialDefense) : -1;
        pokemon->speed = strcmp(speed, "") ? atoi(speed) : -1;
        pokemon->generation = strcmp(generation, "") ? atoi(generation) : -1;
        strcpy(pokemon->legendary, legendary);
        //printf("%s, from gen %d\n", pokemon->pokemon_name, pokemon->generation);
        return 1;
    }
    return 0;
}
