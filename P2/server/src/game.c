#include "game.h"

void recollect(int player, PlayersInfo *players_info) {
    char buffer[80];

    int incomes[] = {
        players_info->villagers[player][0] * players_info->levels[player][0] * 2,
        players_info->villagers[player][1] * players_info->levels[player][1] * 2,
        players_info->villagers[player][2] * players_info->levels[player][2]};

    sprintf(buffer, "Jugador %s, has recibido %i comida, %i oro, %i ciencia\n",
            players_info->names[player], incomes[0], incomes[1], incomes[2]);

    server_send_message(players_info->sockets[player], 1, buffer);

    players_info->resources[player][0] +=
        players_info->villagers[player][0] * players_info->levels[player][0] * 2;

    players_info->resources[player][1] +=
        players_info->villagers[player][1] * players_info->levels[player][1] * 2;

    players_info->resources[player][2] +=
        players_info->villagers[player][2] * players_info->levels[player][2];
}

void log_all(char *message, int pkg_id, PlayersInfo *players_info) {
    for (int i = 0; i < players_info->n_players; i++) {
        if (players_info->sockets[i])
            server_send_message(players_info->sockets[i], pkg_id, message);
    }
}

int play(int player, char *payload, PlayersInfo * players_info) {
    if (strcmp(payload, "1")== 0) {
        log_info(player, players_info);
        return 1;
    } else if (strcmp(payload, "2") == 0) {
        create_villager(player, players_info);
        return 2;
    } else if (strcmp(payload, "3") == 0) {
        level_up(player, players_info);
        return 3;
    } else if (strcmp(payload, "4") == 0) {
        attack(player, players_info);
        return 4;
    } else if (strcmp(payload, "5") == 0) {
        spy(player, players_info);
        return 5;
    } else if (strcmp(payload, "6") == 0) {
        steal(player, players_info);
        return 6;
    } else if (strcmp(payload, "7") == 0) {
        surrender(player, players_info);
        return 7;
    } else {
        pass(player, players_info); // NO hay q hacer nada, ya esta lista
        return 8;
    }
}
void log_info(int player, PlayersInfo * players_info) {
  //CANT DE RECURSOS
  server_send_message(players_info->sockets[player], 1, "Cantidad de recursos:\n");
  char *message =alloc_for_string(" -Oro:%i\n", players_info->resources[player][0]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -Comida:%i\n", players_info->resources[player][1]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -Ciencia:%i\n", players_info->resources[player][2]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);

  //ALDEANOS POR rol
  server_send_message(players_info->sockets[player], 1, "Aldeanos por rol:\n");
  message =alloc_for_string(" -agricultores:%i\n", players_info->villagers[player][0]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -mineros:%i\n", players_info->villagers[player][1]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -ingenieros:%i\n", players_info->villagers[player][2]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -guerreros:%i\n\n", players_info->villagers[player][3]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);

  //NIVELES ALCANZADOS
  server_send_message(players_info->sockets[player], 1, "Niveles alcanzados:\n");
  message =alloc_for_string(" -agricultores:%i\n", players_info->levels[player][0]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -mineros:%i\n", players_info->levels[player][1]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -ingenieros:%i\n", players_info->levels[player][2]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -guerreros:%i\n", players_info->levels[player][3]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -ataque:%i\n", players_info->levels[player][4]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);
  message =alloc_for_string(" -defensa:%i\n", players_info->levels[player][5]);
  server_send_message(players_info->sockets[player], 1, message);
  free(message);

  server_send_message(players_info->sockets[player], 1, "#########################\n");

}

void create_villager(int player, PlayersInfo * players_info) {
  //insertar logica;
  server_send_message(players_info->sockets[player], 6, "# Ingrese la cantidad de aldeanos a agregar por rol:\n\n" );
  int id = server_receive_id(players_info->sockets[player]);
  char *payload = server_receive_payload(players_info->sockets[player]);

}
void level_up(int player, PlayersInfo* players_info) { 
    char buffer[512];
    int condicion = 1;
    while (condicion) {
        sprintf(buffer, "Ingrese cual aspecto desea mejorar:\n(1) Nivel Agricultores (Nivel %d).\n(2) Nivel Mineros (Nivel %d).\n(3) Nivel Ingenieros (Nivel %d).\n(4) Nivel Ataque (Nivel %d).\n(5) Nivel Defensa (Nivel %d).\n(6) Volver al Menú.\n", players_info->levels[player][0], players_info->levels[player][1], players_info->levels[player][2], players_info->levels[player][3], players_info->levels[player][4]);
        server_send_message(players_info->sockets[player], 11, buffer);
        int id = server_receive_id(players_info->sockets[player]);
        int payload = atoi(server_receive_payload(players_info->sockets[player]));
        printf("pkg_id: %d.\n", id);
        printf("payload niveles: %d.\n", payload);
        if (payload > 0 && payload < 6) {
            if (players_info->levels[player][payload - 1] == 5) {
                sprintf(buffer, "Este aspecto ya está mejorado al máximo, no se puede seguir mejorando.\n");
                server_send_message(players_info->sockets[player], 1, buffer);
            }
            else {
                int costo = (players_info->levels[player][payload - 1]) * 10;
                int food = players_info->resources[player][0];
                int gold = players_info->resources[player][1];
                int science = players_info->resources[player][2];
                if (food >= costo && gold >= costo && science >= costo) {
                    players_info->resources[player][0] -= costo;
                    players_info->resources[player][1] -= costo;
                    players_info->resources[player][2] -= costo;
                    players_info->levels[player][payload - 1] += 1;
                    sprintf(buffer, "Aspecto mejorado al nivel %d con éxito.\n", players_info->levels[player][payload - 1]);
                    server_send_message(players_info->sockets[player], 1, buffer);
                }
                else {
                    sprintf(buffer, "Recursos insuficientes para mejorar este aspecto.\nNecesitas %d comida, %d oro y %d ciencia. Tienes %d comida, %d oro y %d ciencia.\n", 
                    costo, costo, costo, food, gold, science);
                    server_send_message(players_info->sockets[player], 1, buffer);
                }
            } 
        }
        else if (payload == 6){
            condicion = 0;
        }
        else {
            sprintf(buffer, "El valor ingresado es inválido, por favor escoja uno valor entre 1 y 6.\n");
            server_send_message(players_info->sockets[player], 1, buffer);
        }
    }
}
void attack(int player, PlayersInfo * players_info) {
  //insertar logica;
}
void spy(int player, PlayersInfo * players_info) {
  //insertar logica;
}
void steal(int player, PlayersInfo * players_info) {
  //insertar logica;
}
void surrender(int player, PlayersInfo * players_info) {
  //insertar logica;

  server_send_message(players_info->sockets[player], 1,
    "Te has rendido.\n#########################\n");
}
void pass(int player, PlayersInfo * players_info) {
  server_send_message(players_info->sockets[player], 1,
    "Has pasado.\n#########################\n");
}
