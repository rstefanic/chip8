#include "bcd.h"

unsigned char bcd_hundreds_at(unsigned char val)
{
    return val / 100;
}

unsigned char bcd_tens_at(unsigned char val)
{
    return (val / 10) % 10;
}

unsigned char bcd_ones_at(unsigned char val)
{
    return val % 10;
}