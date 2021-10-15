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
    //cr_ls_files(0);
    //cr_ls_processes();
    print_frame_bit_map();
    cr_start_process(69, "coloro");
    print_page_table(69);
    link_new_page_to_empty_frame(0, 69);
    link_new_page_to_empty_frame(2, 69);
    print_page_table(69);
    print_frame_bit_map();
    //cr_finish_process(0);
    //print_frame_bit_map();
    //cr_ls_processes();
    //CrmsFile* crms_file = cr_open(0,"im_a_mp3.bin", 'r');
    //unsigned int dir = cr_conseguir_dir(crms_file);
    //printf("MAIN: dir: %u\n",dir);
    //char * buffer = "CAMILOSORIA";
    //int result  = cr_write_file(crms_file,"im_a_mp3.bin", 3);

    //cr_read(crms_file, malloc(crms_file->size), crms_file->size);

    //destroy_crms_file(crms_file);
}
