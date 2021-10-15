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
    // cr_exists(0,"secret.txt");
    // cr_ls_files(0);
    cr_ls_processes();
    cr_start_process(69, "coloro");
    cr_ls_processes();
    cr_finish_process(69);
    cr_ls_processes();
    //CrmsFile* crms_file = cr_open(0,"secret.txt", 'r');
    
    //cr_read(crms_file, malloc(crms_file->size), crms_file->size);

    //destroy_crms_file(crms_file);
}
