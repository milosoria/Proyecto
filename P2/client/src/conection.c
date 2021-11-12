#include "conection.h"

int prepare_socket(char * IP, int PORT){

    struct sockaddr_in server_addr;

    // Se crea el socket que se conectará con el servidor
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Se definen los parametros del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_aton(IP, &server_addr.sin_addr);
    // Se pide una conexión al servidor
    int ret = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buf[1];
    recv(client_socket, buf, 1, MSG_WAITFORONE);
    printf("First ping recieved, ready to operate\n");
    return client_socket;
}
