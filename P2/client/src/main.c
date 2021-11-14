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
    if (strcmp(argv[1], "-i")==0) {
        IP = argv[2];
    } else {
        IP = "0.0.0.0";
    }
    if (strcmp(argv[3], "-p")==0) {
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
            char * message = client_receive_payload(server_socket);
            printf("%s", message);
            free(message);
        } else if(pkg_id == 2){
            // stdin
            printf("Ingrese su Nombre: ");
            char *response = get_input();
            int option = 1;
            client_send_message(server_socket, option, response);
        } else if (pkg_id ==3){
            char *response = calloc(5,sizeof(char));
            printf("Ingrese la distribucion de los 9 aldeanos iniciales:\n\n");
            printf("Cuantos aldeanos quieres de agricultores?: ");
            response[0] = getchar();
            getchar();
            printf("\n");
            printf("Cuantos aldeanos quieres de mineros?: ");
            response[1] = getchar();
            getchar();
            printf("\n");
            printf("Cuantos aldeanos quieres de ingenieros?: ");
            response[2] = getchar();
            getchar();
            printf("\n");
            printf("Cuantos aldeanos quieres de guerreros?: ");
            response[3] = getchar();
            getchar();
            printf("\n");
            response[4] = '\0';
            int option = 2;
            client_send_message(server_socket, option, response);
        } else if (pkg_id==4){
            char * message = client_receive_payload(server_socket);
            printf("%s",message);
            printf("Ingrese su Jugada: ");
            char response[1];
            scanf("%s",response);
            response[strcspn(response, "\n")] = 0;
            printf("response %s\n",response);
            int option = 1;
            client_send_message(server_socket, option, response);
        } else if (pkg_id == 5){
            printf("Desea iniciar la partida? Ingrese 1 cuando este listo: ");
            char enter;
            scanf("%s",&enter);
            printf("\n");
            char response[1]="1";
            int option = 0;
            client_send_message(server_socket, option, response);
        }else if (pkg_id == 6){
          char *response = calloc(5,sizeof(char));
          char * algo= client_receive_payload(server_socket);
          printf("%s\n",algo );

          //printf("Ingrese la cantidad de aldeanos a agregar por rol:\n\n");
          printf("N° agricultores nuevos?: ");
          response[0] = getchar();
          getchar();
          printf("\n");
          printf("N° mineros nuevos?: ");
          response[1] = getchar();
          getchar();
          printf("\n");
          printf("N° ingenieros nuevos?: ");
          response[2] = getchar();
          getchar();
          printf("\n");
          printf("N° guerreros nuevos?: ");
          response[3] = getchar();
          getchar();
          printf("\n");
          response[4] = '\0';
          int option = 5;
          printf("%s\n",response );
          client_send_message(server_socket, option, response);
        }

    }

    // Se cierra el socket
    close(server_socket);
    free(IP);
    return 0;
}
