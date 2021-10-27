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
    int bytes = sizeof("Vamos por ese 7!")/ sizeof(char);
    printf("Montando memoria\n");
    char * memory_path  = argv[1];
    cr_mount(memory_path);
    int check = 1;
    char file_name[20]; 
    CrmsFile* file;
    char * buff = malloc(bytes*sizeof(char));
    int escritos;
    

    printf("¿Imprimimos los procesos?\n>Press (1):");
    scanf("%i", &opt);
    printf("\n");

    cr_ls_processes();
    printf("\n");

    printf("Comenzando test de CR_OPEN, modo 'w'...\n");
    printf("Probemos abrir un archivo de un proceso no existente\n");
    printf("Ingrese un PID no existente\n>PID:");
    scanf("%i", &pid);
    printf("\n");

    file = cr_open(pid, "notgonnawork.txt", 'w');
    printf("Bien, era de esperarse...\n");

    cr_ls_processes();
    printf("\n");

    printf("Elijamos un proceso existente para testear:\n>PID:");
    scanf("%i", &pid);

    printf("Listamos los archivos del proceso con id %i.\n", pid);
    printf("CR_LS_FILES RUNNING: id = %i.\n", pid);
    cr_ls_files(pid);
    printf("\n");

    while (check)
    {
        printf("Del proceso anterior (pid = %i) ingrese un 'file_name'\n>file name:", pid);
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
            printf("\tDireccion fisica del CrmsFile: %u\n", file->dir);
            printf("\tTamaño del CrmsFile: %u\n", file->size);
            printf("\tBytes leidos del CrmsFile: %u\n", file->bytes_leidos);
            printf("\n");
            printf("¿Imprimimos los archivos del proceso anterior (pid = %i)?\n>Press (1)", pid);
            scanf("%i", &opt);
            cr_ls_files(pid);
        }

        printf("¿Desea probar CR_OPEN denuevo?\n>Sí (1), no (0):");
        scanf("%i", &check);
        printf("\n");
    }
    check = 1;
    
    printf("Ahora, probemos escribir en algun archivo, para eso usaremos CR_WRITE_FILE.\n");
    printf("Para esto, debemos hacer un malloc para guardar en un buffer aquello que escribiremos\n");
    printf("***** CREANDO BUFFER *****\n");
    printf("***** BUFFER CREADO *****\n");
    printf("Listo con el malloc, ¿seguimos? \n>Press (1):");
    scanf("%i",&opt);
    

    printf("Probemos primero si funciona escribir una cantidad pequeña de bytes.\n");
    printf("STATUS: tamaño del archivo: %i, bytes restantes por escribir: %i.\n", file->size, bytes);
    strcpy(buff, "Vamos por ese 7!");
    printf("Procedemos a escribir lo siguiente: '%s'\n",buff);
    printf("\n");

    escritos = cr_write_file(file, buff, bytes);
    printf("\n");
    printf("STATUS: tamaño del archivo: %i, bytes escritos: %i.\n", file->size, escritos);
    sleep(2);
    printf("Comprobemos si fue escrito correctamente\n");

    char * written = malloc(sizeof(char)*bytes);
    int leidos =  cr_read(file, written, bytes);
    printf("Leimos %i bytes y obtuvimos: '%s'\n",leidos,written);
    printf("\n");
    sleep(2);

    printf("¡Muy bien! Funcionó a la perfección. Hasta ahora hemos escrito %i bytes y por lo tanto, el archivo tiene el mismo tamaño %i.\n", escritos, file->size);
    printf("Pero, ¿qué pasa si escribimos una cantidad de bytes lo suficientemente grande de modo que no alcancemos a escribir todo? Probémoslo.\n");
    printf("\n");
    sleep(2);


    printf("¿Cuántos bytes desea escribir?\n>bytes:");
    scanf("%i", &bytes);
    written = realloc(written,sizeof(char)* bytes);
    char * buff2 = malloc(sizeof(char)*bytes);
    printf("\n");

    for (int i = 0 ; i<bytes;i++) {
        buff2[i] = 'a';
    }
    escritos = cr_write_file(file,buff2,bytes);
    printf("\n");
    printf("STATUS: tamaño del archivo: %i, bytes escritos: %i.\n", file->size, escritos);

    printf("\n");
    sleep(2);
    
    printf("Comprobemos si fue escrito correctamente\n");
    leidos = cr_read(file, written, bytes);

    printf("Leimos %i bytes y obtuvimos: '%s'\n",leidos,written);
    printf("\n");
    sleep(2);
    printf("Habiendo terminado con esta pequeña prueba de write_file, liberamos el buffer.\n");
    printf("***** LIBERANDO BUFFER *****\n");
    free(buff);
    free(written);
    free(buff2);
    printf("***** BUFFER LIBERADO *****\n");
    // Cerrar el archivo CR_CLOSE
    printf("También hay que liberar el archivo abierto.\n");
    printf("Para eso, usamos la función CR_CLOSE.\n");
    printf("***** LIBERANDO ARCHIVO *****\n");
    cr_close(file);
    printf("***** ARCHIVO LIBERADO *****\n");
    printf("\n");
}
