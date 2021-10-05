# Proyecto (Lunes 11 de Octubre, 2021):

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

- Valid (1 Byte): 0x01
