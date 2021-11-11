#include "conection.h"

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
    fd_set read_set;
    fd_set new_set;

    // Se define la estructura para almacenar info del socket del servidor al
    // momento de su creación
    struct sockaddr_in server_addr;
    int MAX_N_PLAYERS = 4;

    // Se solicita un socket al SO, que se usará para escuchar conexiones
    // entrantes
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // inicializamos el set de sockets vacio
    FD_ZERO(&read_set);
    // luego ponemos el listener (server) dentro del set
    FD_SET(server_socket, &read_set);
    // este sera el file_descriptor mas large del set:
    /* nfds should be set to the highest-numbered file descriptor in any of the
     * three sets, plus 1.  The indicated file descriptors in each set are
     * checked, */
    /* up to this limit (but see BUGS). */
    int max_sock = server_socket;

    // Se configura el socket a gusto (recomiendo fuertemente el REUSEPORT!)
    int opt = 1;
    int ret =
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (ret == -1) {
        printf("Problem setting the socket up");
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
    int ret2 =
        bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Se coloca el socket en modo listening
    // Seteamos el maximo de conecciones en la cola, como el maximo menos una
    // conexion
    int ret3 = listen(server_socket, MAX_N_PLAYERS);

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
    *clients_sockets = (PlayersInfo){
        .n_players = 0,
        .sockets = {0},
        .names = calloc(4, sizeof(char *)),
        .villagers = calloc(4, sizeof(int *))};
    int to_read;
    int cli_sock;
    char *buffer;
    int id;
    int names_count = 0;
    int villagers_count = 0;
    char *to_send;
    // indicador si es que el jugador lider determina empezar y se cumplen las
    // restricciones
    while (1) {
        if (villagers_count == clients_sockets->n_players && names_count == clients_sockets->n_players){
            server_send_message(clients_sockets->sockets[0],1,"Por favor empezar el juego, todos l@s jugadores esperan\n");
        }        // recorremos los sockets actuales en cada iteracion
        for (int i = 0; i < max_sock; i++) {
            memcpy(&new_set, &read_set, sizeof(new_set));
            // Se aceptan a los primeros 2 clientes que lleguen. "accept" retorna el
            // n° de otro socket asignado para la comunicación
            to_read = select(max_sock + 1, &read_set, NULL, NULL, NULL);
            if (to_read == -1) {
                printf("select failed: errno = %d\n", errno);
                if (errno == EINTR)
                    continue;
            }
            // comprobamos i pertenece al set read_set
            if (FD_ISSET(i, &read_set)) {
                // comprobamos si el actual corresponde al server_socket
                if (i == server_socket) {
                    // aceptamos una nueva conexion si es que el numero de jugadores es
                    // menor a MAX_N_PLAYERS
                    cli_sock = accept(
                            server_socket,
                            (struct sockaddr *)&clients_addrs[clients_sockets->n_players],
                            sizeof(client1_addr));
                    // si no aceptamos ningun socket, entonces continuamos iterando
                    if (cli_sock == -1) {
                        printf("accept failed: errno=%d\n", errno);
                        continue;
                    } else {
                        // agregamos el nuevo socket al set de escucha para que en el
                        // siguiente loop sea agregado al read_set
                        FD_SET(cli_sock, &new_set);
                        clients_sockets->sockets[clients_sockets->n_players] = i;
                        // aumentamos uno a la cuenta de los jugadores
                        clients_sockets->n_players++;
                        // redefinimos el max_sock si es que existe uno mayor
                        if (cli_sock > max_sock) {
                            max_sock = cli_sock;
                        }
                        if (clients_sockets->n_players == 1) {
                            server_send_message(
                                    i, 1,
                                    "Bienvenido nuevo jugador! Eres el primero y por ende, el "
                                    "jugador jefe\n"
                                    "Es tu responsabilidad solicitar cuando inciar el juego\n"
                                    "Por favor sigue las instrucciones con cuidado\n");
                        } else {
                            server_send_message(i, 1,
                                    "Bienvenido nuevo jugador! Por favor sigue"
                                    "las instrucciones con cuidado\n");
                        }
                        server_send_message(i, 2, "Debes enviar tu nombre de jugador");
                        server_send_message(
                                i, 3,
                                "Debes elegir como quieres distribuir tus 9 aldeanos "
                                "inciales\n"
                                "en los distintos roles, de la siguiente forma: 'm5i4'\n "
                                "(cinco mineros,cuatro ingenieros)");
                    }
                } else {
                    // el socket corresponde a un cliente, por lo tanto leemos bytes
                    id = server_receive_id(i);
                    buffer = server_receive_payload(i);
                    // es necesario liberar el buffer despues
                    // si no recibimos payload
                    if (buffer == NULL) {
                        // cerramos el socket
                        close(i);
                        // lo eliminamos del set
                        FD_CLR(i, &new_set);
                        // si i es igual al maximo socket
                        if (i == max_sock) {
                            for (int x = 0; x < max_sock; x++) {
                                // buscamos un nuevo maximo
                                if (FD_ISSET(x, &new_set))
                                    max_sock = x;
                            }
                        }
                    } else {
                        if (id == 1) {
                            // hay que liberarlo despues
                            clients_sockets->names[i] =
                                malloc(sizeof(buffer) / sizeof(char));
                            strcpy(clients_sockets->names[i],
                                    buffer);
                            sprintf(to_send, "Se ha conectado un nuevo jugador %s\n", buffer); server_send_message(clients_sockets->sockets[0], 0, to_send);
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
                                server_send_message(
                                        i, 1, "Distribuya correctamente sus 9 aldeanos\n");
                            }
                        } else if (id == 0) {
                            // el jugador jefe, solicita iniciar el juego
                            if (i == clients_sockets->sockets[0]) {
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
                            } else {
                                server_send_message(
                                        i, 1, "No tienes permiso para iniciar el juego\n");
                            }
                        }
                    }
                }
            }
        }
    }
}
