#include "conection.h"

//LINKS REFERENCIAS:
//https://www.man7.org/linux/man-pages/man2/socket.2.html
//https://man7.org/linux/man-pages/man7/socket.7.html
//https://www.man7.org/linux/man-pages/man3/setsockopt.3p.html
//https://man7.org/linux/man-pages/man2/setsockopt.2.html
//https://linux.die.net/man/3/htons
//https://linux.die.net/man/3/inet_aton
//https://www.howtogeek.com/225487/what-is-the-difference-between-127.0.0.1-and-0.0.0.0/
//https://www.man7.org/linux/man-pages/man2/bind.2.html
//https://www.man7.org/linux/man-pages/man2/accept.2.html


PlayersInfo * prepare_sockets_and_get_clients(char * IP, int port){
    // Se define la estructura para almacenar info del socket del servidor al momento de su creación
    struct sockaddr_in server_addr;
    int MAX_N_PLAYERS = 4;

    // Se solicita un socket al SO, que se usará para escuchar conexiones entrantes
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Se configura el socket a gusto (recomiendo fuertemente el REUSEPORT!)
    int opt = 1;
    int ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (ret == -1) {
        printf("Problem setting the socket up");
    }

    // Se guardan el puerto e IP en la estructura antes definida
    // Se llena la direccion con 0
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // Converts IP adress into binary form (network byte order) and saves it in the ptr given in arg2
    inet_aton(IP, &server_addr.sin_addr); server_addr.sin_port = htons(port);

    // Se le asigna al socket del servidor un puerto y una IP donde escuchar
    int ret2 = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Se coloca el socket en modo listening
    // Seteamos el maximo de conecciones en la cola, como el maximo menos una conexion
    int ret3 = listen(server_socket, MAX_N_PLAYERS-1);

    // Se definen las estructuras para almacenar info sobre los sockets de los clientes
    struct sockaddr_in client1_addr;
    struct sockaddr_in client2_addr;
    struct sockaddr_in client3_addr;
    struct sockaddr_in client4_addr;
    socklen_t addr_size = sizeof(client1_addr);

    // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
    PlayersInfo * sockets_clients = malloc(sizeof(PlayersInfo));

    while (MAX_N_PLAYERS > sockets_clients->n_players){
        // Se aceptan a los primeros 2 clientes que lleguen. "accept" retorna el n° de otro socket asignado para la comunicación
        if (sockets_clients -> n_players == 0){
            // Este es el jugador lider de la partida
            sockets_clients->sockets[sockets_clients->n_players] = accept(server_socket, (struct sockaddr *)&client1_addr, &addr_size);
            sockets_clients -> n_players++;
        }else if (sockets_clients->n_players == 1){
            sockets_clients->sockets[sockets_clients->n_players] = accept(server_socket, (struct sockaddr *)&client2_addr, &addr_size);
            sockets_clients -> n_players++;
        }else if (sockets_clients->n_players == 2){
            sockets_clients->sockets[sockets_clients->n_players] = accept(server_socket, (struct sockaddr *)&client3_addr, &addr_size);
            sockets_clients -> n_players++;
        } else {
            sockets_clients->sockets[sockets_clients->n_players] = accept(server_socket, (struct sockaddr *)&client4_addr, &addr_size);
            sockets_clients -> n_players++;
        }
    }

    return sockets_clients;
}
