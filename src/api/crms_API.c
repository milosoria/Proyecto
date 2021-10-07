#include "crms_API.h"
#include <stdlib.h>

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
    free(file->file_ptr);
    free(file);
}
// Funciones Generales
void cr_mount(char * memory_path){
    return;
}

void cr_ls_processes(){
    return;
}

int cr_exists(int process_id, char * file_name){}

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
