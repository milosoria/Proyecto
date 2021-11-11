#include <unistd.h>
#include <stdio.h>
#include "conection.h"
#include "comunication.h"

char * get_input(){
  char * response = malloc(20);
  int pos=0;
  while (1){
    char c = getchar();
    if (c == '\n') break;
    response[pos] = c;
    pos++;
  }
  response[pos] = '\0';
  return response;
}


int main (int argc, char *argv[]){
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

  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  int pkg_id;
  while (1){
    pkg_id = client_receive_id(server_socket);
    
    if (pkg_id == 1) { //Recibimos un mensaje del servidor
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      free(message);

      printf("¿Qué desea hacer?\n   1)Enviar mensaje al servidor\n   2)Enviar mensaje al otro cliente\n");
      int option = getchar() - '0';
      getchar(); //Para capturar el "enter" que queda en el buffer de entrada stdin
      
      printf("Ingrese su mensaje: ");
      char * response = get_input();

      client_send_message(server_socket, option, response);
    }

    /* if (pkg_id == 2) { //Recibimos un mensaje que proviene del otro cliente */
    /*   char * message = client_receive_payload(server_socket); */
    /*   printf("El otro cliente dice: %s\n", message); */
    /*   free(message); */

    /*   printf("¿Qué desea hacer?\n   1)Enviar mensaje al servidor\n   2)Enviar mensaje al otro cliente\n"); */
    /*   int option = getchar() - '0'; */
    /*   getchar(); //Para capturar el "enter" que queda en el buffer de entrada stdin */
      
    /*   printf("Ingrese su mensaje: "); */
    /*   char * response = get_input(); */

    /*   client_send_message(server_socket, option, response); */
    /* } */
    /* printf("------------------\n"); */
  }

  // Se cierra el socket
  close(server_socket);
  free(IP);
  return 0;
}
