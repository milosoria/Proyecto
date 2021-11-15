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
  printf("que esta llegando?: %s\n",payload );
  //int count = 0;
  int *roles = calloc(4, sizeof(int));
  for (int i=0;i<4;i++){
      // i marca el tipo de aldeano que quertemos hacer

      int n_aldeanos = (int)(payload[i]-'0'); // cuantos aldeanos de cierto tipo quiere fabricar
      //hay que comprobar que cuenta con el saldo para comprarlo
      int comida;
      int oro;
      if (i == 0){
        // si es agricultor
        comida = players_info->resources[player][1];
        if (comida >= n_aldeanos*10){
          players_info->resources[player][1] = players_info->resources[player][1] - n_aldeanos*10;
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;
        } else {
          server_send_message(players_info->sockets[player], 1, "No tienes suficiente material para hacer aldeanos\n");
        }
      } else {
        // si es mineros, ing o guerreros
        comida = players_info->resources[player][1];
        oro = players_info->resources[player][0];
        if (i == 1 && comida >= n_aldeanos*10 && oro >= n_aldeanos*5){
          //minero
          players_info->resources[player][0] = players_info->resources[player][0] - n_aldeanos*5; //oro
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;

          players_info->resources[player][1] = players_info->resources[player][1] - n_aldeanos*10; // comida
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;
        } else if (i == 2 && comida >= n_aldeanos*20 && oro >= n_aldeanos*10){
          //ingeniero
          players_info->resources[player][0] = players_info->resources[player][0] - n_aldeanos*10; //oro
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;

          players_info->resources[player][1] = players_info->resources[player][1] - n_aldeanos*20; // comida
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;
        } else if (i == 3 && comida >= n_aldeanos*10 && oro >= n_aldeanos*10){
          //guerrero
          players_info->resources[player][0] = players_info->resources[player][0] - n_aldeanos*10; //oro
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;

          players_info->resources[player][1] = players_info->resources[player][1] - n_aldeanos*10; // comida
          players_info->villagers[player][i] = players_info->villagers[player][i] + n_aldeanos;
        } else {
          server_send_message(players_info->sockets[player], 1, "No tienes suficiente material para hacer aldeanos\n");
        }

    }//

  }
  //players_info->villagers[player] += roles;
  printf("agricultores: %i\n mineros:%i\n",players_info->villagers[player][0], players_info->villagers[player][1] );
  server_send_message(players_info->sockets[player], 1,"Se han agregado tus aldeanos\n");
}

void level_up(int player, PlayersInfo* players_info) {
  //insertar logica;
}
void attack(int player, PlayersInfo * players_info) {
  //insertar logica;
  server_send_message(players_info->sockets[player], 7, "Escribe el nombre de quien quieres Atacar?:" );
  int id = server_receive_id(players_info->sockets[player]);
  char *payload = server_receive_payload(players_info->sockets[player]);
  printf("Que esta llegando: %s\n", payload);
  printf("Cual es el nombre del jugador?:%s\n",players_info->names[player] );
  printf("player:%i\n",player );

  if (strcmp(payload, players_info->names[player]) != 0){
    printf("llego al IF\n" );
    for (int i = 0; i < players_info->n_players; i++ ){
      printf("llego al for\n" );
      if (strcmp(payload, players_info->names[i]) == 0 && players_info->sockets[i] != 0){
        //hasta aqui se asume que se paso un nombre correcto
        int fuerza_atacante = (players_info->villagers[player][3])*(players_info->levels[player][0]); // n_guerreros*nivel_ataque
        int fuerza_defensor = (players_info->villagers[i][3])*(players_info->levels[player][1])*2; //n_guerreros*nivel_defensa

        if (fuerza_atacante > fuerza_defensor){
          for (int j; j < 4; j++){
            players_info->resources[player][j] = players_info->resources[player][j] +players_info->resources[i][j];
            players_info->resources[i][j] = 0;
            //players_info->names[i] = "HaPerdido";
            //La idea es por cada turno hacer jugar a los que no tengan este nombres
            // y prohibir que ataquen a los que lo tengan
            server_send_message(players_info->sockets[player], 1, "Has ganado el ataque\n\0" );
          }
          //falta eliminar al perdedor
        } else {
          printf("llego al else\n" );
          //atacante pierde la mitad de su tropa
          players_info->villagers[player][3] = (players_info->villagers[player][3])/2;
          server_send_message(players_info->sockets[player], 1, "Has perdido la mitad de tus guerreros\n\0" );
          printf("todo ok\n" );
        }
      } else {
        printf("no se ha encontrado aun, %s -- %s\n",payload,  players_info->names[i]);
        server_send_message(players_info->sockets[player], 1, "No puedes atacar a este jugador, no existe o ya ha perdido.\n\0" );
      }
    }
  } else{
    printf("No puedes atacar por que el jugador ya perdio, o eres tu\n");
    server_send_message(players_info->sockets[player], 1, "No puedes atacar a este jugador, por que el jugador ya perdio, o eres tu\n\0" );
  }



}
void spy(int player, PlayersInfo * players_info) {
  //insertar logica;
  server_send_message(players_info->sockets[player], 8, "Escribe el nombre de quien quieres Espiar?:" );
  int id = server_receive_id(players_info->sockets[player]);
  char *payload = server_receive_payload(players_info->sockets[player]);
  for (int i = 0; i < players_info->n_players; i++ ){
    printf("llego al for\n" );

    if (strcmp(payload, players_info->names[i]) == 0){
      //hasta aqui se asume que se paso un nombre correcto
      int oro_disponible = players_info->resources[player][0];

      if (oro_disponible >= 30){
        players_info->resources[player][0] = (players_info->resources[player][0]) - 30;
        server_send_message(players_info->sockets[player], 1, "TOP SECRET:\n");
        //info nº Guerreros
        char *message =alloc_for_string(" -guerreros:%i\n\n", players_info->villagers[i][3]);
        server_send_message(players_info->sockets[player], 1, message);
        free(message);
        //ataque
        message =alloc_for_string(" -nivel de ataque:%i\n", players_info->levels[i][4]);
        server_send_message(players_info->sockets[player], 1, message);
        free(message);
        //defensa
        message =alloc_for_string(" -nivel de defensa:%i\n", players_info->levels[i][5]);
        server_send_message(players_info->sockets[player], 1, message);
        free(message);
      } else {
        server_send_message(players_info->sockets[player], 1, "No tienes suficiente dinero.\n\0" );

      }



    } else {
      printf("no se ha encontrado aun, %s -- %s\n",payload,  players_info->names[i]);
      server_send_message(players_info->sockets[player], 1, "Buscando.\n\0" );
    }
  }
}
void steal(int player, PlayersInfo * players_info) {
  //insertar logica;
}
void surrender(int player, PlayersInfo * players_info) {
  //insertar logica; no esta completa

  server_send_message(players_info->sockets[player], 1,
    "Te has rendido.\n#########################\n");
}
void pass(int player, PlayersInfo * players_info) {
  server_send_message(players_info->sockets[player], 1,
    "Has pasado.\n#########################\n");
}
