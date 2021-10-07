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
#define PAGE_TABLE_SIZE 32
// PAGE_TABLE: 
#define PAGE_TABLE_N_ENTRIES 32
// PFN 7 bits
// FRAME_BITMAP:
#define FRAME_BITMAP_SIZE 16
// FRAME:
#define FRAME_SIZE 8388608
#define N_FRAMES 128
