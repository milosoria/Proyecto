#pragma once
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "comunication.h"

typedef struct players_info{
    int n_players;
    int server_socket;
    int sockets[4];
    // liberar
    char ** names;
    // liberar
    // arreglo de la forma [[agricultores,mineros,ingenieros,guerreros],[...]], tienen que sumar 9
    int ** villagers;
    int ** levels;
    int ** resources;
} PlayersInfo;

PlayersInfo * prepare_sockets_and_get_clients(char * IP, int port);
char * alloc_for_string(char * string,char* src);
