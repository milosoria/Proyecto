#include "crms_API.h"
//#include <byteswap.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>

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
    crms -> last_pos = 0;
    crms -> file_name = strdup(file_name);
    return crms;
}

void destroy_crms_file(CrmsFile* file){
    free(file);
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
    unsigned char process_name_buff[NAMES_SIZE];
    unsigned int process_id_uint;
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

            // TODO: Esta bien esto para escribirlo en big endian?
            for (int k=0; k < NAMES_SIZE;k++){
                process_name_buff[k] = ((((process_name[k]) & 0xff) << 8) | ((process_name[k]) >> 8));
            }

            fwrite(process_name_buff, sizeof(unsigned char), NAMES_SIZE, memory);
            printf("\tWRITING INFO NAME:%s AND PID:%i\n", process_name,process_id);
            for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                fwrite(&init_valid, sizeof(unsigned char), 1, memory);
                //Pasamos a la siguiente entrada de archivos
                fseek(memory,VIRTUAL_ADRESS_SIZE+PROCESS_FILE_SIZE,SEEK_CUR);
            }
            fclose(memory);
            return;
        }
        fseek(memory,PROCESS_ID_SIZE+NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
    }
    fclose(memory);

}

void cr_finish_process(int process_id){
    return;
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
                    return -1;
                } else {
                    // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
                    fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
                }
            }
            // Si no encontramos el proceso, retornamos ERROR
            fclose(memory);
            printf("ERROR: el proceso por leer no existe.\n");
            return -1;
            
        } else if (mode == 'w') {
            // error pq el archivo ya exite
            printf("ERROR: se está creando un archivo que ya existe.\n");
            return -1;
        }
    } else {
        if (mode == 'r'){
            // error pq el archivo no exite
            printf("ERROR: se está leyendo un archivo que no existe.\n");
            return -1;
        } else if (mode == 'w') {
            return NULL;
        }
    }
}

/* 1.Lees de entrada en entrada del proceso (mediante el uso de fseek y fread) */
/* 2.Una vez encuentras el proceso buscas entrada por entrada el archivo que estas buscando (de nuevo, fseek y fread) */
/* 3.Una vez encuentras el archivo mediante el VPN y el tamaño del archivo sabes exactamente que paginas usa el archivo */
/* 5.Lees pagina por pagina del archivo */
/* 6.Por cada pagina perteneciente al archivo con el PFN obtienes exactamente el frame y al que pertenece la pagina. */
/* 7.Entre el VPN, y el PFN de cada pagina sabes cuales frames leer y cuanto leer. */

int cr_write_file(CrmsFile* file_desc, void * buffer, int n_bytes){}

int cr_read(CrmsFile * file_desc, void* buffer, int n_bytes){
    int process_id = file_desc -> process_id;
    char* file_name = file_desc -> file_name;

    FILE * memory = fopen(MEMORY_PATH, "rb");
    unsigned char process_file[NAMES_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
    unsigned char process_state;

    unsigned int file_size;
    unsigned int file_va;

    unsigned int moves;

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
            printf("\t[PID:%u] FOUND\n", process_id_uint);
            // recorremos las entradas de archivos
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE,SEEK_CUR);
            unsigned int entry;
            for (int i = 0; i < PAGE_TABLE_N_ENTRIES; i++)
            {
                fread(&entry, PAGE_TABLE_ENTRY_SIZE,1,memory);
                
                unsigned int validez = ta_validez(entry);
                unsigned int pfn = ta_pfn(entry);
                
                printf("\tPAGE ENTRY %d: %u\n", i, entry);
            }
            exit(0);
            fclose(memory);
            printf("ERROR: el archivo por leer no existe.\n");
            return -1;
        } else {
            // Nos saltamos todas las entradas del proceso y dejamos el puntero en la siguiente entrada de al PCB
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    // Si no encontramos el proceso, retornamos ERROR
    fclose(memory);
    printf("ERROR: el proceso por leer no existe.\n");
    return -1;
}

void cr_delete(CrmsFile * file_desc){
    return;
}
void cr_close(CrmsFile* file_desc){
    return;
}

unsigned int va_offset(unsigned int file_va){
    unsigned int offset = file_va & 2097151;
    return offset;
}

unsigned int va_vpn(unsigned int file_va){
    unsigned int vpn = file_va >> 23;
    return vpn;
}

unsigned int ta_validez(unsigned int table_entry){
    unsigned int validez = table_entry >> 7;
    return validez;
}

unsigned int ta_pfn(unsigned int table_entry){
    unsigned int pfn = table_entry & 63;
    return pfn;
}

void va_print(unsigned int file_va){
    unsigned int vpn;
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
