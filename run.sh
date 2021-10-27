#!/bin/bash

if [ $# -ne 0 ]; then
    if [ $1 == "archivos" ]; then
        cp src/mains/main_archivos.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        ./crms memfilled1.bin
    elif [ $1 == "generales" ]; then
        cp src/mains/main_generales.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        ./crms memfilled2.bin
    elif [ $1 == "procesos" ]; then
        cp src/mains/main_procesos.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        ./crms memfilled3.bin
    else
        cp src/mains/main_cropen.c src/crms/main.c
        wait
        make
        wait
        printf "\nEmpezando tests"
        ./crms memfilled4.bin
    fi
fi
