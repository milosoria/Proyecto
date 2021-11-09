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

    int state = 1;

    while (state == 1)
    {
        int mode;
        printf("1 -> Start a process\n2 -> Finish a process\n3 -> List processes\n0 -> End this madness\n\nEnter an integer: ");
        scanf("%i", &mode);

        if (mode == 1)
        {
            int pid;
            printf("\nUtiliza tu imaginacion:\n\n\tID ---> ");
            scanf("%i", &pid);

            char * str =  calloc(20,sizeof(char)); 
            printf("\tName -> ");
            scanf("%s", str);

            printf("\n");
            cr_start_process(pid, str);

            free(str);
        }

        else if (mode == 2)
        {
            int show;

            printf("1 -> Show frame bitmap\nOther -> Don't\n\nEnter an integer: ");
            scanf("%i", &show);

            printf("\nTienes el poder de Thanos para eliminar de la faz de memfilled.bin un proceso\n¿Que proceso deseas eliminar\nID: ");

            int id;
            scanf("%i", &id);

            if(show == 1){
                print_frame_bit_map();
            }

            cr_finish_process(id);

            if(show == 1){
                print_frame_bit_map();
            }
        }

        else if(mode == 3){
            printf("\nProcesos que viven en la memoria:\n\n");

            cr_ls_processes();
        }

        else if (mode == 0)
        {
            state --;
            break;
        }

        else
        {
            printf("Ups, no se que hacer con ese input\n\n");
        }
        int ch;
        printf("Enter 1 and press ENTER to continue\n");
        scanf("%i",&ch);

        printf("\n\n\n-------- Pedido listo --------\n\n\n");
        
    }

    printf("\nGRACIAS TOTALES\n");
    return 0;

}
