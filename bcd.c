#include "bcd.h"

char bcd_hundreds_at(char reg)
{
    return (reg/100) % 10;   
}

char bcd_tens_at(char reg)
{
    return (reg/10) % 10;
}

char bcd_ones_at(char reg)
{
    return reg % 10;
}