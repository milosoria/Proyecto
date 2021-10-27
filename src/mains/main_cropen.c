
#include "../api/crms_API.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <byteswap.h>

int main( int argc, char**argv){
    // FILE OPEN - w
    printf("\n");
    printf("-------Probando CR_OPEN en modo 'w'-------\n");
    if (argc != 2) {
        printf("The execution needs to have the path to the virtual memory to run\n");
        exit(0);
    }

    int opt;
    int pid;
    int bytes;
    char * memory_path  = argv[1];
    int check = 1;
    char file_name[20]; 
    CrmsFile* file;
    char * buff;

    

    cr_mount(memory_path);
    printf("¿Imprimimos los procesos?\n>Press (1):");
    scanf("%i", &opt);
    printf("\n");

    cr_ls_processes();
    printf("\n");

    printf("Comenzando test de CR_OPEN, modo 'w'...\n");
    printf("Probemos abrir un archivo de un proceso no existente\n");
    printf("Ingrese un PID no existente\n>PID:");
    scanf("%d", &pid);
    printf("Elija el proceso que quiere testear:\n");

    printf("Ingrese un PID\n>PID:");
    scanf("%d", &pid);
    printf("\n");
    file = cr_open(pid, "notgonnawork.txt", 'w');
    printf("Bien, era de esperarse...\n");

    printf("Elija el proceso que quiere testear:\n");
    scanf("%d", &pid);

    printf("Listamos los archivos del proceso con id %d.\n", pid);
    printf("CR_LS_FILES RUNNING: id = %d.\n", pid);
    cr_ls_files(pid);
    printf("\n");

    while (check)
    {
        printf("Del proceso anterior (pid = %d) ingrese un 'file_name'\n>file name:", pid);
        scanf("%s", file_name);
        printf("\n");

        file = cr_open(pid, file_name, 'w');
        if (!file){
            printf("El archivo ya existe... Probemos con un nombre nuevo la proxima vez\n");
        } else {
            printf("El nuevo archivo ha sido creado con exito y listo para ser utilizado!!!\n");
            printf("\tNombre del CrmsFile: %s\n", file->file_name);
            printf("\tPID del CrmsFile: %u\n", file->process_id);
            printf("\tDireccion Virtual CrmsFile: %u\n", file->virtual_dir);
            printf("\tPFN del CrmsFile: %u\n", file->pfn);
            printf("\tDireccion fisica del CrmsFile: %u\n", file->dir);
            printf("\tTamaño del CrmsFile: %u\n", file->size);
            printf("\tBytes leidos del CrmsFile: %u\n", file->bytes_leidos);
            printf("\n");
            printf("¿Imprimimos los archivos del proceso anterior (pid = %d)?\n>Press (1)", pid);
            scanf("%d", &opt);
            cr_ls_files(pid);
        }

        printf("¿Desea probar CR_OPEN denuevo?\n>Sí (1), no (0):");
        scanf("%d", &check);
        printf("\n");
    }
    check = 1;
    exit(0);
    
    printf("Ahora, probemos escribir en algun archivo, para eso usaremos CR_WRITE_FILE.\n");
    printf("Para esto, debemos hacer un malloc para guardar en un buffer aquello que escribiremos\n");
    printf("***** CREANDO BUFFER *****\n");
    bytes = sizeof("Vamos por ese 7!")/ sizeof(char);
    buff = malloc(bytes);
    printf("***** BUFFER CREADO *****\n");
    printf("Listo con el malloc, ¿seguimos? \n>Press (1):");
    scanf("%i",&opt);
    

    printf("Probemos primero si funciona escribir una cantidad pequeña de bytes.\n");
    printf("STATUS: tamaño del archivo: %i, bytes leídos del archivo (total): %i, bytes restantes por leer: %i.\n", file->size, file->bytes_leidos, file->size - file->bytes_leidos);
    strcpy(buff, "Vamos por ese 7!");
    printf("Procedemos a escribir lo siguiente: '%s'\n",buff);
    printf("\n");

    cr_write_file(file, buff, bytes);
    printf("\n");
    printf("STATUS: tamaño del archivo: %i, bytes leídos del archivo (total): %i, bytes restantes por leer: %i.\n", file->size, file->bytes_leidos, file->size - file->bytes_leidos);

    printf("¡Muy bien! Funcionó a la perfección. Hasta ahora hemos escrito %i bytes y por lo tanto, el archivo tiene el mismo tamaño %i.\n", file->bytes_leidos, file->size);
    printf("Pero, ¿qué pasa si escribimos una cantidad de bytes lo suficientemente grande de modo que no alcancemos a escribir todo? Probémoslo.\n");

    while (check)
    {
        printf("¿Cuántos bytes desea escribir?\n>bytes:");
        scanf("%d", &bytes);
        printf("\n");

        char * buff2 = malloc(bytes);
        for (int i = 0 ; i<bytes;bytes++) {
            buff2[i] = 'a';
        }
        printf("\n");
        printf("STATUS: tamaño del archivo: %i, bytes leídos del archivo (total): %i, bytes restantes por leer: %i.\n", file->size, file->bytes_leidos, file->size - file->bytes_leidos);

        printf("¿Desea ingresar otra cantidad de bytes por leer?\n>Sí (1), no (0):");
        scanf("%d", &check);
        printf("\n");
    }
    check = 1;
    /* printf("Habiendo terminado con esta pequeña prueba de read, liberamos el buffer.\n"); */
    /* printf("***** LIBERANDO BUFFER *****\n"); */
    /* free(buff); */
    /* printf("***** BUFFER LIBERADO *****\n"); */
    /* // Cerrar el archivo CR_CLOSE */
    /* printf("También hay que liberar el archivo abierto.\n"); */
    /* printf("Para eso, usamos la función CR_CLOSE.\n"); */
    /* printf("***** LIBERANDO ARCHIVO *****\n"); */
    /* cr_close(file); */
    /* printf("***** ARCHIVO LIBERADO *****\n"); */
    /* printf("\n"); */

    /* // Grandes ligas */
    /* printf("Ahora, vamos a las grandes ligas. Ese archivo era chico, de no más de una frame, ¿qué pasa si probamos con uno de más de 8MB?\n"); */
    /* printf(">Press (1):"); */
    /* scanf("%i", &peo); */
    /* printf("\n"); */
    

    /* while(check){ */
    /*     printf("Vamos para atrás, elija el proceso que quiere testear:\n"); */
    /*     cr_ls_processes(); */
    /*     printf("\n"); */
    /*     printf("Ingrese un PID\n>PID:"); */
    /*     scanf("%d", &pid); */
    /*     printf("\n"); */

    /*     printf("Listamos los archivos del proceso con id %d.\n", pid); */
    /*     printf("CR_LS_FILES RUNNING: id = %d.\n", pid); */
    /*     cr_ls_files(pid); */
    /*     printf("\n"); */

    /*     printf("Del proceso anterior (pid = %d) ingrese un 'file_name'\n>file name:", pid); */
    /*     scanf("%s", file_name); */
    /*     printf("\n"); */
    /*     file = cr_open(pid, file_name, 'r'); */
    /*     printf("\n"); */

    /*     printf("¡Enhorabuena! Se pudo abrir el archivo %s de tamaño %i bytes.\n", file->file_name, file->size); */
    /*     printf("¿Desea elegir otro archivo de otro proceso?\n>Sí (1), no (0):"); */
    /*     scanf("%d", &check); */
    /*     printf("\n"); */
    /* } */
    /* check = 1; */
    

    /* printf("Notamos que su tamaño en bytes es: %i, más de 8MB (=8388608 bytes).\n", file->size); */
    /* printf("Creamos el buffer que contenga estos datos:\n"); */
    /* printf("***** CREANDO BUFFER *****\n"); */
    /* buff = malloc(file->size); */
    /* printf("***** BUFFER CREADO *****\n"); */
    /* printf("Tenemos el buffer, ¿continue?\n>Press (1):"); */
    /* scanf("%i", &peo); */
    /* printf("\n"); */

    /* printf("Ahora, digamos que queremos leerlo entero, es decir CR_READ recibirá como input:\n"); */
    /* printf("\t1. El archivo en sí: 'file'\n"); */
    /* printf("\t2. El buffer: 'malloc(file->size)'\n"); */
    /* printf("\t3. La cantidad de bytes por leer: 'file->size'\n"); */
    /* printf("¿Le damos? (Puede que demore en recorrer todo el archivo) \n>Press (1):"); */
    /* scanf("%i", &peo); */
    /* cr_read(file, buff, file->size); */
    /* printf("\n"); */

    /* printf("¡Excelente! Ahora tenemos la TOTALIDAD del archivo guardada en el buffer.\n"); */
    /* printf("¿Podemos acceder a esa información?\n"); */
    /* sleep(5); */
    /* printf("¡Claro que yes!\n"); */
    /* sleep(3); */
    /* printf("Implementamos una función que recibe el 'buffer' y el 'crms_file' y e imprime los datos del buffer en un archivo real, ubicado en la carpeta ROOT del proyecto.\n"); */
    /* printf("¿Shall we?\n>Press (1):"); */
    /* scanf("%i", &peo); */
    /* write_file_real(buff, file); */

    /* printf("¡Eureka! Vemos que el archivo con nombre: %s, ha sido creado en nuestro directorio.\n", file->file_name); */
    /* printf("¡Echémosle un ojo!\n"); */
    /* printf("Lo abriste?\n>Press (1):"); */
    /* scanf("%i", &peo); */
    /* write_file_real(buff, file); */


    /* // Free todo. */
    /* printf("No olvidar, hay que 'free' el buffer y cerrar el archivo (usando CR_CLOSE).\n"); */
    /* printf("***** LIBERANDO BUFFER *****\n"); */
    /* free(buff); */
    /* printf("***** BUFFER LIBERADO *****\n"); */
    /* printf("***** LIBERANDO ARCHIVO *****\n"); */
    /* cr_close(file); */
    /* printf("***** ARCHIVO LIBERADO *****\n"); */
    /* printf("\n"); */

    /* printf("\n"); */
    /* printf("¡Listo con CR_OPEN, modo 'r'!\n"); */
    /* printf("\n"); */

}
