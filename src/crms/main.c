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
    cr_ls_processes();
    cr_ls_files(0);
    CrmsFile* new = cr_open(0,"owyes",'w');
    cr_ls_files(0);
    char * buffer = malloc(500);
    char * empty = malloc(500);
    for(int i= 0;i<500;i++){
        buffer[i] = 'c';
    }
    cr_write_file(new,buffer,500);
    cr_read(new,empty,500);
    write_file_real(empty,new);
}
