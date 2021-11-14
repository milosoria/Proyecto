#include "comunication.h"

int client_receive_id(int client_socket){
    // Se obtiene solamente el ID del mensaje
    int id = 0;
    int received = recv(client_socket, &id, 1, 0);
    if (received) return id;
    else return -1;
}

char * client_receive_payload(int client_socket){
    // Se obtiene el largo del payload
    int len = 0;
    int recieved = recv(client_socket, &len, 1, 0);

    if (!len || recieved < 1) {
        return NULL;
    } else {
        // Se obtiene el payload
        char *payload = calloc(len,sizeof(char));
        int received = recv(client_socket, payload, len, 0);
        if (received < 1) return NULL;
        // Se retorna
        return payload;
    }
}

void client_send_message(int client_socket, int pkg_id, char * message){
    int payloadSize = strlen(message) + 1; //+1 para considerar el caracter nulo. 
    //Esto solo es válido para strings, Ustedes cuando armen sus paquetes saben exactamente cuantos bytes tiene el payload.
    // Se arma el paquete
    char msg[1+1+payloadSize];
    msg[0] = pkg_id;
    msg[1] = payloadSize;
    memcpy(&msg[2], message, payloadSize);
    // Se envía el paquete
    send(client_socket, msg, 2+payloadSize, 0);
}
