#include "../api/crms_API.h"
#include <stdlib.h>

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
    cr_exists(200,"hecomes.mp4");
    //cr_ls_files(200);
    //cr_ls_processes();
    //cr_start_process(215,"camilo");
    //cr_ls_processes();
    // Crms_open
    CrmsFile* crms_file = cr_open(200,"hecomes.mp4", 'r');

    
    virtual_dir = crms_file -> virtual_dir;
    size = crms_file -> size;
    tabla_paginas = crms_file -> tabla_paginas;

    printf("\t SIZE: %u \n", bswap_32(size));
    printf("\t DIR VIRTUAL: %u \n", bswap_32(virtual_dir));
    printf("\t TABLA DE PÁGINAS \n");
    printf("\t MAIN: el valor de tabla_paginas es: %p.\n", tabla_paginas);

    /*
    for (int i = 0; i < PAGE_TABLE_N_ENTRIES; i++)
    {
        printf("\t Entrada %d: %u\n", i, bswap_32(*(tabla_paginas + i)));
    }
    */
}
