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
    // nombre del archivo
    char *file_name;
    // direccion dentro de la memoria
    unsigned int  virtual_dir;
    // ultima posicion leida por cr_read o offset desde donde comenzar a leer
    unsigned int process_id;
    // size del archivo
    unsigned int size;
    // dirección física
    unsigned int dir;
    // útlima posición leída
    unsigned int last_pos;
} CrmsFile;

// Funciones para manejar struct
CrmsFile * init_crms_file(unsigned int virtual_dir, unsigned int process_id, unsigned int size, char *file_name);
void destroy_crms_file(CrmsFile* file);


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

// Virtual Address
unsigned int va_vpn(unsigned int file_va);
unsigned int va_offset(unsigned int file_va);

// Page Table
unsigned int ta_validez(unsigned int table_entry);
unsigned int ta_pfn(unsigned int table_entry);

void bin(unsigned n, int m);
void va_print(unsigned int file_va);

//Coloro
unsigned int find_empty_frame();
void change_frame_bit_map(unsigned char posicion);
void print_frame_bit_map();
