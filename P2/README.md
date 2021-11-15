# P2 Lunes 15 de Noviembre
## Conexion Inicial:

- [x] *Conexion*: El servidor debera esperar a los jugadores para comenzar. Cuando un jugador se conecte se le debe dar la bienvenida. El primer jugador conectado sera designado el lider del grupo

- [x] *Lobby*: Al conectarse todos los jugadores ingresan a un lobby inicial donde se les pedira el nombre y se le pedira que reparta 9 aldeanos en uno de los cuatro roles. El lider del grupo deve ser notificado sobre los nuevos jugadores que ingresen mostrando su nombre

- [x] *Inicio Juego*: El lider del grupo sera el unico con la opcion de iniciar el juego. Si el es el unico conectado o si alguno  de los jugadores aun no ingresa nombre o no ha repartido los aldeanos, el juego no podra comenzar y se le notificara al lider del grupo que los jugadores no estan listos
## Flujo Del Programa:
Sera por turnos y el turno de cada jugador debe seguir el siguiente orden:

### Recolectar recursos
Para poder crear soldados y crecer la aldea, el jugador debe conseguir y gastar recursos. Los recursos son: oro, comida y ciencia. Al inicio de cada turno , el jugador recibira una cantidad de estos recursos que dependera de la cantidad de aldeanos que tiene asignados a extraer esos recursos multiplicado por su nivel de produccion:
* Oro: cantidad de mineros X nivel de mineros X 2
* Comida: cantidad de agricultores X nivel de agricultores X 2
* Ciencia: cantidad de ingenieros X nivel de ingenieros

### Roles
Durante su turno, cada jugador podra gastar sus recursos para crear nuevos aldeanos. Cada aldeano creado debe tener un rol. El costo de producir cada aldeano y sus caracteristicas son las siguientes:
- [x] Agricultores: costo creacion 10 de comida
- [x] Mineros: costo de creacion 10 de comida y 5 de oro
- [x] Ingenieros: costo de creacion 20 de comida y 10 de oro
- [x] Guerreros: defienden o atacan aldeas, costo de creacion 10 de comida y 10 de oro

### Realizar Acciones
Luego de obtener recursos, un jugador puede realizar tantas acciones como quiera hasta pasar de turno. Las acciones posibles son:
- [x] Mostrar informacion: cantidad de cada recurso, cantidad de aldeanos en cada rol, nivel de agricultores, mineros,ingenieros,ataque y defensa
- [x] Crear aldeano: gastar comida y oro para crear nuevos aldeanos, lo cual dependera del rol a asignar (comprobar que los recursos son suficientes)
- [x] Subir de nivel: todos los roles tienen asociado un nivel (los guerreros ataque y defensa)
    - Nivel agricultores,mineros,ingenieros
    - Nivel Defensa
    - Nivel Ataque
    Al inicio del juego, todos los niveles comienzan en 1 y el costo para subirlos es:
    - Nivel 2: 10 de comida + 10 de oro + 10 de ciencia
    - Nivel 3: 20 de comida + 20 de oro + 20 de ciencia
    - Nivel 4: 30 de comida + 30 de oro + 30 de ciencia
    - Nivel 5: 40 de comida + 40 de oro + 40 de ciencia

- [x] Atacar: Un usuario puede atacar la aldea de otro. Se compara la fuerza de sus ejercitos:
    - Fuerza atacante: cantidad de guerreros X nivel de ataque
    - Fuerza defensor: cantidad de guerreros X nivel de defensa X 2
    Si el atacante gana, obtiene toda la comida,oro y ciencia del defensor. Ademas el defensor es eliminado de la partida. Un jugador eliminado no puede ser atacado y debe ser saltado por el servidor. En caso contrario, el atacante pierde la mitad de sus guerreros (floor)
- [x] Espiar: Se podra espiar una al de enemiga para conocer el estado de su ejercito (cantidad de guerreros, nivel de ataque y defensa). Con un costo de 30 de oro y el gasto debe verse reflejado apenas comience la accion
- [x] Robar: 10 de ciencia para robar recursos a otros jugadores. Esta accion debe permitir elegir a otro usuario y elegir entre comida y oro. El usuario debera perder el 10% del recurso seleccionado y el usuario que roba, obtendra esa cantidad

- [x] Pasar: Terminar el turno actual
- [x] Rendirse: Queda a nuestro criterio si se desconecta o queda como espectador

## Interfaz por consola
En cada turno se deben imprimir los recursos y una lista de acciones posibles, al igual que en los menus secundarios. Fuera del turno se deben imprimir los mensajes qeu informen sobre el desarrollo del juego (que hace cada jugador en su turno, excepto en el caso de espionaje)
## Implementacion
### Cliente
El cliente debe recibir e imprimir los menus y/o mensajes correspondietnes y debe enviar los inputs al servidor
### Servidor
El servidor debe mediar la comunicacion entre los clientes. Se encarga de procesar toda la logica del juego.
### Protocolo de comunicacion
Todos los mensjaes enviados, tanto de parte del servidor, como de parte del cliente, deberan seguir el siguiente formato:
- ID (1 byte): Indica el tipo de paquete
- PayloadSize (1 byte): Corresponde al size en bytes del payload (0-255)
- Payload (PayloadSize bytes): Es el mensaje. En caso de no requerir, el PayloadSize sera 0 y el Payload estara vacio
## Ejecucion
Primer argumento ip del servidor y segundo el puerto en escucha:
        `./server -i <ip_address> -p <tcp_port>`
        `./client -i <ip_address> -p <tcp_port>`

# Documentacion
## Integrantes:
- Camilo Soria Aranguiz 18207979
- Andrés Fauré Ropert 17639875
- Joaquín Strobl Diez 17637074
- Juan José Aubele León 18637124
- Matías Briones Sánchez 15621340

## Instrucciones de Ejecucion
Para ejecutar el programa, se debe primero ejecutar el servidor desde la carpeta 'server', y luego los clientes desde la carpeta 'client'. Luego, todas las instrucciones para usar el programa aparecen en las respectivas consola.

## Descripcion de Paquetes
### Servidor
- ID:0 Comienzo del juego
- ID:1 Mensaje para imprimir en consola sin esperar respuesta (logs)
- ID:2 Mensaje pidiendo nombre
- ID:3 Mensaje pidiendo aldeanos por tipo
- ID:4 Mensajes interactivos (jugadas)
- ID:5 Mensaje indicando que el juego puede comenzar
### Cliente
- ID:0 Iniciar juego (proveniente del jugador jefe)
- ID:1 Nombre
- ID:2 Distribucion de aldeanos iniciales
- ID:3 Jugada
- ID:4 Jugada Opciones

## Principales Funciones y Decisiones de Diseño

### Principales Funciones
Nuestro directorio de trabajo `P2` incluye dos directorios principales, `client` y `server`. El primero posee las funciones que hacen correr el cliente, y el segundo las que hacen correr el servidor.

Tanto el servidor como los clientes operan desde un archivo llamado `main.c`, en sus respectivos directorios. Desde el servidor, se definió un `struct` `PlayersInfo` que posee la información de las aldeas de todos los jugadores. Las funciones más usadas fueron las que hacían que la comunicación cliente - servidor funcionara efectivamente, estas son `server_receive_id`, `server_receive_payload` y `server_send_message` para el servidor; y `client_receive_id`, `client_receive_payload` y  `client_send_message` para el cliente. Estas funciones están en un archivo llamado `comunication.c`, dentro de cada respectivo directorio. A su vez, para la operación del juego, se programó una función para cada acción posible, que se encuentran en el los archivos titulados `game.c` dentro del directorio de servidor.

### Decisiones de Diseño
El programa funciona completamente desde la consola, es decir, el serivdor y cada cliente (por separado) debe inicializarse en consolas distintas. Para la dinámica de turnos, las deciciones se que cada cliente puede tomar se describen en un "menú de opciones". Decidimos que se acaba un turno cuando un jugador decide "Pasar" (es decir, selecciona la opción 8 en el menú de opciones). Lo anterior significa que un jugador puede llevar a cabo una cantidad indeterminada de acciones en un turno (con tal que tenga recursos para hacerlo).

La principal decisión de diseño que implementamos fue que el juego debe jugarse con al menos dos jugadores. Es decir, en un comienzo, el juego no permitirá partir con sólo un jugador, y, si llega a partir con más de uno, si por alguna razón todos los jugadores son eliminados excepto uno (sea por ataques o por rendirse), el programa terminará en el turno siguiente. Por ejemplo, en un juego de dos jugadores, si uno ataca al otro y gana, significa que será el único jugador restante. Es decir, cuando ese jugador termine su turno, el juego se acabará. Esta decisión se tomó ya que no nos parecia lógico que el juego se jugara de a uno, ya que muchas de las acciones por tomar necesitan a otro jugador para interactuar.

## Bonus
No se programaron ninguno de los Bonus para esta entrega.
