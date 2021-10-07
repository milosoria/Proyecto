#include <stdio.h>
#include <sys/types.h>

typedef struct crmsfile {
    // direccion dentro de la memoria
    u_int64_t  file_dir;
    // pointer a la memoria
    FILE * file_ptr;
    // ultima posicion leida por cr_read o offset desde donde comenzar a leer
    int last_pos;
    // size del archivo
    int n_bytes;
} CrmsFile;

// Funciones para manejar struct
CrmsFile * init_crms_file(u_int64_t dir, FILE* file_ptr,int last_pos, int size);

void destroy_crms_file(CrmsFile* file);

// Funciones Generales
void cr_mount(char * memory_path);

void cr_ls_processes();

int cr_exists(int process_id, char * file_name);

// Funciones Procesos

void cr_start_process(int process_id , char * process_name);

void cr_finish_process(int process_id);

// Funciones Archivos
CrmsFile* cr_open(int process_id, char* file_name, char mode);
int cr_write_file(CrmsFile* file_desc, void * buffer, int n_bytes);
int cr_read( CrmsFile * file_desc, void* buffer, int n_bytes);
void cr_delete(CrmsFile * file_desc);
void cr_close(CrmsFile* file_desc);
