#include "conection.h"
#include <errno.h>
#include <string.h>
#define SA struct sockaddr

// LINKS REFERENCIAS:
// https://www.man7.org/linux/man-pages/man2/socket.2.html
// https://man7.org/linux/man-pages/man7/socket.7.html
// https://www.man7.org/linux/man-pages/man3/setsockopt.3p.html
// https://man7.org/linux/man-pages/man2/setsockopt.2.html
// https://linux.die.net/man/3/htons
// https://linux.die.net/man/3/inet_aton
// https://www.howtogeek.com/225487/what-is-the-difference-between-127.0.0.1-and-0.0.0.0/
// https://www.man7.org/linux/man-pages/man2/bind.2.html
// https://www.man7.org/linux/man-pages/man2/accept.2.html

PlayersInfo *prepare_sockets_and_get_clients(char *IP, int port) {

    // Se define la estructura para almacenar info del socket del servidor al
    // momento de su creación
    struct sockaddr_in server_addr;
    int MAX_N_PLAYERS = 4;

    // Se solicita un socket al SO, que se usará para escuchar conexiones
    // entrantes
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    printf("Server Socket is %i\n", server_socket);

    // Se configura el socket a gusto (recomiendo fuertemente el REUSEPORT!)
    int opt = 1;

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) <
            0) {
        printf("Problem setting the socket up\n");
        exit(EXIT_FAILURE);
    }

    // Se guardan el puerto e IP en la estructura antes definida
    // Se llena la direccion con 0
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // Converts IP adress into binary form (network byte order) and saves it in
    // the ptr given in arg2
    inet_aton(IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    // Se le asigna al socket del servidor un puerto y una IP donde escuchar
    if (bind(server_socket, (SA *)&server_addr,
                sizeof(server_addr)) < 0) {
        printf("Binding failed! Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Se coloca el socket en modo listening
    // Seteamos el maximo de conecciones en la cola, como el maximo de jugadores
    // posibles
    if (listen(server_socket, MAX_N_PLAYERS) < 0) {
        printf("Listening failed!\n");
        exit(EXIT_FAILURE);
    }
    // Se definen las estructuras para almacenar info sobre los sockets de los
    // clientes
    struct sockaddr_in client1_addr;
    struct sockaddr_in client2_addr;
    struct sockaddr_in client3_addr;
    struct sockaddr_in client4_addr;
    struct sockaddr_in clients_addrs[4] = {client1_addr, client2_addr,
        client3_addr, client4_addr};
    socklen_t addr_size = sizeof(client1_addr);
    // Se inicializa una estructura propia para guardar los n°s de sockets de los
    // clientes.
    PlayersInfo *players_info = malloc(sizeof(PlayersInfo));
    // es necesario liberar el sockets_clients
    *players_info = (PlayersInfo){
        .n_players = 0,
        .sockets = {0},
        .server_socket = server_socket,
        .names = calloc(4, sizeof(char *)),
        .villagers = calloc(4, sizeof(int *))};

    char *buffer;
    int names_count=0, villagers_count=0, advised = 0;
    char to_send[200];

    int id, to_read, cli_sock;
    fd_set read_set;
    // este sera el file_descriptor mas large del set:
    int max_sock = server_socket;
    // indicador si es que el jugador lider determina empezar y se cumplen las
    // restricciones
    while (1) {

        if (villagers_count == players_info->n_players &&
                names_count == players_info->n_players && !advised && players_info->n_players>1) {
                   printf("new loop iteratin, villagers: %i, names: %i, advised: %i\n",villagers_count == players_info->n_players,names_count==players_info->n_players,!advised); 
                   server_send_message(
                    players_info->sockets[0], 5,
                    "Por favor empezar el juego, todos l@s jugadores esperan\n");
            advised = 1;
        } // recorremos los sockets actuales en cada iteracion
        // inicializamos el set de sockets vacio
        FD_ZERO(&read_set);
        // luego ponemos el listener (server) dentro del set
        FD_SET(server_socket, &read_set);
        // agreagamos socket de clientes al set
        for (int i = 0; i < players_info->n_players; i++) {
            // socket descriptor
            cli_sock = players_info->sockets[i];

            // if valid socket descriptor then add to read list
            if (cli_sock > 0)
                FD_SET(cli_sock, &read_set);

            // highest file descriptor number, need it for the select function
            if (cli_sock > max_sock)
                max_sock = cli_sock;
        }

        to_read = select(max_sock + 1, &read_set, NULL, NULL, NULL);
        if (to_read < 0 && errno != EINTR) {
            printf("select failed: errno = %s\n", strerror(errno));
        } 

        if (FD_ISSET(server_socket, &read_set)) {
            cli_sock = accept(server_socket, (SA *)&clients_addrs[players_info->n_players], &addr_size);
            if (cli_sock < 0) {
                printf("accept error! %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket descriptor %d\n", cli_sock);
            // mandamos un mensaje ping para que el codigo del cliente continue
            send(cli_sock, "1", 1, 0);
            players_info->n_players++;
            for (int player = 0; player < players_info->n_players; player++) {

                if (players_info->sockets[player] == 0) {
                    players_info->sockets[player] = cli_sock;
                    // aumentamos uno a la cuenta de los jugadores
                    // redefinimos el max_sock si es que existe uno mayor
                    if (cli_sock > max_sock) {
                        max_sock = cli_sock;
                    }
                    if (players_info->n_players == 1) {
                        // si es el primer jugador, entonces es asignado como el jugador
                        // lider
                        server_send_message(
                                players_info->sockets[player], 1,
                                "Bienvenido nuevo jugador! Eres el primero y por ende, el "
                                "jugador jefe\n\n"
                                "Es tu responsabilidad solicitar cuando inciar el juego\n"
                                "Por favor sigue las instrucciones con cuidado\n");
                    } else {
                        // a cualquier otro jugador, le damos la bienvenida
                        server_send_message(players_info->sockets[player], 1,
                                "Bienvenido nuevo jugador! Por favor sigue"
                                "las instrucciones con cuidado\n\n");
                    }
                    // a todos los jugadores les pedimos su nombre y que distribuyan a sus
                    // aldeanos
                    server_send_message(players_info->sockets[player], 2, "Debes enviar tu nombre de jugador\n");
                    server_send_message(
                            players_info->sockets[player], 3,
                            "Tienes 9 aldeanos iniciales, como deseas distribuirlos?\n\n");
                    FD_SET(cli_sock,&read_set);
                    break;
                }
            }
        }

        for (int player = 0; player < players_info->n_players; player++) {
            cli_sock = players_info->sockets[player];
            // comprobamos i pertenece al set read_set
            if (FD_ISSET(cli_sock, &read_set)) {
                // recibimos mensaje
                id = server_receive_id(cli_sock);
                buffer = server_receive_payload(cli_sock);
                int len = strlen(buffer)+1;
                // es necesario liberar el buffer despues
                if (id == 1) {
                    // hay que liberarlo despues
                    players_info->names[player] = malloc(len);
                    memcpy(players_info->names[player], buffer,len);
                    sprintf(to_send, "Se ha conectado un nuevo jugador con nombre %s\n",
                            buffer);
                    server_send_message(players_info->sockets[0], 1, to_send);
                    names_count++;
                } else if (id == 2) {
                    int count = 0;
                    int *roles = calloc(4, sizeof(int));
                    for (int i=0;i<4;i++){
                        count += (int)(buffer[i]-'0');
                    }
                    // repartio correctamente el numero de aldeanos
                    if (count == 9) {
                        memcpy(roles,buffer,len);
                        players_info->villagers[player] = roles;
                        villagers_count++;
                        server_send_message(players_info->sockets[player], 1,
                                "Su distribucion ha sido guardada, comenzara la partida cuando el jugador lider lo indique\n");
                    } else {
                        server_send_message(players_info->sockets[player], 1,
                                "Distribuya correctamente sus 9 aldeanos\n");
                        server_send_message(
                                players_info->sockets[player], 3,
                                "Tienes 9 aldeanos iniciales, como deseas distribuirlos?\n\n");
                    }
                } else if (id == 0) {
                    // el jugador jefe, solicita iniciar el juego
                    if (cli_sock == players_info->sockets[0]) {
                        // comprobamos si todos definieron sus aldeanos e ingresaron nombre
                        if (villagers_count == players_info->n_players &&
                                names_count == players_info->n_players) {
                            for (int k = 0; k < players_info->n_players; k++) {
                                server_send_message(players_info->sockets[k], 0,
                                        "Comienza el juego, PREPARAD@S???\n");
                            }
                            return players_info;
                        } else {
                            server_send_message(players_info->sockets[0], 1,
                                    "Faltan jugadores por definir sus "
                                    "nombres y distribuir sus aldeanos\n");
                        }
                    }
                }
                free(buffer);
            }
        }
    }
}
