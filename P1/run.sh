#!/bin/bash

if [ $# -ne 0 ]; then
    if [ $1 == "archivos" ]; then
        cp src/mains/main_archivos.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        if [ $2 == "valgrind" ]; then
            valgrind --leak-check=full --show-leak-kinds=all ./crms memfilled1.bin
        else 
            ./crms memfilled1.bin
        fi
    elif [ $1 == "generales" ]; then
        cp src/mains/main_generales.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        if [ $2 == "valgrind" ]; then
            valgrind --leak-check=yes ./crms memfilled2.bin
        else 
            ./crms memfilled2.bin
        fi
    elif [ $1 == "procesos" ]; then
        cp src/mains/main_procesos.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        if [ $2 == "valgrind" ]; then
            valgrind --leak-check=yes ./crms memfilled3.bin
        else 
            ./crms memfilled3.bin
        fi
    else
        cp src/mains/main_cropen.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        if [ $2 == "valgrind" ]; then
            valgrind --leak-check=yes ./crms memfilled4.bin
        else 
            ./crms memfilled4.bin
        fi
    fi
fi
