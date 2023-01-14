#include <stdlib.h>
#include <stdio.h>


#define MAXLENGTH 32
#define BUFFER 100

// Pokemon structure
typedef struct PokemonType
{
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

int read_pokemon(FILE *, Pokemon **, char*);
int line_to_pokemon(char *, Pokemon *, char *, char *);