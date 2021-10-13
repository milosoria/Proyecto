// PCB:
#define PCB_N_ENTRIES 16
// 4 KB
#define PCB_SIZE 256 * 16 
// 256/16 entries = 16 bytes cada una
// size of 1 byte
#define NAMES_SIZE 12
// process_name, file_name
#define PROCESS_SIZE 256
#define PROCESS_ID_SIZE 1 
#define PROCESS_STATE_SIZE 1 
#define PROCESS_N_FILES_ENTRIES 10
#define PROCESS_FILE_ENTRY_SIZE 21
#define PROCESS_FILE_SIZE 4
#define VIRTUAL_ADRESS_SIZE 4
#define PAGE_TABLE_ENTRY_SIZE 1
// PAGE_TABLE: 
#define PAGE_TABLE_N_ENTRIES 32
// PFN 7 bits
// FRAME_BITMAP:
#define FRAME_BITMAP_SIZE 16
// FRAME:
#define FRAME_SIZE 8388608
#define N_FRAMES 128
/* - State(1 Byte): 0x01 || 0x00 (ejecutando o no) */
/* - 1 Byte para el PID */
/* - 12 Bytes ProcessName */
/* - Despues de los 14 Bytes, cada entrada tiene 10 subentradas para guardar informacion sobre los archivos (21 Bytes x 10) de su memoria virtual: */
/*     * Valid (1 Byte): 0x01 || 0x00 (valido o no) */
/*     * FileName (12 Bytes) */
/*     * FileSize (4 Bytes): el max es 32 MB */
/*     * VirtualAdress (4 Bytes): 4 bits no significativos (0b0000) + 5 bits VPN + 23 bits offset */
/* - Por ultimo: Tabla de Paginas (32 Bytes), 1 Byte y 32 entradas */
