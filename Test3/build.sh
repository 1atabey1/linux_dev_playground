#!/bin/bash

gcc -fdiagnostics-color=always -g -Wall ./src/testpkgmodule.c -o testpkg $(python3-config --embed --cflags) $(python3-config --embed --ldflags)