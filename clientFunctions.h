#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define MAXLENGTH 32 
#define BUFFER 100

typedef struct PokemonType {
    int pokedex_number;
    char pokemon_name[MAXLENGTH];
    char primary_type[MAXLENGTH];
    char secondary_type[MAXLENGTH];
    int total;
    int hp;
    int attack;
    int defense;
    int special_attack;
    int special_defence;
    int speed;
    int generation;
    char legendary[6];
} Pokemon;


// writedata structure, which will get passed into the write thread function as an argument
typedef struct WriteDataType {
    Pokemon* queried_pokemons;
    int num_pokemons;
    sem_t mutex;
} WriteData;

// function prototypes
void print_menu();
char* get_pokemon_type();
void *write_thread_function(void *arg);
void write_pokemons(Pokemon *pokemons, int num_pokemons);
void pokemon_to_line(char *line_to_write, const Pokemon *pokemon_to_write, char *separator);
