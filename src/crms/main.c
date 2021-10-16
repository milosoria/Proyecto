#include "../api/crms_API.h"
#include <stdlib.h>
//#include <byteswap.h>

int main( int argc, char**argv){
    if (argc != 2) {
        printf("The execution needs to have the path to the virtual memory to run\n");
        exit(0);
    }

    char * memory_path  = argv[1];
    
    cr_mount(memory_path);

    cr_start_process(69, "coloro");
    print_page_table(69);
    CrmsFile * crms_file = cr_open(69, "ooweee.txt", 'w');
    print_page_table(69);
    //print_frame_bit_map();
    //print_page_table(crms_file -> process_id);

    cr_delete(crms_file);
    print_page_table(69);

    //print_frame_bit_map();
    //print_page_table(crms_file -> process_id);

    cr_close(crms_file);
}
