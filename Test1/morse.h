#ifndef _MORSE_H
#define _MORSE_H

/* header file for morse.c */
#include <xmc_common.h>

// Global variables
extern const char morseAlpha[26][6];
extern const char morseNum[10][6];

// function prototypes
char* convString2Morse(char*);
uint8_t convMorse2NumPeriods(char);

#endif