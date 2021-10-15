#include "../api/crms_API.h"
#include <stdlib.h>
//#include <byteswap.h>

int main( int argc, char**argv){
    if (argc != 2) {
        printf("The execution needs to have the path to the virtual memory to run\n");
        exit(0);
    }

    unsigned int* tabla_paginas;
    char * memory_path  = argv[1];
    unsigned int virtual_dir;
    unsigned int size;
    
    cr_mount(memory_path);
    // Los mallocs de buffer SIEMPRE se harán del tamaño total del archivo
    //char* entero = malloc(crms_file -> size);
    //char* parcial = malloc(crms_file -> size);
    //free(entero);
    //free(parcial);
    //cr_close(crms_file);
}
