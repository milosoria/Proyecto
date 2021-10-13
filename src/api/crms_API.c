#include "crms_API.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include <sys/types.h>

// esta bien hacerlo asi?
char * MEMORY_PATH;

// Funciones para manejar struct
CrmsFile * init_crms_file(u_int64_t dir,int process_id, FILE* file_ptr,int last_pos, int size){
    /* crmsfile representa un archivo */
    /* si miras la función cr_open, esta recibe el id del proceso, el nombre del archivo y el modo */
    /* lo que te retorna es la representación de un archivo asociado a un proceso y la gracia de esta representación es que tenga
     * toda la información que estimes necesaria para que el resto de las funciones de la API que lo recibirán como argumento puedan funcionar bien */
    CrmsFile * crms = malloc(sizeof(CrmsFile));
    crms -> process_id = process_id;
    crms -> file_dir = dir;
    crms -> file_ptr = file_ptr;
    crms -> last_pos = last_pos;
    // para usar fseek
    crms -> n_bytes = size;

    return crms;
}

void destroy_crms_file(CrmsFile* file){
    fclose(file->file_ptr);
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
        if (process_state == (unsigned char)0x01){
            // obtenemos el pid
            process_id_uint = fgetc(memory);
            // obtenemos el nombre del proceso
            fread(process_name,NAMES_SIZE,1,memory);
            printf("\t[PID:%i] Process: %s state 0x%02x\n", (unsigned int)process_id_uint,process_name,process_state);
            fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        } else {
            fseek(memory,PROCESS_ID_SIZE+NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }

    }
    fclose(memory);
}


int cr_exists(int process_id, char * file_name){
    printf("CR_EXISTS RUNNING\n");
    FILE * memory = fopen(MEMORY_PATH, "rb");

    unsigned char process_file[PROCESS_FILE_ENTRY_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
    unsigned char process_state;

    for (int i=0; i < PCB_N_ENTRIES; i++){

        fread(&process_state,PROCESS_STATE_SIZE,1,memory);
        process_id_uint = fgetc(memory);

        if (process_state == (unsigned char)0x01){
            fseek(memory,NAMES_SIZE,SEEK_CUR);

            if (process_id_uint == (unsigned int) process_id){
                printf("\tPID %u FOUND %u\n", process_id, process_id_uint);
                for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                    // nos saltamos el byte de validez
                    fread(&file_state,1,1,memory);
                    // TODO: Entra aca, a pesar de que el estado es 0x00
                    if ((unsigned char)process_state ==(unsigned char) 0x01){
                        printf("\tFILE STATE 0x%02x\n",file_state);
                        // leemos el nombre del archivo del primer archivo
                        fread(process_file,NAMES_SIZE,1,memory); 
                        printf("\tFILE NAME %s\n", process_file);
                        //printf("\tPROCESS FILE NAME %s\n", process_file);
                        if (strcmp(process_file,file_name) == 0){
                            // encontramos el archivo, entonces retornamos 1
                            fclose(memory);
                            return 1;
                        }
                        // dejamos el puntero al comienzo de la siguiente entrada de archivos
                    }
                    fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR); 
                }
                // Si no encontramos el archivo, retornamos 0
                fclose(memory);
                return 0;
            } else {
                fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
            }
        } else {
            fseek(memory,NAMES_SIZE+PROCESS_N_FILES_ENTRIES*PROCESS_FILE_ENTRY_SIZE + PAGE_TABLE_ENTRY_SIZE*PAGE_TABLE_N_ENTRIES,SEEK_CUR);
        }
    }
    fclose(memory);
    return 0;
}

void cr_ls_files(int process_id){
    /*     // Comprobar si en la PCB el proceso tiene al archivo file_name */
    /*     FILE * memory = fopen(MEMORY_PATH, "r+b"); */
    /*     char process_file[PROCESS_FILE_ENTRY_SIZE]; */
    /*     char process_id_buff[PROCESS_ID_SIZE]; */

    /*     for (int i=0; i < PCB_N_ENTRIES; i++){ */

    /*         fseek(memory,PROCESS_STATE_SIZE,SEEK_CUR); */
    /*         fread(process_id_buff,1, PROCESS_ID_SIZE,memory); */
    /*         fseek(memory,NAMES_SIZE,SEEK_CUR); */

    /*         if ((int) process_id_buff[0] == process_id){ */
    /*             printf("[PID:%s] Process with the following files:\n", process_id_buff); */
    /*             for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){ */
    /*                 // nos saltamos el byte de validez */
    /*                 fseek(memory,1,SEEK_CUR); */

    /*                 // leemos el nombre del archivo del primer archivo */
    /*                 fread(process_file,1,NAMES_SIZE,memory); */ 
    /*                 printf("\tFile with name %s\n", process_file); */

    /*                 // dejamos el puntero en la siguiente entrada */
    /*                 fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR); */ 
    /*             } */

    /*             fclose(memory); */
    /*             return; */
    /*         } else { */
    /*             fseek(memory, PROCESS_N_FILES_ENTRIES * PROCESS_FILE_SIZE, SEEK_CUR); */
    /*         } */

    /*         fseek(memory,PAGE_TABLE_SIZE,SEEK_CUR); */

    /*     } */
}

// Funciones Procesos

void cr_start_process(int process_id, char * process_name){
    return;
}

void cr_finish_process(int process_id){
    return;
}

// Funciones Archivos
CrmsFile* cr_open(int process_id, char* file_name, char mode){
    if (mode == 'r'){
        return NULL;
    } else {
        int exists = cr_exists(process_id, file_name);
        return NULL;
    }
}

/* 1.Lees de entrada en entrada del proceso (mediante el uso de fseek y fread) */
/* 2.Una vez encuentras el proceso buscas entrada por entrada el archivo que estas buscando (de nuevo, fseek y fread) */
/* 3.Una vez encuentras el archivo mediante el VPN y el tamaño del archivo sabes exactamente que paginas usa el archivo */
/* 5.Lees pagina por pagina del archivo */
/* 6.Por cada pagina perteneciente al archivo con el PFN obtienes exactamente el frame y al que pertenece la pagina. */
/* 7.Entre el VPN, y el PFN de cada pagina sabes cuales frames leer y cuanto leer. */

int cr_write_file(CrmsFile* file_desc, void * buffer, int n_bytes){}

int cr_read( CrmsFile * file_desc, void* buffer, int n_bytes){}

void cr_delete(CrmsFile * file_desc){
    return;
}
void cr_close(CrmsFile* file_desc){
    return;
}
