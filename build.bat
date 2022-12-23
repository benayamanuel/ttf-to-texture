@echo off

IF NOT EXIST bin/ mkdir bin && echo bin/ not found, creating directory..

clang main.c -o bin/main.exe -Wno-deprecated && echo Binaries built succesfully.

