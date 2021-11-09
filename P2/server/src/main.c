#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"

char * revert(char * message){
    //Se invierte el mensaje

    int len = strlen(message) + 1;
    char * response = malloc(len);

    for (int i = 0; i < len-1; i++)
    {
        response[i] = message[len-2-i];
    }
    response[len-1] = '\0';
    return response;
}

int main(int argc, char *argv[]){
    /* ./server -i <ip_address> -p <tcp_port> */
    if (argc < 5){
        printf("The correct use is ./server -i <ip_address> -p <tcp_port>");
    }

    char * IP;
    int PORT;
    if (strcmp(argv[1],"-i")){
        IP = argv[2]; 
    } else{
        IP = "0.0.0.0";
    }
    if (strcmp(argv[3],"-p")){
        PORT = (int)*argv[4];
    } else {
        PORT = 8080;
    }

    // Se crea el servidor y se obtienen los sockets de ambos clientes.
    PlayersInfo * players_info = prepare_sockets_and_get_clients(IP, PORT);

    // Le enviamos al primer cliente un mensaje de bienvenida
    server_send_message(players_info->sockets[0], 1, "Bienvenido Jugador 1!!");

    // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
    while (1)
    {
        // usar select() para escuchar multiples sockets y actuar en base al primer envio

        /* // Se obtiene el paquete del cliente 1 */
        /* int msg_code = server_receive_id(players_info->sockets[my_attention]); */

        /* if (msg_code == 1) //El cliente me envió un mensaje a mi (servidor) */
        /* { */
        /*     char * client_message = server_receive_payload(players_info->sockets[my_attention]); */
        /*     printf("El cliente %d dice: %s\n", my_attention+1, client_message); */

        /*     // Le enviaremos el mismo mensaje invertido jeje */
        /*     char * response = revert(client_message); */

        /*     // Le enviamos la respuesta */
        /*     server_send_message(players_info->sockets[my_attention], 1, response); */
        /* } */
        /* else if (msg_code == 2){ //El cliente le envía un mensaje al otro cliente */
        /*     char * client_message = server_receive_payload(players_info->sockets[my_attention]); */
        /*     printf("Servidor traspasando desde %d a %d el mensaje: %s\n", my_attention+1, ((my_attention+1)%2)+1, client_message); */

        /*     // Mi atención cambia al otro socket */
        /*     my_attention = (my_attention + 1) % 2; */

        /*     server_send_message(players_info->sockets[my_attention], 2, client_message); */
        /* } */
        printf("------------------\n");
    }

    return 0;
}
