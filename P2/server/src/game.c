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

    players_info->resources[player][0] += incomes[0];
    players_info->resources[player][1] += incomes[1];
    players_info->resources[player][2] += incomes[2];
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
    char *message =alloc_for_string("\t-Oro:%i\n", players_info->resources[player][0]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-Comida:%i\n", players_info->resources[player][1]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-Ciencia:%i\n\n", players_info->resources[player][2]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);

    //ALDEANOS POR rol
    server_send_message(players_info->sockets[player], 1, "Aldeanos por rol:\n");
    message =alloc_for_string("\t-agricultores:%i\n", players_info->villagers[player][0]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-mineros:%i\n", players_info->villagers[player][1]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-ingenieros:%i\n", players_info->villagers[player][2]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-guerreros:%i\n\n", players_info->villagers[player][3]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);

    //NIVELES ALCANZADOS
    server_send_message(players_info->sockets[player], 1, "Niveles alcanzados:\n");
    message =alloc_for_string("\t-agricultores:%i\n", players_info->levels[player][0]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-mineros:%i\n", players_info->levels[player][1]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-ingenieros:%i\n", players_info->levels[player][2]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-guerreros:%i\n", players_info->levels[player][3]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-ataque:%i\n", players_info->levels[player][4]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);
    message =alloc_for_string("\t-defensa:%i\n\n", players_info->levels[player][5]);
    server_send_message(players_info->sockets[player], 1, message);
    free(message);

    server_send_message(players_info->sockets[player], 1, "#########################\n");

}

void create_villager(int player, PlayersInfo * players_info) {
    //insertar logica;
    server_send_message(players_info->sockets[player], 6, "# Ingrese la cantidad de aldeanos a agregar por rol:\n\n" );
    int id = server_receive_id(players_info->sockets[player]);
    char *payload = server_receive_payload(players_info->sockets[player]);
    //int count = 0;
    int *roles = calloc(4, sizeof(int));
    for (int i=0;i<4;i++){
        // i marca el tipo de aldeano que queremos hacer

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
                server_send_message(players_info->sockets[player], 1, "No tienes suficiente material para hacer aldeanos.\n");
                free(payload);
                return;
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
                server_send_message(players_info->sockets[player], 1, "No tienes suficiente material para hacer aldeanos.\n");
                free(payload);
                return;
            }

        }//
    }
    //players_info->villagers[player] += roles;
    printf("agricultores: %i\n mineros:%i\n",players_info->villagers[player][0], players_info->villagers[player][1] );
    server_send_message(players_info->sockets[player], 1,"Se han agregado tus aldeanos\n");
    free(payload);
}

void level_up(int player, PlayersInfo* players_info) { 
    char buffer[512];
    int condicion = 1;
    while (condicion) {
        sprintf(buffer, "Ingrese cual aspecto desea mejorar:\n(1) Nivel Agricultores (Nivel %d).\n(2) Nivel Mineros (Nivel %d).\n(3) Nivel Ingenieros (Nivel %d).\n(4) Nivel Ataque (Nivel %d).\n(5) Nivel Defensa (Nivel %d).\n(6) Volver al Menú.\n", players_info->levels[player][0], players_info->levels[player][1], players_info->levels[player][2], players_info->levels[player][3], players_info->levels[player][4]);
        server_send_message(players_info->sockets[player], 11, buffer);
        int id = server_receive_id(players_info->sockets[player]);
        int payload = atoi(server_receive_payload(players_info->sockets[player]));
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
    } else {
        printf("No puedes atacar por que el jugador ya perdio, o eres tu\n");
        server_send_message(players_info->sockets[player], 1, "No puedes atacar a este jugador, por que el jugador ya perdio, o eres tu\n\0" );
    }
    free(payload);
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
                char *message =alloc_for_string("\t-guerreros:%i\n\n", players_info->villagers[i][3]);
                server_send_message(players_info->sockets[player], 1, message);
                free(message);
                //ataque
                message =alloc_for_string("\t-nivel de ataque:%i\n", players_info->levels[i][4]);
                server_send_message(players_info->sockets[player], 1, message);
                free(message);
                //defensa
                message =alloc_for_string("\t-nivel de defensa:%i\n", players_info->levels[i][5]);
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
    free(payload);
}

void steal(int player, PlayersInfo * players_info) {
    char buffer[512];
    int id;
    int payload;
    char* payload_char;
    int condicion = 1;
    while (condicion) {
        int count = 0;
        int stealed_player = 0;
        server_send_message(players_info->sockets[player], 1, "Escoja a que jugador desea robar.\n");
        for (int i = 0; i < 4; i++) {
            if (players_info->sockets[i] && i != player) {
                count++;
                sprintf(buffer, "(%d) %s.\n", count, players_info->names[i]);
                server_send_message(players_info->sockets[player], 1, buffer);
            }
        }
        sprintf(buffer, "(%d) Volver al Menú.\n", count + 1);
        server_send_message(players_info->sockets[player], 11, buffer);
        id = server_receive_id(players_info->sockets[player]);
        payload = atoi(server_receive_payload(players_info->sockets[player]));
        if (payload > 0 && payload <= count) {
            for (int i = 0; i < 4; i++) {
                if (players_info->sockets[i] && i != player) {
                    stealed_player++;
                    if (stealed_player == payload){
                        stealed_player = i;
                        break;
                    } 
                }
            }
            int condicion_2 = 1;
            while (condicion_2) {
                server_send_message(players_info->sockets[player], 1, "Escoja que desea robar.\n");
                sprintf(buffer, "(1) Comida.\n(2) Oro.\n(3) Regresar.\n");
                server_send_message(players_info->sockets[player], 11, buffer);
                id = server_receive_id(players_info->sockets[player]);
                payload_char = server_receive_payload(players_info->sockets[player]);
                payload = atoi(payload_char);
                free(payload_char);
                if (payload > 0 && payload < 3){
                    if (players_info->resources[player][2] >= 10){
                        players_info->resources[player][2] -= 10;
                        int stealed = players_info->resources[stealed_player][payload - 1] / 10;
                        players_info->resources[player][payload - 1] += stealed;
                        players_info->resources[stealed_player][payload - 1] -= stealed;
                        if (payload == 1){
                            sprintf(buffer, "Has robado con éxito %d comida a %s.\n", stealed, players_info->names[stealed_player]);
                            server_send_message(players_info->sockets[player], 1, buffer);
                            sprintf(buffer, "%s te ha robado %d comida.\n", players_info->names[player], stealed);
                            server_send_message(players_info->sockets[stealed_player], 1, buffer);
                        } else{
                            sprintf(buffer, "Has robado con éxito %d oro a %s.\n", stealed, players_info->names[stealed_player]);
                            server_send_message(players_info->sockets[player], 1, buffer);
                            sprintf(buffer, "%s te ha robado %d oro.\n", players_info->names[player], stealed);
                            server_send_message(players_info->sockets[stealed_player], 1, buffer);
                        }
                    }
                    else{
                        server_send_message(players_info->sockets[player], 1, "No tienes suficiente ciencia.\n");
                    }
                } else if (payload == 3) {
                    condicion_2 = 0;
                } else {
                    server_send_message(players_info->sockets[player], 1, "Valor ingresado invalido.\n");
                }
            }
        }
        else if (payload == count + 1){
            condicion = 0;
        }
        else{
            server_send_message(players_info->sockets[player], 1, "Valor ingresado invalido.\n");
        }
    }
}

void surrender(int player, PlayersInfo * players_info) {
    server_send_message(players_info->sockets[player], 1, "Te has rendido.\n#########################\n");
    server_send_message(players_info->sockets[player], 12, "");
    players_info->n_players--;
    players_info->sockets[player] = 0;
    free(players_info->names[player]);
    free(players_info->levels[player]);
    free(players_info->resources[player]);
    free(players_info->villagers[player]);
}

void pass(int player, PlayersInfo * players_info) {
    server_send_message(players_info->sockets[player], 1,
            "Has pasado.\n#########################\n");
}
