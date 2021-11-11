#include "conection.h"
#include <errno.h>

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
    if (bind(server_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        printf("Binding failed! Error: %i\n", errno);
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
    PlayersInfo *clients_sockets = malloc(sizeof(PlayersInfo));
    // es necesario liberar el sockets_clients
    *clients_sockets = (PlayersInfo){.n_players = 0,
        .sockets = {0},
        .server_socket = server_socket,
        .names = calloc(4, sizeof(char *)),
        .villagers = calloc(4, sizeof(int *))};

    int to_read;
    char *buffer;
    int id;
    int names_count = 0;
    int villagers_count = 0;
    int advised = 0;
    char to_send[200];

    fd_set read_set;
    int cli_sock;
    // este sera el file_descriptor mas large del set:
    int max_sock = server_socket;
    // indicador si es que el jugador lider determina empezar y se cumplen las
    // restricciones
    while (1) {
        if (villagers_count == clients_sockets->n_players &&
                names_count == clients_sockets->n_players && !advised) {
            server_send_message(
                    clients_sockets->sockets[0], 1,
                    "Por favor empezar el juego, todos l@s jugadores esperan\n");
            advised = 1;
        } // recorremos los sockets actuales en cada iteracion
        // inicializamos el set de sockets vacio
        FD_ZERO(&read_set);
        // luego ponemos el listener (server) dentro del set
        FD_SET(server_socket, &read_set);
        // agreagamos socket de clientes al set
        for (int i = 0; i < clients_sockets->n_players; i++) {
            // socket descriptor
            cli_sock = clients_sockets->sockets[i];

            // if valid socket descriptor then add to read list
            if (cli_sock > 0)
                FD_SET(cli_sock, &read_set);

            // highest file descriptor number, need it for the select function
            if (cli_sock > max_sock)
                max_sock = cli_sock;
        }

        to_read = select(max_sock + 1, &read_set, NULL, NULL, NULL);
        printf("to_read %i\n", to_read);
        if (to_read < 0 && errno != EINTR) {
            printf("select failed: errno = %d\n", errno);
        }
        if (FD_ISSET(server_socket, &read_set)) {
            int addr_len = sizeof(clients_addrs[clients_sockets->n_players]);
            if ((cli_sock = accept(
                            server_socket,
                            (struct sockaddr *)&clients_addrs[clients_sockets->n_players],
                            addr_len)) < 0) {
                printf("accept error! %i\n",errno);
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket descriptor %d\n", cli_sock);
            for (int j = 0; j < clients_sockets->n_players; j++) {

                if (clients_sockets->sockets[j] == 0) {
                    clients_sockets->sockets[j] = cli_sock;
                    // aumentamos uno a la cuenta de los jugadores
                    clients_sockets->n_players++;
                    // redefinimos el max_sock si es que existe uno mayor
                    if (cli_sock > max_sock) {
                        max_sock = cli_sock;
                    }
                    if (clients_sockets->n_players == 1) {
                        // si es el primer jugador, entonces es asignado como el jugador
                        // lider
                        server_send_message(
                                j, 1,
                                "Bienvenido nuevo jugador! Eres el primero y por ende, el "
                                "jugador jefe\n"
                                "Es tu responsabilidad solicitar cuando inciar el juego\n"
                                "Por favor sigue las instrucciones con cuidado\n");
                    } else {
                        // a cualquier otro jugador, le damos la bienvenida
                        server_send_message(j, 1,
                                "Bienvenido nuevo jugador! Por favor sigue"
                                "las instrucciones con cuidado\n");
                    }
                    // a todos los jugadores les pedimos su nombre y que distribuyan a sus
                    // aldeanos
                    server_send_message(j, 2, "Debes enviar tu nombre de jugador");
                    server_send_message(
                            j, 3,
                            "Debes elegir como quieres distribuir tus 9 aldeanos "
                            "inciales\n"
                            "en los distintos roles, de la siguiente forma: 'm5i4'\n "
                            "(cinco mineros,cuatro ingenieros)");
                    break;
                }
            }
        }

        for (int i = 0; i < max_sock; i++) {
            cli_sock = clients_sockets->sockets[i];
            // comprobamos i pertenece al set read_set
            if (FD_ISSET(cli_sock, &read_set)) {
                // recibimos mensaje
                id = server_receive_id(cli_sock);
                buffer = server_receive_payload(cli_sock);
                // es necesario liberar el buffer despues
                if (id == 1) {
                    // hay que liberarlo despues
                    clients_sockets->names[i] = malloc(sizeof(buffer) / sizeof(char));
                    strcpy(clients_sockets->names[i], buffer);
                    sprintf(to_send, "Se ha conectado un nuevo jugador con nombre %s\n",
                            buffer);
                    server_send_message(clients_sockets->sockets[0], 0, to_send);
                    free(buffer);
                    names_count++;
                } else if (id == 2) {
                    int len = sizeof(buffer) / sizeof(char);
                    int count = 0;
                    int *roles = calloc(4, sizeof(int));
                    for (int j = 0; j < len; j += 2) {
                        count += (int)buffer[j + 1];
                        // agricultores
                        if (buffer[j] == 'a') {
                            roles[0] = (int)buffer[j + 1];
                        }
                        // mineros
                        else if (buffer[j] == 'm') {
                            roles[1] = (int)buffer[j + 1];
                        }
                        // ingenieros
                        else if (buffer[j] == 'i') {
                            roles[2] = (int)buffer[j + 1];
                        }
                        // guerreros
                        else if (buffer[j] == 'g') {
                            roles[3] = (int)buffer[j + 1];
                        }
                    }
                    // repartio correctamente el numero de aldeanos
                    if (count == 9) {
                        clients_sockets->villagers[i] = roles;
                        villagers_count++;
                    } else {
                        server_send_message(i, 1,
                                "Distribuya correctamente sus 9 aldeanos\n");
                    }
                } else if (id == 0) {
                    // el jugador jefe, solicita iniciar el juego
                    if (cli_sock == clients_sockets->sockets[0]) {
                        // comprobamos si todos definieron sus aldeanos e ingresaron nombre
                        if (villagers_count == clients_sockets->n_players &&
                                names_count == clients_sockets->n_players) {
                            for (int k = 0; k < clients_sockets->n_players; k++) {
                                server_send_message(clients_sockets->sockets[k], 0,
                                        "Comienza el juego, PREPARAD@S???\n");
                            }
                            return clients_sockets;
                        } else {
                            server_send_message(clients_sockets->sockets[0], 1,
                                    "Faltan jugadores por definir sus "
                                    "nombres y distribuir sus aldeanos\n");
                        }
                    }
                }
            }
        }
    }
}
