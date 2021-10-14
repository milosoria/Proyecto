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
    cr_exists(0,"secret.txt");
    cr_ls_files(0);
    cr_ls_processes();
    CrmsFile* crms_file = cr_open(0,"secret.txt", 'r');

    
    /* printf("\t SIZE: %u \n", bswap_32(size)); */
    //printf("\t DIR VIRTUAL: %u \n", bswap_32(virtual_dir));
    //printf("\t TABLA DE PÁGINAS \n");
    /* cprintf("\t MAIN: el valor de tabla_paginas es: %p.\n", tabla_paginas); */
}
