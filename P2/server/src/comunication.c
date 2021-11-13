#include "comunication.h"

// LINKS REFERENCIAS
// https://pubs.opengroup.org/onlinepubs/009695399/functions/recv.html
// https://pubs.opengroup.org/onlinepubs/009695399/functions/send.html

int server_receive_id(int client_socket) {
    // Se obtiene solamente el ID del mensaje
    int id = 0;
    recv(client_socket, &id, 1, 0);
    return id;
}

char *server_receive_payload(int client_socket) {
    // Se obtiene el largo del payload
    int len = 0;
    int recieved = recv(client_socket, &len, 1, 0);

    if (!len || recieved < 1) {
        return NULL;
    } else {
        // Se obtiene el payload
        char *payload = calloc(len,sizeof(char));
        int received = recv(client_socket, payload, len, 0);
        if (received <1) return NULL;
        // Se retorna
        return payload;
    }
}

void server_send_message(int client_socket, int pkg_id, char *message) {
    int payloadSize = strlen(message) + 1;
    // printf("payload size: %d\n", payloadSize);
    // Se arma el paquete
    char msg[1 + 1 + payloadSize];
    msg[0] = pkg_id;
    msg[1] = payloadSize;
    memcpy(&msg[2], message, payloadSize);
    // Se envía el paquete
    send(client_socket, msg, 2 + payloadSize, 0);
}
