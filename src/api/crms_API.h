#include <stdio.h>
#include <sys/types.h>
#include "../crms/sizes.h"
#if HAVE_BYTESWAP_H 
#include <byteswap.h> 
#else 
#define bswap_16(value) \ 
((((value) & 0xff) << 8) | ((value) >> 8)) 

#define bswap_32(value) \ 
(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \ 
(uint32_t)bswap_16((uint16_t)((value) >> 16))) 

#define bswap_64(value) \ 
(((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) \ 
<< 32) | \ 
(uint64_t)bswap_32((uint32_t)((value) >> 32))) 
#endif 

//TODO: son estos atributos necesarios???
typedef struct crmsfile {
    // direccion dentro de la memoria
    unsigned int  virtual_dir;
    // ultima posicion leida por cr_read o offset desde donde comenzar a leer
    int last_pos;
    int process_id;
    // size del archivo
    unsigned int size;
    // tabla de páginas del archivo
    unsigned int* tabla_paginas;
} CrmsFile;

// Funciones para manejar struct
CrmsFile * init_crms_file(unsigned int virtual_dir,int process_id, int last_pos, unsigned int size, unsigned int* tabla_paginas);

// Funciones Generales
void cr_mount(char * memory_path);

void cr_ls_processes();

void cr_ls_files(int process_id);

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
