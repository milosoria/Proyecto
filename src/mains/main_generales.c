#include "../api/crms_API.h"
#include <stdlib.h>
#include <unistd.h>
//#include <byteswap.h>

int main( int argc, char**argv){
    if (argc != 2) {
        printf("The execution needs to have the path to the virtual memory to run\n");
        exit(0);
    }
    int peo;
    int while_check = 1;
    int pid;
    char file_name[20]; 
    char * memory_path  = argv[1];

    printf("\n");
    printf("-- FUNCIONES GENERALES --\n");

    cr_mount(memory_path);
    printf("Mounted memory con string: %s.\n", argv[1]);
    printf("\n");

    printf("¿Go next? (CS_LS_PROCESSES)\n>Press (1):");
    scanf("%d", &peo);
    printf("\n");

    cr_ls_processes();
    printf("\n");
    
    printf("Ahora, queremos probar CR_LS_FILES.\n");
    while (while_check)
    {
        printf("Ingrese un PID\n>PID:");
        scanf("%d", &pid);
        printf("\n");

        printf("Listamos los archivos del proceso con id %d.\n", pid);
        printf("CR_LS_FILES RUNNING: id = %d.\n", pid);
        cr_ls_files(pid);
        printf("\n");

        printf("¿Desea probar CR_LS_FILES denuevo?\n>Sí (1), no (0):");
        scanf("%d", &while_check);
        printf("\n");
    }
    
    printf("Ahora, probaremos CR_EXISTS\n");
    while_check = 1;
    while (while_check)
    {
        printf("Del proceso anterior (pid = %d) ngrese un 'file_name'\n>file_name:", pid);
        scanf("%s", file_name);
        printf("\n");

        printf("CR_EXISTS RUNNING: id = %d, file_name = %s\n", pid, file_name);
        if (cr_exists(pid, file_name)){
            printf("¡Archivo existe!\n");
        } else {
            printf("zsh: Segmentation fault\n");
            sleep(2);
            printf("Broma... el archivo no existe.");
        }
        printf("\n");

        printf("¿Desea probar CR_EXISTS denuevo?\n>Sí (1), no (0):");
        scanf("%d", &while_check);
        printf("\n");

    }

    printf("\t END MAIN\n");
}
