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

    CrmsFile* crms_file = cr_open(0, "message.txt", 'r');

    cr_delete(crms_file);

    cr_close(crms_file);
}
