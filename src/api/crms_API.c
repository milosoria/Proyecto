#include "crms_API.h"
//#include <byteswap.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <math.h>

// esta bien hacerlo asi?
char * MEMORY_PATH;

// Funciones para manejar struct
CrmsFile * init_crms_file(unsigned int virtual_dir, unsigned int process_id, unsigned int size, char *file_name){
    /* crmsfile representa un archivo */
    /* si miras la función cr_open, esta recibe el id del proceso, el nombre del archivo y el modo */
    /* lo que te retorna es la representación de un archivo asociado a un proceso y la gracia de esta representación es que tenga
     * toda la información que estimes necesaria para que el resto de las funciones de la API que lo recibirán como 
     argumento puedan funcionar bien */
    CrmsFile * crms = malloc(sizeof(CrmsFile));
    crms -> process_id = process_id;
    crms -> virtual_dir = virtual_dir;
    crms -> size = size;
    // Los siguientes parten en 0, si inicializan con cr_conseguir_dir
    crms -> dir = 0;
    // Este se va llenando en cr_read.
    crms -> bytes_leidos = 0;

    crms -> file_name = strdup(file_name);
    return crms;
}

// Funciones Generales
void cr_mount(char * memory_path){
    MEMORY_PATH = memory_path;
}

void cr_ls_processes(){
    printf("CR_LS_PROCESSES RUNNING\n");
    FILE * memory = fopen(MEMORY_PATH, "rb");
    unsigned char process_name[NAMES_SIZE];
    unsigned char process_state;
    unsigned int process_id_uint;
    for (int i=0; i < PCB_N_ENTRIES; i++){
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        process_id_uint = fgetc(memory);
        // obtenemos el nombre del proceso
        fread(process_name,NAMES_SIZE,1,memory);
        if (process_state == (unsigned char)0x01){
            // obtenemos el pid
            printf("\t[PID:%i] Process: %s state 0x%02x\n", (unsigned int)process_id_uint,process_name,process_state);
        }
        fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
    }
    fclose(memory);
}

void cr_ls_files(int process_id){
    printf("CR_ls_files RUNNING\n");
    FILE * memory = fopen(MEMORY_PATH, "rb");

    unsigned char process_file[NAMES_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
    unsigned char process_state;

    for (int i=0; i < PCB_N_ENTRIES; i++){

        // estado del proceso
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        // id del proceso 
        process_id_uint = fgetc(memory);
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);

        // si el proceso esta cargado y tiene id igual al buscado
        if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id){

            printf("\t[PID:%u] FOUND\n", process_id_uint);
            // recorremos las entradas de archivos
            for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                // estado del archivo
                fread(&file_state,1,1,memory);
                // nombre del archivo
                fread(process_file,NAMES_SIZE,1,memory); 
                // si el archivo esta cargado
                if (file_state ==  0x01){
                    printf("\tFILE %s STATE 0x%02x\n",process_file,file_state);
                }
                // dejamos el puntero en la siguiente entrada si es que quedan entradas
                fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR); 
            }
            // Si no encontramos el archivo, retornamos 0
            fclose(memory);
            return;
        } else {
            // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);

}

int cr_exists(int process_id, char * file_name){
    printf("CR_EXISTS RUNNING\n");
    FILE * memory = fopen(MEMORY_PATH, "rb");

    unsigned char process_file[NAMES_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
    unsigned char process_state;

    for (int i=0; i < PCB_N_ENTRIES; i++){

        // estado del proceso
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        // id del proceso 
        process_id_uint = fgetc(memory);
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);

        // si el proceso esta cargado y tiene id igual al buscado
        if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id){

            printf("\t[PID:%u] FOUND\n", process_id_uint);
            // recorremos las entradas de archivos
            for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                // estado del archivo
                fread(&file_state,1,1,memory);
                // nombre del archivo
                fread(process_file,NAMES_SIZE,1,memory); 
                // si el archivo esta cargado
                if (file_state ==  0x01){
                    printf("\tFILE %s STATE 0x%02x\n",process_file,file_state);
                    // comprobamos si es el que buscamos
                    if (strcmp((char *) process_file,file_name) == 0){
                        printf("\tFILE FOUND %s\n", process_file);
                        // encontramos el archivo, entonces retornamos 1
                        fclose(memory);
                        return 1;
                    }
                }
                // dejamos el puntero en la siguiente entrada si es que quedan entradas
                fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR); 
            }
            // Si no encontramos el archivo, retornamos 0
            fclose(memory);
            return 0;
        } else {
            // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
    return 0;
}

// Funciones Procesos
void cr_start_process(int process_id, char * process_name){

    printf("CR_START_PROCESS RUNNING\n");
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    unsigned char process_state;
    unsigned char init_state = 0x01;
    unsigned char init_valid = 0x00;

    for (int i=0; i < PCB_N_ENTRIES; i++){
        fread(&process_state,PROCESS_STATE_SIZE,1,memory); 
        // buscamos la primera entrada disponible
        if (process_state == (unsigned char)0x00){
            // encontramos una entrada vacia en la pcb
            printf("\tFOUND AN EMPTY ENTRY\n");
            // Nos devolvemos un byte, para sobrescribir el estado
            fseek(memory,-1L,SEEK_CUR);
            fwrite(&init_state, sizeof(unsigned char), 1, memory);
            fwrite(&process_id, sizeof(unsigned char), 1, memory);


            size_t element_size = sizeof *process_name;

            fwrite(process_name, element_size, NAMES_SIZE, memory);

            printf("\tWRITING INFO NAME:%s AND PID:%i\n", process_name,process_id);
            for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                //Cambiamos los bytes de validez de los archivos a 0x00
                fwrite(&init_valid, sizeof(unsigned char), 1, memory);
                //Pasamos a la siguiente entrada de archivos
                // Hay que saltarse Nombre + Tamaño + Direccion
                fseek(memory,NAMES_SIZE+VIRTUAL_ADRESS_SIZE+PROCESS_FILE_SIZE,SEEK_CUR);
            }
            //Estamos en la tabla de paginas, debemos dejar bits de validez en 0
            unsigned char byte_prueba;
            for (int n=0; n < PAGE_TABLE_N_ENTRIES; n++){
                //Cada entrada es de un byte, donode el primer bit debe ser 0 (bit de validez)
                //y los siguientes 7 bits deben ser el PFN (que se obtiene del frame bit map)
                byte_prueba = fgetc(memory);
                if (byte_prueba > 127)
                {
                    byte_prueba = byte_prueba - 128;
                }
                // Nos devolvemos un byte, para sobrescribir el byte
                fseek(memory,-1L,SEEK_CUR);
                fwrite(&byte_prueba, 1, 1, memory);
            }
            fclose(memory);
            return;
        }
        fseek(memory,PROCESS_ID_SIZE+NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
    }
    fclose(memory);
}

void cr_finish_process(int process_id){

    printf("CR_FINISH_PROCESS RUNNING\n");
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    unsigned char process_state;
    unsigned char process_id_uint;
    unsigned char init_valid = 0x00;
    unsigned char page_table_entry;

    for (int i=0; i < PCB_N_ENTRIES; i++){
        fread(&process_state,PROCESS_STATE_SIZE,1,memory); 
        // buscamos en los procesos activos
        if (process_state == (unsigned char)0x01){
            // Obtenemos el pid
            fread(&process_id_uint, PROCESS_ID_SIZE, 1, memory);
            if (process_id_uint == (unsigned char)process_id){
                printf("\tFINISHING PROCESS ID: %i\n", process_id);
                //Retrocedo 2 para llegar al bit de validez y lo cambio a 0
                fseek(memory,-1L,SEEK_CUR);
                fseek(memory,-1L,SEEK_CUR);
                //Cambiamos el byte de validez a 0x00
                fwrite(&init_valid, sizeof(unsigned char), 1, memory);
                //Avanzamos a la page table
                fseek(memory,PROCESS_ID_SIZE+NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE,SEEK_CUR);
                //Leemos cada entrada del page table, 
                printf("\tSe liberan los siguientes frames:\n");
                for (int k=0; k<PAGE_TABLE_N_ENTRIES; k++){
                    fread(&page_table_entry, PAGE_TABLE_ENTRY_SIZE, 1, memory);
                    if (page_table_entry > 127){
                        //Si tiene bit de validez igual a 1 sabemos que tiene un frame asociado
                        page_table_entry = page_table_entry - 128;
                        printf("\t\tEntry #%i - PFN: %i\n", k, page_table_entry);
                        change_frame_bit_map(page_table_entry);
                    }
                }
                fclose(memory);
                return;
            }
            fseek(memory,NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
        else
        {
            fseek(memory,PROCESS_ID_SIZE+NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
}

// Funciones Archivos
CrmsFile* cr_open(int process_id, char* file_name, char mode){
    printf("CR_OPEN RUNNING\n");

    int exists = cr_exists(process_id, file_name);

    printf("CR_EXISTS ENDS\n");

    if (exists){
        if (mode == 'r'){
            CrmsFile* crms_file;
            FILE * memory = fopen(MEMORY_PATH, "rb");
            unsigned char process_file[NAMES_SIZE];
            unsigned int process_id_uint;
            unsigned char file_state;
            unsigned char process_state;

            unsigned int file_size;
            unsigned int file_va;

            for (int i=0; i < PCB_N_ENTRIES; i++){
                // estado del proceso
                fread(&process_state,PROCESS_STATE_SIZE,1,memory);
                // id del proceso 
                process_id_uint = fgetc(memory);
                // saltamos su nombre
                fseek(memory,NAMES_SIZE,SEEK_CUR);
                // si el proceso esta cargado y tiene id igual al buscado
                if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id){
                    printf("\t[PID:%u] FOUND\n", process_id_uint);
                    // recorremos las entradas de archivos
                    for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                        // estado del archivo
                        fread(&file_state,1,1,memory);
                        // nombre del archivo
                        fread(process_file,NAMES_SIZE,1,memory);
                        // si el archivo esta cargado
                        if (file_state ==  0x01 && strcmp((char *) process_file,file_name) == 0){
                            printf("\tFILE FOUND %s\n", process_file);
                            // comprobamos si es el que buscamos
                            fread(&file_size,PROCESS_FILE_SIZE,1,memory);
                            file_size = (unsigned int) bswap_32(file_size);

                            fread(&file_va,VIRTUAL_ADRESS_SIZE,1,memory);
                            file_va = (unsigned int) bswap_32(file_va);
                            // le sumamos todos los bytes de las subentradas de archivos 10 subentradas * 21 bytes por subentrada.
                            printf("\tSIZE: %u\n", file_size);
                            printf("\tVA: %u\n", file_va);

                            crms_file = init_crms_file(file_va, process_id_uint, file_size, file_name);
                            fclose(memory);
                            return crms_file;
                        } else {
                            // dejamos el puntero en la siguiente entrada si es que quedan entradas
                            fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR);
                        }

                    }
                    // Si no encontramos el archivo, retornamos ERROR
                    fclose(memory);
                    printf("ERROR: el archivo por leer no existe.\n");
                    return NULL;
                } else {
                    // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
                    fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
                }
            }
            // Si no encontramos el proceso, retornamos ERROR
            fclose(memory);
            printf("ERROR: el proceso por leer no existe.\n");
            return NULL;

        } else if (mode == 'w') {
            // error pq el archivo ya exite
            printf("ERROR: se está creando un archivo que ya existe.\n");
            return NULL;
        }
    } else {
        if (mode == 'r'){
            // error pq el archivo no exite
            printf("ERROR: se está leyendo un archivo que no existe.\n");
            return NULL;
        } else if (mode == 'w') {
            printf("\tENTRANDO MODE W\n");
            CrmsFile* crms_file;
            FILE * memory = fopen(MEMORY_PATH, "r+b");
            unsigned int process_id_uint;
            unsigned char file_state;
            unsigned char process_state;
            unsigned int max_virtual_dir = 0;

            int found = 0;
            unsigned int file_size;
            unsigned int file_va;
            int moves = 0;
            int pcbs = 0;
            int moves_real;

            for (int i=0; i < PCB_N_ENTRIES; i++){
                // estado del proceso
                fread(&process_state,PROCESS_STATE_SIZE,1,memory);
                // id del proceso 
                process_id_uint = fgetc(memory);
                // saltamos su nombre
                fseek(memory,NAMES_SIZE,SEEK_CUR);
                // si el proceso esta cargado y tiene id igual al buscado
                if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id){
                    printf("\t[PID:%u] FOUND\n", process_id_uint);
                    // recorremos las entradas de archivos
                    for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                        // estado del archivo
                        fread(&file_state,1,1,memory);
                        fseek(memory,NAMES_SIZE,SEEK_CUR);
                        // nombre del archivo
                        // si el archivo esta cargado

                        if (file_state ==  0x01){

                            fread(&file_size,PROCESS_FILE_SIZE,1,memory);
                            file_size = (unsigned int) bswap_32(file_size);

                            fread(&file_va,VIRTUAL_ADRESS_SIZE,1,memory);
                            file_va = (unsigned int) bswap_32(file_va);

                            if (file_va > max_virtual_dir){
                                max_virtual_dir =file_va;
                            }

                        } else {
                            if (!found){
                                moves_real = ( pcbs-1)*PCB_ENTRY_SIZE+moves*PROCESS_FILE_ENTRY_SIZE+NAMES_SIZE+PROCESS_FILE_SIZE+1;
                                fseek(memory,-NAMES_SIZE,SEEK_CUR);
                                fseek(memory,-1L,SEEK_CUR);
                                unsigned int init = 0x01;
                                unsigned int size = 0;
                                fwrite(&init, 1, 1,memory);
                                fwrite(file_name, NAMES_SIZE, 1,memory);
                                fwrite(&size, PROCESS_FILE_SIZE,1,memory);
                                fseek(memory,VIRTUAL_ADRESS_SIZE,SEEK_CUR);
                                found = 1;
                            }else{
                                fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR);
                            }
                            // dejamos el puntero en la siguiente entrada si es que quedan entradas
                        }
                        moves ++;
                    }
                    // max_virtual_dir + 1 < size mem virtual
                    max_virtual_dir++;
                    crms_file = init_crms_file(max_virtual_dir, process_id_uint, 0, file_name);
                    fseek(memory,moves_real,SEEK_SET);
                    fwrite(&max_virtual_dir,VIRTUAL_ADRESS_SIZE,1,memory);
                    fclose(memory);
                    return crms_file;
                } else {
                    // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
                    fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
                }
                pcbs ++;
            }
            // Si no encontramos el proceso, retornamos ERROR
            fclose(memory);
            printf("ERROR: no se encontro e.\n");
            return NULL;
        }
    }
    return NULL;
}

/* 1.Lees de entrada en entrada del proceso (mediante el uso de fseek y fread) */
/* 2.Una vez encuentras el proceso buscas entrada por entrada el archivo que estas buscando (de nuevo, fseek y fread) */
/* 3.Una vez encuentras el archivo mediante el VPN y el tamaño del archivo sabes exactamente que paginas usa el archivo */
/* 5.Lees pagina por pagina del archivo */
/* 6.Por cada pagina perteneciente al archivo con el PFN obtienes exactamente el frame y al que pertenece la pagina. */
/* 7.Entre el VPN, y el PFN de cada pagina sabes cuales frames leer y cuanto leer. */


int cr_write_file(CrmsFile* file_desc, void * buffer, int n_bytes){
    // IMPORTANTE: CABE RECALCAR QUE LOS ARCHIVOS PARTEN CON SIZE 0 Y LUEGO SU SIZE CRECE A MEDIDA QUE SE ESCRIBE.
    // TODO: 
    //   - Identificar si existe otro archivo en las proximas direcciones, "cortando" la memoria continua

    printf("CR_WRITE_FILE RUNNING\n");

    unsigned int dir_actual= file_desc->dir; 
    unsigned int offset;
    unsigned int pfn;
    unsigned char vpn = va_vpn(file_desc->virtual_dir);

    printf("\tDIR INICIAL: %u\n", dir_actual);

    printf("\tBytes por leer en esta llamada: %d.\n", n_bytes);
    printf("\t-- BEGIN FOR --\n");
    printf("\t%i size\n",file_desc->size);

    // Abrimos el archivo de memoria
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    // Tenemos que leer la cantidad de bytes.
    for (int i = 1; i < n_bytes + 1; i++)
    {
        // Encontramos la nueva dir_actual.
        cr_conseguir_dir(file_desc,'w');
        dir_actual = file_desc -> dir;
        printf("\t DIR IN FOR %u\n", dir_actual);
        // funcion para obtener offset
        offset = get_offset(dir_actual);

        if (offset == 0){
            // verificar que la pagina este vacia
            // El proceso comienza en una nueva pagina o nos cambiamos a una nueva pagina despues de escribir
            // conectamos un nuevo frame
            pfn = link_new_page_to_empty_frame(vpn, file_desc->process_id);
            if (pfn ==0){
                // CASOS DE TERMINO:
                /* No quedan frames disponibles para continuar, o */
                /* - Si no hay frames disponibles, entonces retornamos file_desc -> size */ 
                return file_desc->size;
            }
            /* • Se termina el espacio contiguo en la memoria virtual, es lo mismo esto que no encontrar frames disponibles? */
        } else {
            // seguimos en la misma pagina
            // nos movemos a la direccion fisica pero sumando size
            fseek(memory, dir_actual+file_desc->size, SEEK_SET);
            // escribimos un byte
            unsigned char byte = *(unsigned char*) &buffer[file_desc->size]; 
            printf("\t BYTE TO WRITE 0x%02x\n", byte);
            fwrite(&byte, 1, 1, memory);
            // actualizamos los bytes escritos
            file_desc -> size += 1;
        }

        /* • SE ESCRIBIERON LOS N BYTES. */
        if (file_desc -> size == n_bytes){
            printf("\t-- END FILE --\n");
            printf("Se han escrito %i bytes exitosamente\n", file_desc->size);
            printf("CR_WRITE_FILE END. Bytes escritos (output): %i.\n", i);
            fclose(memory);
            return i;
        }
    }
    printf("\t-- END FOR --\n");
    printf("Se han escrito los %d bytes.\n", n_bytes);
    printf("CR_READ END. Bytes escritos (output): %d.\n", n_bytes);
    fclose(memory);
    return n_bytes;

}

int cr_conseguir_dir( CrmsFile * file_desc, char mode){

    int process_id = file_desc -> process_id;

    // Se obtiene el virtual por leer address del archivo
    unsigned int file_va = file_desc -> virtual_dir + mode == 'w'?file_desc->size:file_desc -> bytes_leidos;


    unsigned char vpn = va_vpn(file_va);
    unsigned int offset = get_offset(file_va);

    FILE * memory = fopen(MEMORY_PATH, "rb");
    // Datos para llegar al Page Table
    unsigned int process_id_uint;
    unsigned char process_state;

    unsigned int moves = 0;

    for (int i=0; i < PCB_N_ENTRIES; i++){
        // estado del proceso
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        moves += PROCESS_STATE_SIZE;
        // id del proceso 
        process_id_uint = fgetc(memory);
        moves += 1;
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);
        moves += NAMES_SIZE;
        // si el proceso esta cargado y tiene id igual al buscado
        if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id){
            // recorremos las entradas de archivos
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE,SEEK_CUR);
            moves += PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE;
            unsigned char entry;

            // Guardamos la dirección (en bytes de la page table).
            for (int i = 0; i < PAGE_TABLE_N_ENTRIES; i++)
            {   
                // DE ACÁ EN ADELANTE ESTOY ITERANDO POR LA PAGE TABLE
                fread(&entry, PAGE_TABLE_ENTRY_SIZE,1,memory);
                if (i == vpn)
                {
                    unsigned char validez = ta_validez(entry);
                    if (validez == (unsigned char)0x01)
                    {
                        unsigned int pfn = ta_pfn(entry);
                        unsigned int dir_fisica;

                        // Concatenamos los bits pfn con offset para hacer la dirección física.
                        pfn = pfn << 23;
                        dir_fisica = pfn + offset;
                        // Le sumamos los 4KB de la PCB y los 16B del Frame Bitmap
                        unsigned int dir = dir_fisica + PCB_SIZE + FRAME_BITMAP_SIZE;


                        file_desc -> pfn = pfn;
                        file_desc -> dir = dir;

                        // Fijamos la última posición leída a la dirección física inicial. 
                        fclose(memory);
                        return 0;
                    } else 
                    {
                        printf("ERROR: la entrada de la tabla de páginas no es válida.\n");
                        fclose(memory);
                        return -1;    
                    }
                }
            }
            fclose(memory);
            printf("ERROR: el archivo por leer no existe.\n");
            return -1;
        } else {
            // Este proceso no nos sirve, por lo que >
            // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
            moves += PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES;
        }
    }
    // Si no encontramos el proceso, retornamos ERROR
    fclose(memory);
    printf("ERROR: el proceso del archivo no existe.\n");
    return -1;
}

int cr_read(CrmsFile * file_desc, char* buffer, int n_bytes){

    printf("CR_READ RUNNING\n");
    // Si la dirección física del archivo todavía no ha sido init.
    if (!file_desc -> bytes_leidos){
        printf("\tEs primera vez que se lee este archivo (o se leyó antes, pero terminó de leerse completamente), por lo que se buscará su dirección física.\n");
        cr_conseguir_dir(file_desc,'r');
        printf("\tDirección física encontrada: %u.\n", file_desc -> dir);
    } else {
        printf("\tEste archivo ya se ha leído antes.\n");
        cr_conseguir_dir(file_desc,'r');
        printf("\tCantidad de bytes previamente leídos: %u.\n", file_desc -> bytes_leidos);
    }

    // Posición actual inicial -> última posición en la que se leyó el archivo.
    unsigned int dir_actual = file_desc -> dir;

    printf("\tDIR INICIAL: %u\n", dir_actual);

    printf("\tBytes por leer en esta llamada: %d.\n", n_bytes);

    printf("\t-- BEGIN FOR --\n");
    // Abrimos el archivo de memoria
    FILE * memory = fopen(MEMORY_PATH, "rb");
    // Definimos el dato de memoria que se va a guardar.
    unsigned char dato;
    // Tenemos que leer la cantidad de bytes.
    for (int i = 1; i < n_bytes + 1; i++)
    {
        // Encontramos la nueva dir_actual.
        cr_conseguir_dir(file_desc,'r');
        dir_actual = file_desc -> dir;
        // Nos movemos a la dirección física.
        fseek(memory, dir_actual, SEEK_SET);
        // Extreamos byte de memoria de archivo.
        fread(&dato, 1, 1, memory);
        // Guardamos el dato en la posición bytes_leidos del buffer. 
        buffer[file_desc->bytes_leidos] = dato;

        file_desc -> bytes_leidos += 1;
        //REVISAR SI SE LLEGA AL FINAL DEL ARCHIVO
        // Revisamos si se llega al final del archivo.
        if (file_desc -> bytes_leidos == file_desc -> size){
            printf("\t-- END FILE --\n");
            printf("Se ha llegado hasta el final del archivo.\nReseteando sus bytes_leidos a 0.\n");
            file_desc -> bytes_leidos = 0;
            // Retornamos la cantidad de bytes leídos hasta ahora por esta llamada.
            printf("CR_READ END. Bytes leídos (output): %d.\n", i);
            fclose(memory);
            return i;
        }
    }
    // Si termina el for, es por que se leyeron todos los bytes por leer del input 'n_bytes'.
    // Así, termina la función retornando la totalidad de bytes leídos (que es igual a 'n_bytes').
    printf("\t-- END FOR --\n");
    printf("Se han leído los %d bytes.\n", n_bytes);
    printf("CR_READ END. Bytes leídos (output): %d.\n", n_bytes);
    fclose(memory);
    return n_bytes;
}

void cr_delete(CrmsFile * file_desc){
    printf("CR_DELETE RUNNING\n");

    // Cosas del proceso y archivos.
    int process_id = file_desc -> process_id;
    char* file_name = file_desc -> file_name;

    // Definimos invalid
    unsigned char invalid = 0x00;


    FILE * memory = fopen(MEMORY_PATH, "r+b");
    // Datos para llegar al Page Table
    unsigned char process_file[NAMES_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
    unsigned char process_state;

    // Page Table
    unsigned char entry;

    // Subentradas
    unsigned int size;
    unsigned int dir_virtual;
    unsigned char vpn_antes;
    unsigned char vpn_despues;


    // Sabemos que el archivo existe y está activado.
    // Recorremos su PCB.
    for (int i = 0; i < PCB_N_ENTRIES; i++){
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        // id del proceso 
        process_id_uint = fgetc(memory);
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);
        // si el proceso esta cargado y tiene id igual al buscado
      if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id)
        {
            // Recorremos los archivos
            for (int j = 0; j < PROCESS_N_FILES_ENTRIES; j++)
            {
                // Leemos validez.
                fread(&file_state, 1, 1, memory);
                // Leemos nombre
                fread(process_file, NAMES_SIZE, 1, memory);
                // Si está cargado Y su nombre es igual al que buscamos:
                if (file_state ==  0x01 && strcmp((char *) process_file,file_name) == 0)
                {
                    printf("\tFOUND: %s.\n", process_file);
                    // Volvemos 13 bytes para editar el byte de validez.
                    fseek(memory,-13L,SEEK_CUR);
                    // Cambiamos su bit de validez a 0.
                    fwrite(&invalid, sizeof(unsigned char), 1, memory);
                    //Ahora, tenemos que llegar a page table, por lo que nos saltamos los que quedan.
                    fseek(memory,NAMES_SIZE+PROCESS_FILE_SIZE + VIRTUAL_ADRESS_SIZE,SEEK_CUR);
                // Si no es el archivo que buscamos.

                } else {
                    // Pasamos al próximo archivo.
                    fseek(memory, PROCESS_FILE_SIZE + VIRTUAL_ADRESS_SIZE,SEEK_CUR);
                }
                
            }
            // PAGE TABLE
            for (int k = 0; k < PAGE_TABLE_N_ENTRIES; k++)
            {
                entry = fgetc(memory);
                // entry es válida
                if (entry > 127)
                {
                    // Este es el pfn
                    entry = entry - 0x80;
                    //Volvemos uno atrás y lo reescribimos
                    fseek(memory,-1L,SEEK_CUR);
                    fwrite(&entry, sizeof(unsigned char), 1, memory);
                    change_frame_bit_map(entry);
                }
            }
            // Volvemos al principio
            rewind(memory);
            for (int l = 0; l < PCB_N_ENTRIES; l++)
            {
                fread(&process_state,PROCESS_STATE_SIZE,1,memory);
                // id del proceso 
                process_id_uint = fgetc(memory);
                // saltamos su nombre
                fseek(memory,NAMES_SIZE,SEEK_CUR);
                // si el proceso esta cargado y tiene id igual al buscado
                if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id){
                    //Estamos en las subentradas del proceso
                    for (int m = 0; m < PROCESS_N_FILES_ENTRIES; m++){
                        // Leemos validez.
                        fread(&file_state, 1, 1, memory);
                        // Leemos nombre
                        fread(process_file, NAMES_SIZE, 1, memory);
                        
                        // Saltamos nombre
                        //fseek(memory, NAMES_SIZE, SEEK_CUR);
                        fread(&size, PROCESS_FILE_SIZE, 1, memory);
                        fread(&dir_virtual, VIRTUAL_ADRESS_SIZE, 1, memory);
                        if (file_state ==  0x01)
                        {
                            vpn_antes = va_vpn(dir_virtual);
                            vpn_despues = va_vpn(dir_virtual + size);
                            if (vpn_antes == vpn_despues)
                            {
                                activate_page_table(vpn_despues, process_id);
                            } else {
                                for (int p = vpn_antes; p < vpn_despues+1; p++)
                                {
                                    activate_page_table(p, process_id);
                                }
                                
                            }
                            
                        }
                        
                        size = bswap_32(size);
                        dir_virtual = bswap_32(dir_virtual);
                    }
                }
                else{
                    fseek(memory, PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE+PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
                }
            }
        // Si no es el proceso que buscamos:    
        } else
        {
            fseek(memory, PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE+PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
}

void cr_close(CrmsFile* file_desc){
    free(file_desc -> file_name);
    free(file_desc);
}

unsigned int get_offset(unsigned int file_va){
    unsigned int offset = file_va & 2097151;
    return offset;
}


unsigned char va_vpn(unsigned int file_va){
    unsigned char vpn = file_va >> 23;
    return vpn;
}

unsigned char ta_validez(unsigned char table_entry){
    unsigned char validez = table_entry >> 7;
    return validez;
}

unsigned int ta_pfn(unsigned char table_entry){
    unsigned int pfn = table_entry & 127;
    return pfn;
}

void va_print(unsigned int file_va){
    unsigned char vpn;
    unsigned int offset;

    printf("VA en binario: ");
    bin(file_va, 32);
    printf("\n");

    offset = file_va & 2097151;
    vpn = file_va >> 23;

    printf("Offset en binario: ");
    bin(offset, 23);
    printf("\n");


    printf("VPN en binario: ");
    bin(vpn, 5);
    printf("\n");

}

void bin(unsigned n, int m)
    // Imprime "m" bits de un decimal "n" en binario.
    // Ej: bin(89216, 32)
    //  > "00000000000000010101110010000000"
    // Ej: bin(89216, 23)
    //  > "00000010101110010000000"
    // Ej: bin(0, 5)
    //  > "00000"
{
    unsigned i;
    for (i = 1 << (m-1); i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}

unsigned char find_empty_frame(){
    //OJO: no solo lo encuentra, también lo cambia
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    fseek(memory, PCB_SIZE, SEEK_CUR);
    unsigned char byte;
    unsigned char cero = 0x00;
    unsigned char uno = 0x01;
    unsigned char bit_buscado;
    unsigned char bits_pasados;

    for (int n=0; n < FRAME_BITMAP_SIZE; n++){
        byte = fgetc(memory);
        for (int i = 0; i < 8; i++)
        {
            bit_buscado = (byte >> (7-i)) & uno;
            if (bit_buscado == cero)
            {
                bits_pasados = n * 8;
                bits_pasados = bits_pasados + i;
                change_frame_bit_map(bits_pasados);
                return bits_pasados;
            }   
        }
    }
    printf("No hay frames disponibles\n");
    fclose(memory);
    return NULL;
}

void activate_page_table(unsigned char vpn, unsigned int process_id){
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    // Datos para llegar al Page Table
    unsigned char process_file[NAMES_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
    unsigned char process_state;

    // Page Table
    unsigned char entry;

    // Subentradas
    unsigned int size;
    unsigned int dir_virtual;
    unsigned char vpn_antes;
    unsigned char vpn_despues;


    // Sabemos que el archivo existe y está activado.
    // Recorremos su PCB.
    for (int i = 0; i < PCB_N_ENTRIES; i++){
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        // id del proceso 
        process_id_uint = fgetc(memory);
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);
        // si el proceso esta cargado y tiene id igual al buscado
        if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) process_id)
        {
            fseek(memory, 210, SEEK_CUR);
            // PAGE TABLE
            fseek(memory, vpn, SEEK_CUR);
            unsigned char byte = fgetc(memory);
            if (byte < 128)
            {
                change_frame_bit_map(byte);
                byte = byte + 0x80;
            }
        // Si no es el proceso que buscamos:    
        } else
        {
            fseek(memory, PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE+PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
}

void change_frame_bit_map(unsigned char posicion){
    //Esta funcion recibe una posicion del frame bit map
    //hay 128 posiciones, del 0 al 127
    //Y cambia esa posicion de 1 a 0 o de 0 a 1 segun corresponda
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    fseek(memory, PCB_SIZE, SEEK_CUR);
    unsigned int posicion_byte;
    unsigned char byte;
    unsigned char posicion_relativa;
    unsigned char byte_resta;
    unsigned char bit_buscado;
    unsigned char uno = 0x01;

    posicion_byte = posicion / 8;
    while (posicion_byte != 0)
    {
        fseek(memory, PAGE_TABLE_ENTRY_SIZE, SEEK_CUR);
        posicion_byte--;
        posicion = posicion - 8;
    }
    posicion_relativa = 7 - posicion;
    byte_resta = pow(2, posicion_relativa);
    byte = fgetc(memory);
    bit_buscado = (byte >> posicion_relativa) & uno;
    if (bit_buscado == uno)
    {
        byte = byte - byte_resta;
    }
    else{
        byte = byte + byte_resta;
    }
    fseek(memory,-1L,SEEK_CUR);
    fwrite(&byte, sizeof(unsigned char), 1, memory);
    fclose(memory);
}

void print_frame_bit_map(){
    //Esta funcion imprime en consola el frame bit map
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    fseek(memory, PCB_SIZE, SEEK_CUR);
    unsigned char byte;
    for (int n=0; n < FRAME_BITMAP_SIZE; n++){
        byte = fgetc(memory);
        bin(byte, 8);
        printf("\n");
    }
    fclose(memory);
}

unsigned int link_new_page_to_empty_frame(unsigned char VPN, unsigned int PID){
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    unsigned char PFN;

    //Encontramos el primer frame libre y lo reservamos para esta page
    //Tambien se cambia el valor en el frame bit map a 1
    PFN = find_empty_frame();
    printf("PFN es : %d\n", PFN);

    unsigned int process_id_uint;
    unsigned char process_state;

    for (int i=0; i < PCB_N_ENTRIES; i++){

        // estado del proceso
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        // id del proceso 
        process_id_uint = fgetc(memory);
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);

        // si el proceso esta cargado y tiene id igual al buscado
        if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) PID){
            //Nos saltamos las entradas de archivos hasta llegar a la tabla de paginas
            fseek(memory, PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE, SEEK_CUR);
            //Avanzamos al entry dado por VPN
            fseek(memory, VPN*PAGE_TABLE_ENTRY_SIZE, SEEK_CUR);
            //Agregamos bit de validez a la entrada
            PFN = PFN + 0x80;
            printf("Nuevo valor de entrada: %d\n", PFN);
            fwrite(&PFN, sizeof(unsigned char), 1, memory);
            fclose(memory);
            return PFN;
        } else {
            // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
    return 0;
}

void print_page_table(unsigned int PID){
    FILE * memory = fopen(MEMORY_PATH, "r+b");

    unsigned int process_id_uint;
    unsigned char process_state;
    unsigned char byte;

    for (int i=0; i < PCB_N_ENTRIES; i++){

        // estado del proceso
        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        // id del proceso 
        process_id_uint = fgetc(memory);
        // saltamos su nombre
        fseek(memory,NAMES_SIZE,SEEK_CUR);

        // si el proceso esta cargado y tiene id igual al buscado
        if (process_state == (unsigned char)0x01 && process_id_uint == (unsigned int) PID){
            fseek(memory, PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE, SEEK_CUR);
            for (int k = 0; k < PAGE_TABLE_N_ENTRIES; k++)
            {
                byte = fgetc(memory);
                printf("Entrada de Page table: ");
                bin(byte, 8);
                printf("\n");
            }
            fclose(memory);
            return;
        } 
        else {
            // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
}

void write_file_real(char* buffer, CrmsFile* file_desc){
    // Imprime los contenidos de un CrmsFile (al que previamente se le leyó toda su info en buffer) 
    // en un archivo de PATH = '../nombre_de_crmsfile'.

    printf("FILE NAME:%s\n", file_desc -> file_name);
    FILE* archivo = fopen(file_desc->file_name, "wb");
    fwrite(buffer, 1, file_desc -> size, archivo);
    fclose(archivo);
}
