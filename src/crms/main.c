#include "../api/crms_API.h"
#include <stdlib.h>

int main( int argc, char**argv){
    if (argc != 2) {
        printf("The execution needs to have the path to the virtual memory to run\n");
        exit(0);
    }

    char * memory_path  = argv[1];
    cr_mount(memory_path);
    cr_ls_processes();
    cr_exists(32,"secret.txt");
}
