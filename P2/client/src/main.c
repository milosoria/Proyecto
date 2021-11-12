#include "comunication.h"
#include "conection.h"
#include <stdio.h>
#include <unistd.h>

char *get_input() {
    char *response = calloc(20,sizeof(char));
    int pos = 0;
    while (1) {
        char c = getchar();
        if (c == '\n')
            break;
        response[pos] = c;
        pos++;
    }
    response[pos] = '\0';
    return response;
}

int main(int argc, char *argv[]) {
    /* ./client -i <ip_address> -p <tcp_port> */
    if (argc < 5) {
        printf("The correct use is ./client -i <ip_address> -p <tcp_port>");
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

    // Se prepara el socket
    int server_socket = prepare_socket(IP, PORT);

    // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción
    // al respecto
    int pkg_id;
    while (1) {
        pkg_id = client_receive_id(server_socket);

        if (pkg_id == 1) { // Recibimos un mensaje del servidor
            char *message = client_receive_payload(server_socket);
            printf("%s", message);
            free(message);

        } else if(pkg_id == 2){
            // stdin
            printf("Ingrese su Nombre: ");
            char *response = get_input();
            int option = 1;
            client_send_message(server_socket, option, response);
        } else if (pkg_id ==3){
            char *response = calloc(4,sizeof(char));
            printf("Ingrese la distribucion de los 9 aldeanos iniciales:\n\n");
            printf("Cuantos aldeanos quieres de agricultores?: ");
            response[0] = getchar();
            printf("AGRI: %c\n",response[0]);
            getchar();
            printf("\n");
            printf("Cuantos aldeanos quieres de mineros?: ");
            response[1] = getchar();
            printf("MIN: %c\n",response[1]);
            getc(stdin);
            printf("\n");
            printf("Cuantos aldeanos quieres de ingenieros?: ");
            response[2] = getchar();
            printf("ING: %c\n",response[2]);
            getchar();
            printf("\n");
            printf("Cuantos aldeanos quieres de guerreros?: ");
            response[3] = getchar();
            printf("GUER: %c\n",response[3]);
            getchar();
            printf("\n");
            int option = 2;
            client_send_message(server_socket, option, response);
        } else if (pkg_id==4){
            printf("Ingrese su Jugada: ");
            char response[1] = {getchar() - '0'};
            //char * response = get_input();
            int option = 1;
            client_send_message(server_socket, option, response);
        }
    }

    // Se cierra el socket
    close(server_socket);
    free(IP);
    return 0;
}
