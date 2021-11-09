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
    int check = 1;
    int pid;
    char * file_name = calloc(20,sizeof(char));
    char * memory_path  = argv[1];
    int bytes;
    char* buff;
    CrmsFile* file;

    printf("\n");
    printf("-- FUNCIONES ARCHIVOS --\n");
    printf("\n");

    cr_mount(memory_path);
    printf("Mounted memory con string: %s.\n", argv[1]);
    printf("\n");

    printf("¿Imprimimos los procesos?\n>Press (1):");
    scanf("%i", &peo);
    printf("\n");

    cr_ls_processes();
    printf("\n");
    
    printf("Elija el proceso que quiere testear:\n");
    printf("Ingrese un PID\n>PID:");
    scanf("%d", &pid);
    printf("\n");

    printf("Listamos los archivos del proceso con id %d.\n", pid);
    printf("CR_LS_FILES RUNNING: id = %d.\n", pid);
    cr_ls_files(pid);
    printf("\n");

    // FILE OPEN - r
    printf("Primero, probaremos la función CR_OPEN, modo 'r'.\n");
    while (check)
    {
        printf("Del proceso anterior (pid = %d) ingrese un 'file_name'\n>file name:", pid);
        scanf("%s", file_name);
        printf("\n");

        file = cr_open(pid, file_name, 'r');

        printf("¿Desea probar CR_OPEN denuevo?\n>Sí (1), no (0):");
        scanf("%d", &check);
        printf("\n");
    }
    check = 1;

    printf("¡Enhorabuena! Se pudo abrir el archivo %s.\n", file->file_name);
    printf("Ahora, tenemos que leerlo, para eso usaremos CR_READ.\n");
    printf("Para eso, debemos hacer un malloc para crear el buffer en donde vamos a guardar nuestra información\n");
    printf("***** CREANDO BUFFER *****\n");
    buff = malloc(file->size);
    printf("***** BUFFER CREADO *****\n");
    printf("Listo con el malloc, ¿seguimos?\n>Press (1):");
    scanf("%i", &peo);
    printf("\n");
    

    printf("Probemos primero si funciona leer una cantidad de bytes menor al tamaño del archivo.\n");
    printf("STATUS: tamaño del archivo: %i, bytes leídos del archivo (total): %i, bytes restantes por leer: %i.\n", file->size, file->bytes_leidos, file->size - file->bytes_leidos);
    while (check)
    {
        printf("¿Cuántos bytes desea leer?\n>bytes:");
        scanf("%d", &bytes);
        printf("\n");

        cr_read(file, buff, bytes);
        printf("\n");
        printf("STATUS: tamaño del archivo: %i, bytes leídos del archivo (total): %i, bytes restantes por leer: %i.\n", file->size, file->bytes_leidos, file->size - file->bytes_leidos);

        printf("¿Desea ingresar otra cantidad de bytes por leer?\n>Sí (1), no (0):");
        scanf("%d", &check);
        printf("\n");
    }
    check = 1;

    printf("¡Muy bien! Funcionó a la perfección. Hasta ahora hemos leído %i bytes de los %i que posee el archivo. Quedan %i bytes por leer.\n", file->bytes_leidos, file->size, file->size - file->bytes_leidos);
    printf("Pero, ¿qué pasa si leemos una cantidad de bytes que excede la capacidad del archivo? Probémoslo.\n");

    while (check)
    {
        printf("¿Cuántos bytes desea leer?\n>bytes:");
        scanf("%d", &bytes);
        printf("\n");

        cr_read(file, buff, bytes);
        printf("\n");
        printf("STATUS: tamaño del archivo: %i, bytes leídos del archivo (total): %i, bytes restantes por leer: %i.\n", file->size, file->bytes_leidos, file->size - file->bytes_leidos);

        printf("¿Desea ingresar otra cantidad de bytes por leer?\n>Sí (1), no (0):");
        scanf("%d", &check);
        printf("\n");
    }
    check = 1;
    printf("Habiendo terminado con esta pequeña prueba de read, liberamos el buffer.\n");
    printf("***** LIBERANDO BUFFER *****\n");
    free(buff);
    printf("***** BUFFER LIBERADO *****\n");
    // Cerrar el archivo CR_CLOSE
    printf("También hay que liberar el archivo abierto.\n");
    printf("Para eso, usamos la función CR_CLOSE.\n");
    printf("***** LIBERANDO ARCHIVO *****\n");
    cr_close(file);
    printf("***** ARCHIVO LIBERADO *****\n");
    printf("\n");

    // Grandes ligas
    printf("Ahora, vamos a las grandes ligas. Ese archivo era chico, de no más de una frame, ¿qué pasa si probamos con uno de más de 8MB?\n");
    printf(">Press (1):");
    scanf("%i", &peo);
    printf("\n");
    

    while(check){
        printf("Vamos para atrás, elija el proceso que quiere testear:\n");
        cr_ls_processes();
        printf("\n");
        printf("Ingrese un PID\n>PID:");
        scanf("%d", &pid);
        printf("\n");

        printf("Listamos los archivos del proceso con id %d.\n", pid);
        printf("CR_LS_FILES RUNNING: id = %d.\n", pid);
        cr_ls_files(pid);
        printf("\n");

        printf("Del proceso anterior (pid = %d) ingrese un 'file_name'\n>file name:", pid);
        scanf("%s", file_name);
        printf("\n");
        file = cr_open(pid, file_name, 'r');
        printf("\n");

        printf("¡Enhorabuena! Se pudo abrir el archivo %s de tamaño %i bytes.\n", file->file_name, file->size);
        printf("¿Desea elegir otro archivo de otro proceso?\n>Sí (1), no (0):");
        scanf("%d", &check);
        printf("\n");
    }
    check = 1;
    

    printf("Notamos que su tamaño en bytes es: %i, más de 8MB (=8388608 bytes).\n", file->size);
    printf("Creamos el buffer que contenga estos datos:\n");
    printf("***** CREANDO BUFFER *****\n");
    buff = malloc(file->size);
    printf("***** BUFFER CREADO *****\n");
    printf("Tenemos el buffer, ¿continue?\n>Press (1):");
    scanf("%i", &peo);
    printf("\n");

    printf("Ahora, digamos que queremos leerlo entero, es decir CR_READ recibirá como input:\n");
    printf("\t1. El archivo en sí: 'file'\n");
    printf("\t2. El buffer: 'malloc(file->size)'\n");
    printf("\t3. La cantidad de bytes por leer: 'file->size'\n");
    printf("¿Le damos? (Puede que demore en recorrer todo el archivo) \n>Press (1):");
    scanf("%i", &peo);
    cr_read(file, buff, file->size);
    printf("\n");

    printf("¡Excelente! Ahora tenemos la TOTALIDAD del archivo guardada en el buffer.\n");
    printf("¿Podemos acceder a esa información?\n");
    sleep(5);
    printf("¡Claro que yes!\n");
    sleep(3);
    printf("Implementamos una función que recibe el 'buffer' y el 'crms_file' y e imprime los datos del buffer en un archivo real, ubicado en la carpeta ROOT del proyecto.\n");
    printf("¿Shall we?\n>Press (1):");
    scanf("%i", &peo);
    write_file_real(buff, file);

    printf("¡Eureka! Vemos que el archivo con nombre: %s, ha sido creado en nuestro directorio.\n", file->file_name);
    printf("¡Echémosle un ojo!\n");
    printf("Lo abriste?\n>Press (1):");
    scanf("%i", &peo);
    write_file_real(buff, file);


    // Free todo.
    printf("No olvidar, hay que 'free' el buffer y cerrar el archivo (usando CR_CLOSE).\n");
    printf("***** LIBERANDO BUFFER *****\n");
    free(buff);
    free(file_name);
    printf("***** BUFFER LIBERADO *****\n");
    printf("***** LIBERANDO ARCHIVO *****\n");
    cr_close(file);
    printf("***** ARCHIVO LIBERADO *****\n");
    printf("\n");

    printf("\n");
    printf("¡Listo con CR_OPEN, modo 'r'!\n");
    printf("\n");

}
