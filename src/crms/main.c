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
    //cr_exists(0,"im_a_mp3.bin");
    // cr_ls_files(0);
    //cr_ls_processes();
    //cr_start_process(69, "coloro");
    //cr_ls_processes();
    CrmsFile* crms_file = cr_open(0,"message.txt", 'r');

    // Los mallocs de buffer SIEMPRE se harán del tamaño total del archivo
    char* entero = malloc(crms_file -> size);
    char* parcial = malloc(crms_file -> size);

    cr_read(crms_file, entero, crms_file -> size);
    cr_read(crms_file, parcial, 1000);
    printf("\n -------------- OUTPUT --------------\n");
    // for (int i = 0; i < crms_file -> size; i++)
    // {
    //     printf("%c", parcial[i]);
    // }
    
    write_file_real(entero, crms_file);
    free(entero);
    free(parcial);
    cr_close(crms_file);
}
