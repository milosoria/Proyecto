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
  //insertar logica;
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
