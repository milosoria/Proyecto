# Proyecto (Viernes 15 de Octubre, 2021):

# P1:
## Memoria:

- Size:
    * 4KB: Espacio reservado Tabla PCB
    * 16B: Frame bitmap
    * 1GB: frames:
        * Size: 8MB
        * PFN: Cada frame posee un numero de 7 bits (unsigned int)
        * #frames = 128 
### Tabla de PCB:

- Size: 256 bytes
- Numero de entradas: 16
- State(1 Byte): 0x01 || 0x00 (ejecutando o no)
- 1 Byte para el PID
- 12 Bytes ProcessName
- Despues de los 14 Bytes, cada entrada tiene 10 subentradas para guardar informacion sobre los archivos de su memoria virtual:
    * Valid (1 Byte): 0x01 || 0x00 (valido o no)
    * FileName (12 Bytes)
    * FileSize (4 Bytes): el max es 32 MB
    * VirtualAdress (4 Bytes): 4 bits no significativos (0b0000) + 5 bits VPN + 23 bits offset
- Tabla de Paginas (32 Bytes)

### Tabla de Paginas:

Informacion para traducir direcciones virtuales a fisicas dentro de la memoria. Las direcciones fisicas seran relativas al ultimo 1 GB de la memoria:

    2^12 + 2^4 + dir (|tabla de PCB| + |Frame bitmap| + dir)

- Numero de entradas: 32 (primer bit es la validez de la entrada 1 valida || 0 no)
- Valid (1 Byte): 0x01
- PFN: 7 bits
- Calculo direccion fisica:
    1. Obtener de la direccion virtual los 5 bits del VPN y 23 del offset
    2. Ingresar a la entrada VPN de la tabla de paginas y obtener PFN
    3. Direccion fisica sera igual al PFN mas el offset

### Frame Bitmap:

Se encuentra a continuacion de la PCB:
- Size: 16 Bytes = 128 bit
- Cada bit del frame bitmap indica si el frame esta libre (0) o no (1)
- Un bit por cada frame

### Frame:
Almacena los datos de los archivos. El size de cada frame es de 8 MB. Se encuentran en el ultimo 1GB y en total hay 128 frames

## API:

Implementacion de funciones para manipular archivos de procesos:

### Funciones Generales:

- [ ] void cr_mount(char \* memory_path): Funcion para montar memoria. Establece como variable global la ruta local donde se encuentra el archivo .bin correspondiente a la memoria

- [ ] voir cr_ls_processes(): Funcion que muestra en pantalla los procesos en ejecucion

- [ ] int cr_exists(int process_id, char \* file_name): Funcion para ver si un archivo con nombre file_name existe en la memoria del proceso con id process_id. Retorna 1 si existe || 0 si no.

### Funciones Procesos:

- [ ] void cr_start_process(int process_id , char \* process_name): Funcion que inica un proceso con id process_id y nombre process_name. Guarda toda la informacion en una entrada de la PCB.

- [ ] void cr_finish_process(int process_id): Funcion para terminar un proceso con id process_id. Es importante que antes de que el proceso termine se debe liberar toda la memoria asignada y no debe tener entrada valida en la tabla de PCB.

### Funciones Archivos:
- [ ] CrmsFile\* cr_open(int process_id, char\* file_name, char mode): Funcion para abrir un archivo perteneciente a prcess_id. Si mode es 'r', busca el archivo con nombre file_name y retorna CrmsFile\* que los representa. Si mode es 'w', se verifica que el archivo no exista y se retornaun nuevo CrmsFile\* que lo representa.

- [ ] int cr_write_file(CrmsFile\* file_desc, void \* buffer, int n_bytes): Funcion para escribir archivos. Escribe en el archivo representado por file_desc los n_bytes que se encuentran en la direccion indicada por buffer y retorna la cantidad de Bytes escritos en el archivo (en caso de no terminar). La escritura comienza desde el primer espacio libre dentro de la memoria virtual. La escritura termina cuando:

    - No quedan frames disponibles para continuar
    - Se termina el espacio contiguo en la memoria virtual
    - Se escribieron los n_bytes

- [ ] int cr_read( CrmsFile \* file_desc, void\* buffer, int n_bytes): Funcion para leer archivos. Lee los siguientes n_bytes desde el archivo representado por file_desc y los gurda en la direccion aputnada por buffer. Debe retornar la cantidad de Bytes que efectivamente leyo. La lectura se efectua recorriendo los frames donde se encuentra escrito su contenido, comenzando desde la ultima posicion leida por un llamado cr_read. (Variable global?)
- [ ] void cr_delete(CrmsFile \* file_desc): Funcion para liberar memoria de un archivo perteneciente a un proceso con id process_id. Para esto el archivo debe dejar de aparecer en la memoria virtual del proceso, ademas, si los frames quedan totalmente libres se debe indicar en el frame bitmap que ese frame ya no esta siendo utilizado e invalidar la entrada en la PCB.
- [ ] void cr_close(CrmsFile\* file_desc): Funcion para cerrar archivo. Cierra el archivo indicado por file_desc.

