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

    char * buffer = malloc(20);
    char * empty = malloc(20);

    for(int i= 0;i<20;i++){
        buffer[i] = 0x01;
    }

    cr_write_file(new,buffer,20);
    cr_read(new,empty,20);
    cr_ls_files(0);

    for(int i= 0;i<20;i++){
        printf("char 0x%02x\n",empty[i]);
    }

}
