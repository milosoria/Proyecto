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
    // Los siguientes parten en 0, si inicializan con cr_conseguir_dir
    crms -> last_pos = 0;
    crms -> dir = 0;
    crms -> dir_page_table = 0;
    // Este se va llenando en cr_read.
    crms -> bytes_leidos = 0;
    
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


            size_t element_size = sizeof *process_name;
            size_t elements_to_write = sizeof process_name;

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
    unsigned char init_state = 0x01;
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
                for (int k=0; k<PAGE_TABLE_N_ENTRIES; k++){
                    fread(&page_table_entry, PAGE_TABLE_ENTRY_SIZE, 1, memory);
                    if (page_table_entry > 127){
                        //Si tiene bit de validez igual a 1 sabemos que tiene un frame asociado
                        page_table_entry = page_table_entry - 128;
                        printf("Entry #%i - PFN: %i\n", k, page_table_entry);
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

int cr_write_file(CrmsFile* file_desc, void * buffer, int n_bytes){
    // asumo que conseguir dir me retorna la direccion fisica, por ahora ocupo dir_page_table
    FILE* memory = fopen(MEMORY_PATH,"r+b");
    fseek(memory,file_desc->dir_page_table,SEEK_CUR);
    unsigned int offset = va_offset(file_desc->virtual_dir);
    printf("virtual_dir %i\n", file_desc->virtual_dir);
    printf("dir_page_table %i\n", file_desc->dir_page_table);
    return 1;

/* - [ ] int cr_write_file(CrmsFile\* file_desc, void \* buffer, int n_bytes):
 * Funcion para escribir archivos.
 * Escribe en el archivo representado por file_desc los n_bytes que se encuentran en la direccion indicada por buffer y
 * retorna la cantidad de Bytes escritos en el archivo (en caso de no terminar). La escritura comienza desde el primer espacio libre (IMPORTANTE)
 * dentro de la memoria virtual. La escritura termina cuando: */

/*     - No quedan frames disponibles para continuar */
/*     - Se termina el espacio contiguo en la memoria virtual */
/*     - Se escribieron los n_bytes */

/* Con la direccion virtual de cada archivo tienes la página y el offset y aparte tienes el tamaño del proceso */
/* Sabes que las paginas tienen asociado un frame único a ellas */
/* asi que si analizas todas las entradas validas puedes saber, haciendo uso de la pagina y los offset y los tamaños que espacios vacios hay y donde */
/* y así mismo si tomas el proceso con mayor direccion virtual tienes por garantia de por como
 * se construye la direccion virutal que despues de ese proceso está todo vacío */

}

int cr_conseguir_dir( CrmsFile * file_desc){
    int process_id = file_desc -> process_id;
    char* file_name = file_desc -> file_name;

    unsigned int file_size = file_desc -> size;
    unsigned int file_va = file_desc -> virtual_dir;

    unsigned int vpn = va_vpn(file_va);
    unsigned int offset = va_offset(file_va);

    FILE * memory = fopen(MEMORY_PATH, "rb");
    unsigned char process_file[NAMES_SIZE];
    unsigned int process_id_uint;
    unsigned char file_state;
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
            file_desc -> dir_page_table = moves;
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
                        file_desc -> last_pos = dir;
                        return -1;
                    } else 
                    {
                        printf("ERROR: la entrada de la tabla de páginas no es válida.\n");
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
    printf("ERROR: el proceso por leer no existe.\n");
    return -1;
}

int cr_read(CrmsFile * file_desc, void* buffer, int n_bytes){
    printf("CR_READ RUNNING\n");
    // Si la dirección física del archivo todavía no ha sido init.
    if (!file_desc -> dir){
        printf("\tEs primera vez que se lee este archivo, por lo que se buscará su dirección física.\n");
        cr_conseguir_dir(file_desc);
        printf("\t Dirección física encontrada: %u.\n", file_desc -> dir);
    }

    // Posición actual inicial -> última posición en la que se leyó el archivo.
    unsigned int dir_actual = file_desc -> last_pos;
    
    printf("\tDIR INICIAL: %u\tEn Binario: ", dir_actual);
    bin(dir_actual, 32);
    printf("\n");

    printf("\tBytes por leer: %d.\n", n_bytes);

    // Tenemos que leer la cantidad de bytes.
    for (int i = 1; i < n_bytes + 1; i++)
    {
        file_desc -> bytes_leidos += 1;

        // REVISAR SI HAY CAMBIO DE PÁGINA

        // >> LEER/GUARDAR BYTE EN BUFFER
        // >> ACTUALIZAR LAST_POS y DIR_ACTUAL



        //REVISAR SI SE LLEGA AL FINAL DEL ARCHIVO
        // Revisamos si se llega al final del archivo.
        if (file_desc -> bytes_leidos == file_desc -> size){
            printf("\tSe ha llegado hasta el final del archivo.\n");

            // Retornamos la cantidad de bytes leídos hasta ahora por esta llamada.
            printf("CR_READ END. Bytes leídos: %d.", i);
             return i;
        }

        //printf(" DIR ACTUAL: %u\n En Binario: ", dir_actual);
        //bin(dir_actual, 32);
        //printf("\n");
    }
    // Si termina el for, es por que se leyeron todos los bytes por leer del input 'n_bytes'.
    // Así, termina la función retornando la totalidad de bytes leídos (que es igual a 'n_bytes').
    printf("\tSe han leído los %d bytes.\n", n_bytes);
    printf("CR_READ END. Bytes leídos: %d.\n", n_bytes);
    return n_bytes;


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

unsigned char ta_validez(unsigned char table_entry){
    unsigned char validez = table_entry >> 7;
    return validez;
}

unsigned int ta_pfn(unsigned char table_entry){
    unsigned int pfn = table_entry & 127;
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

unsigned int find_empty_frame(){
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    fseek(memory, PCB_SIZE, SEEK_CUR);
    unsigned int byte_prueba;
    for (int n=0; n < FRAME_BITMAP_SIZE; n++){
        byte_prueba = fgetc(memory);
        bin(byte_prueba, 8);
        printf("\n");
    }
}

void change_frame_bit_map(unsigned char posicion){
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    fseek(memory, PCB_SIZE, SEEK_CUR);
    unsigned int posicion_byte;
    posicion_byte = posicion / 8;
    printf("Acceder a byte %d para encontrar posicion %d\n", posicion_byte, posicion);
    for (int i = 0; i < posicion_byte; i++)
    {
        
    }
    
}

void print_frame_bit_map(){
    FILE * memory = fopen(MEMORY_PATH, "r+b");
    fseek(memory, PCB_SIZE, SEEK_CUR);
    unsigned char byte;
    for (int n=0; n < FRAME_BITMAP_SIZE; n++){
        byte = fgetc(memory);
        bin(byte, 8);
        printf("\n");
    }
}
