#! /bin/sh
arm-none-eabi-gcc -Wall Test2/SEGGER_SEMIHOST.h Test2/SEGGER_SEMIHOST_Generic.c Test2/SEGGER_SEMIHOST.c Test2/main.c -lc -lrdimon --specs=rdimon.specs -o main