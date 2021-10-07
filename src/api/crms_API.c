#include "crms_API.h"
#include <stdlib.h>
#include <string.h>

// esta bien hacerlo asi?
extern char * MEMORY_PATH;

// Funciones para manejar struct
CrmsFile * init_crms_file(u_int64_t dir, FILE* file_ptr,int last_pos, int size){
    CrmsFile * crms = malloc(sizeof(CrmsFile));
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
    // recorrer pcb y revisar si tiene state 0x01 0 0x00 e imprime el nombre del proceso y su id
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    char process_name[NAMES_SIZE];
    char process_id[PROCESS_ID_SIZE];
    char process_state[PROCESS_STATE_SIZE];
    for (int i=0; i < PCB_N_ENTRIES; i++){

        fread(process_state,PROCESS_STATE_SIZE,1,memory); 
        fread(process_id,PROCESS_ID_SIZE,1,memory); 
        fread(process_name,NAMES_SIZE,1,memory); 

        // avanzamos hacia el siguiente proceso
        fseek(memory,PROCESS_N_FILES_ENTRIES*PROCESS_FILE_SIZE + PAGE_TABLE_SIZE,SEEK_CUR);

        // si esta en ejecucion, entonces lo imprimimos
        if (strcmp(process_state, "0x01")){
            printf("[PID:%s] Process: %s executing\n", process_id,process_name);
        }
    }

    fclose(memory);
    return;
}


int cr_exists(int process_id, char * file_name){
    // Comprobar si en la PCB el proceso tiene al archivo file_name
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    char process_file[PROCESS_FILE_ENTRY_SIZE];
    char process_id_buff[PROCESS_ID_SIZE];

    for (int i=0; i < PCB_N_ENTRIES; i++){

        fseek(memory,PROCESS_STATE_SIZE,SEEK_CUR);
        fread(process_id_buff, PROCESS_ID_SIZE,1,memory);
        fseek(memory,NAMES_SIZE,SEEK_CUR);

        if ((int) process_id_buff[0] == process_id){
            for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                // nos saltamos el byte de validez
                fseek(memory,1,SEEK_CUR);
                // leemos el nombre del archivo del primer archivo
                fread(process_file,NAMES_SIZE,1,memory); 
                if (strcmp(process_file,file_name)){
                    fclose(memory);
                    return 1;
                }
                // dejamos el puntero al comienzo de la siguiente entrada de archivos
                fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR); 
            }

            fclose(memory);
            return 0;

        } else {
            fseek(memory, PROCESS_N_FILES_ENTRIES * PROCESS_FILE_SIZE, SEEK_CUR);
        }
        // El puntero queda al comienzo de la tabla de paginas
        fseek(memory,PAGE_TABLE_SIZE,SEEK_CUR);
        // lo dejamos al comienzo de la siguiente entrada de la PCB
    }

    fclose(memory);
    return 0;
}

void cr_ls_files(int process_id){
    // Comprobar si en la PCB el proceso tiene al archivo file_name
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    char process_file[PROCESS_FILE_ENTRY_SIZE];
    char process_id_buff[PROCESS_ID_SIZE];

    for (int i=0; i < PCB_N_ENTRIES; i++){

        fseek(memory,PROCESS_STATE_SIZE,SEEK_CUR);
        fread(process_id_buff, PROCESS_ID_SIZE,1,memory);
        fseek(memory,NAMES_SIZE,SEEK_CUR);

        if ((int) process_id_buff[0] == process_id){
            printf("[PID:%s] Process with the following files:\n", process_id_buff);
            for (int j=0; j < PROCESS_N_FILES_ENTRIES; j++){
                // nos saltamos el byte de validez
                fseek(memory,1,SEEK_CUR);

                // leemos el nombre del archivo del primer archivo
                fread(process_file,NAMES_SIZE,1,memory); 
                printf("\tFile with name %s\n", process_file);

                // dejamos el puntero en la siguiente entrada
                fseek(memory,PROCESS_FILE_SIZE+VIRTUAL_ADRESS_SIZE,SEEK_CUR); 
            }

            fclose(memory);
            return;
        } else {
            fseek(memory, PROCESS_N_FILES_ENTRIES * PROCESS_FILE_SIZE, SEEK_CUR);
        }

        fseek(memory,PAGE_TABLE_SIZE,SEEK_CUR);

    }
}

// Funciones Procesos

void cr_start_process(int process_id , char * process_name){
    return;
}

void cr_finish_process(int process_id){
    return;
}

// Funciones Archivos
CrmsFile* cr_open(int process_id, char* file_name, char mode){
}

int cr_write_file(CrmsFile* file_desc, void * buffer, int n_bytes){}

int cr_read( CrmsFile * file_desc, void* buffer, int n_bytes){}

void cr_delete(CrmsFile * file_desc){
    return;
}
void cr_close(CrmsFile* file_desc){
    return;
}
