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
    } else {
        printf("#########################\n");
        printf("¡SERVER STARTED!\n");
    }

    char *IP;
    int PORT;
    if (strcmp(argv[1], "-i") == 0) {
        IP = argv[2];
    } else {
        IP = "0.0.0.0";
    }
    if (strcmp(argv[3], "-p") == 0) {
        PORT = atoi(argv[4]);
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

        // nivel aldeanos (3) y ataque,defensa
        players_info->levels[i] = calloc(6, sizeof(int));
        for (int j = 0; j < 6; j++)
            players_info->levels[i][j] = 1;
        // inicializamos todos los niveles en 1
        // inicializamos los recursos en 0
        players_info->resources[i] = calloc(3, sizeof(int));
    }
    int play_selected = 0;
    char *buffer;
    int player = 0;
    int id;
    char *payload;
    // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
    while (1) {
        if (player == 4) {
            player = 0;
        }
        if (players_info->sockets[player] == 0){
            player++;
            continue;
        }
        if (players_info->n_players < 2) {
            players_info->n_players++;
            server_send_message(players_info->sockets[player], 1, "No hay suficientes jugadores para continuar.\n");
            break;
        }
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
        buffer = alloc_for_string("#########################\n"
                "Jugadores, es el turno de %s\n"
                "#########################\n",
                players_info->names[player]);

        log_all(buffer, 1, players_info);
        free(buffer);
        while (play_selected != 8 && play_selected != 7) {
            buffer = alloc_for_string("Jugador %s, que deseas hacer en este turno:\n"
                "(1) Mostrar Información \n"
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
            free(buffer);
            id = server_receive_id(players_info->sockets[player]);
            payload = server_receive_payload(players_info->sockets[player]);
            if (id == -1 || buffer == NULL) {
                buffer = alloc_for_string("El jugador %s se ha desconectado\n",
                        players_info->names[player]);
                log_all(buffer, 1, players_info);
                free(buffer);
                if (players_info->sockets[player]){
                    free(players_info->names[player]);
                    free(players_info->levels[player]);
                    free(players_info->resources[player]);
                    free(players_info->villagers[player]);
                    players_info->sockets[player] = 0;
                }
                players_info->n_players--;
                if (players_info->n_players < 2) {
                    players_info->n_players++;
                    buffer =
                        alloc_for_string("No hay suficientes jugadores para continuar\n"
                                "El juego ha terminado :(\n",
                                NULL);
                    log_all(buffer, 1, players_info);
                    free(payload);
                    goto finish;
                }
                break;
            }
            play_selected = play(player, payload, players_info);
            free(payload);
            // Entregar turno al siguiente jugador activo
        }
        player++;
        play_selected = 0;
        printf("------------------\n");
    }
    finish:
        for (int i = 0; i < players_info->n_players; i++) {
            if (players_info->sockets[i]){
                close(players_info->sockets[i]);
                free(players_info->names[i]);
                free(players_info->levels[i]);
                free(players_info->resources[i]);
                free(players_info->villagers[i]);
            }
        }
        close(players_info->server_socket);
        free(players_info->names);
        free(players_info->villagers);
        free(players_info->resources);
        free(players_info->levels);
        free(players_info);
        return 0;
    goto finish;
}
