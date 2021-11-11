#include "comunication.h"
#include "conection.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* [agr,min,ing,guer] */
/* [food,gold,science] */
int main(int argc, char *argv[]) {
    /* ./server -i <ip_address> -p <tcp_port> */
    if (argc < 5) {
        printf("The correct use is ./server -i <ip_address> -p <tcp_port>");
    }

    char *IP;
    int PORT;
    if (strcmp(argv[1], "-i")) {
        IP = argv[2];
    } else {
        IP = "0.0.0.0";
    }
    if (strcmp(argv[3], "-p")) {
        PORT = (int)*argv[4];
    } else {
        PORT = 8080;
    }

    // Se crea el servidor y se obtienen los sockets de ambos clientes.
    PlayersInfo *players_info = prepare_sockets_and_get_clients(IP, PORT);
    players_info->levels = calloc(players_info->n_players, sizeof(int *));
    players_info->resources = calloc(players_info->n_players, sizeof(int *));
    for (int i = 0; i < players_info->n_players; i++) {
        server_send_message(players_info->sockets[i], 1,
                "#########################\n"
                "El juego ha comenzado\n"
                "#########################\n");
        // inicializamos los valores de recursos y estadisticas

        players_info->levels[i] = calloc(6, sizeof(int));
        // inicializamos todos los niveles en 1
        memset(players_info->levels[i], 1, 4);
        // inicializamos los recursos en 0
        players_info->resources[i] = calloc(3, sizeof(int));
    }
    int player = 0;
    int play_selected = 0;
    char buffer[300];
    int id;
    char *payload;
    // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
    while (1) {
        for (int i = 0; i < players_info->n_players; i++) {
            // Flujo Turno:
            // 1. Recolectar recursos del jugador turn
            /* * Oro: cantidad de mineros X nivel de mineros X 2 */
            /* * Comida: cantidad de agricultores X nivel de agricultores X 2 */
            /* * Ciencia: cantidad de ingenieros X nivel de ingenieros */
            recollect(player, players_info);
            // 2. Elegir una opcion:
            //  - Mostrar informacion (funcion)
            //  - Crear aldeano (funcion)
            //  - Subir de nivel (funcion)
            //  - Atacar (funcion)
            //  - Espiar (funcion)
            //  - Robar (funcion)
            //  - Pasar (funcion)
            //  - Rendirse (funcion)
            sprintf(buffer,
                    "#########################\n"
                    "Jugadores, es el turno de %s\n"
                    "#########################\n",
                    players_info->names[player]);
            log_all(buffer, 1, players_info);
            sprintf(buffer,
                    "#########################\n"
                    "Jugador %s, que deseas hacer en este turno:\n"
                    "(1) Mostrar Informacion \n"
                    "(2) Crear Aldeano \n"
                    "(3) Subir de Nivel \n"
                    "(4) Atacar \n"
                    "(5) Espiar \n"
                    "(6) Robar \n"
                    "(7) Rendirse \n"
                    "(8) Pasar \n"
                    "#########################\n",
                    players_info->names[player]);
            server_send_message(players_info->sockets[player], 4, buffer);
            while (play_selected != 8) {
                id = server_receive_id(players_info->sockets[player]);
                payload = server_receive_payload(players_info->sockets[player]);
                play_selected = play(player, payload, players_info);
                free(payload);
            }
            // Entregar turno al siguiente jugador activo
        }
        printf("------------------\n");
    }

    return 0;
}
