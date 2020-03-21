#ifndef RNG_H
#define RNG_H

#include "chip8.h"

/* The random number generator is used for a 
 * 16-bit gray code counter so that a 16-bit
 * result can be obtained 
 */

void initialize_rng();
short rng();

#endif /* RNG_H not defined */