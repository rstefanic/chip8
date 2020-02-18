#include "rng.h"

void initialize_rng()
{
    srand(time(NULL));   
}

short rng()
{
    return rand();
}