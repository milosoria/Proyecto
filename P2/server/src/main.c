#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"

// variables globales de niveles y recursos de los jugadores
// [[lvl_agr,lvl_min,lvl_ing,lvl_guer],[...]]
int ** LEVELS;
// [[n_comida,n_oro,n_ciencia],[...]]
int ** RESOURCES;

int main(int argc, char *argv[]){
    /* ./server -i <ip_address> -p <tcp_port> */
    if (argc < 5){
        printf("The correct use is ./server -i <ip_address> -p <tcp_port>");
    }

    char * IP;
    int PORT;
    if (strcmp(argv[1],"-i")){
        IP = argv[2]; 
    } else{
        IP = "0.0.0.0";
    }
    if (strcmp(argv[3],"-p")){
        PORT = (int)*argv[4];
    } else {
        PORT = 8080;
    }

    // Se crea el servidor y se obtienen los sockets de ambos clientes.
    PlayersInfo * players_info = prepare_sockets_and_get_clients(IP, PORT);
    LEVELS = calloc(players_info->n_players,sizeof(int*));
    RESOURCES = calloc(players_info->n_players,sizeof(int*));
    for (int i=0; i<players_info->n_players;i++){
        server_send_message(players_info->sockets[i],1,
                "#########################\n"
                "El juego ha comenzado\n"
                "#########################\n");
        // inicializamos los valores de recursos y estadisticas

        LEVELS[i] = calloc(6,sizeof(int));
        // inicializamos todos los niveles en 1
        memset(LEVELS[i],1,4);
        // inicializamos los recursos en 0
        RESOURCES[i]= calloc(3,sizeof(int));
    }
    int turn = 0;
    // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
    while (1)
    {
        for (int i=0; i<players_info->n_players;i++){
            // Flujo Turno:
            // 1. Recolectar recursos del jugador turn
            // 2. Elegir una opcion:
                //  - Mostrar informacion (funcion)
                //  - Crear aldeano (funcion)
                //  - Subir de nivel (funcion)
                //  - Atacar (funcion)
                //  - Espiar (funcion)
                //  - Robar (funcion)
                //  - Pasar (funcion)
                //  - Rendirse (funcion)
            // Entregar turno al siguiente jugador activo
        }
        printf("------------------\n");
    }

    return 0;
}
