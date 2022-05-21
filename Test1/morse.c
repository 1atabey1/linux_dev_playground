#include "morse.h"

/* Global Variables */
const char morseAlpha[26U][6U] = {
    {".-"},    // A
    {"-..."},  // B
    {"-.-."},  // C
    {"-.."},   // D
    {"."},     // E
    {"..-."},  // F
    {"--."},   // G
    {"...."},  // H
    {".."},    // I
    {".---"},  // J 
    {"-.-"},   // K
    {".-.."},  // L
    {"--"},    // M
    {"-."},    // N
    {"---"},   // O
    {".--."},  // P
    {"--.-"},  // Q
    {".-."},   // R
    {"..."},   // S
    {"-"},     // T
    {"..-"},   // U
    {"...-"},  // V
    {".--"},   // W
    {"-..-"},  // X
    {"-.--"},  // Y
    {"--.."}   // Z
};
const char morseNum[10U][6U] = {
    {"-----"}, // 0
    {".----"}, // 1
    {"..---"}, // 2
    {"...--"}, // 3
    {"....-"}, // 4
    {"....."}, // 5
    {"-...."}, // 6
    {"--..."}, // 7
    {"---.."}, // 8
    {"----."} // 9
};

/* Functions */
char* convString2Morse(char *inString)
{
    static char outStr[1000U];
    uint16_t i = 0U;
    uint16_t outStrPos = 0U;
    uint16_t k=0U;
    
    // make sure that outStr is empty
    while (outStr[k] != '\0')
    {
        outStr[k++] = '\0';
    }
    
    while(inString[i] != '\0')
    {
        // check for numbers
        if (inString[i] >= '0' && inString[i] <= '9')
        {
            k = 0U;
            while (morseNum[inString[i]-'0'][k] != '\0')
            {
                outStr[outStrPos+k] = morseNum[inString[i]-'0'][k];
                k++;
            }
            outStrPos += k;
        }
        // check for letters (upper)
        else if (inString[i] >= 'A' && inString[i] <= 'Z')
        {
            k = 0U;
            while (morseAlpha[inString[i]-'A'][k] != '\0')
            {
                outStr[outStrPos+k] = morseAlpha[inString[i]-'A'][k];
                k++;
            }
            outStrPos += k;
        }
        // check for letters (lower)
        else if (inString[i] >= 'a' && inString[i] <= 'z')
        {
            k = 0U;
            while (morseAlpha[inString[i]-'a'][k] != '\0')
            {
                outStr[outStrPos+k] = morseAlpha[inString[i]-'a'][k];
                k++;
            }
            outStrPos += k;
        }
        // check for space
        else if (inString[i] == ' ')
        {
            outStr[outStrPos++] = '/';
        }

        // add pause between non-space symbols
        if(inString[i] != ' ' && inString[i+1] != ' ' && inString[i+1] != '\0') 
        {
            outStr[outStrPos++] = ' ';
        }
        i++;
    }
    return outStr;
}

uint8_t convMorse2NumPeriods(char inChar)
{
    if (inChar == '.')
    {
        return 1U;
    }
    else if (inChar == '-')
    {
        return 3U;
    }
    else if (inChar == ' ')
    {
        // subtract 1 period because there already is a pause after each symbol
        return 30U-20U;
    }
    else if (inChar == '/')
    {
        // subtract 1 period because there already is a pause after each symbol
        return 70U-20U;
    }
    
    // make compiler happy
    return 0U;
}