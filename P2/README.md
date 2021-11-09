# P2 Lunes 15 de Noviembre
## Conexion Inicial:

- [ ] *Conexion*: El servidor debera esperar a los jugadores para comenzar. Cuando un jugador se conecte se le debe dar la bienvenida. El primer jugador conectado sera designado el lider del grupo

- [ ] *Lobby*: Al conectarse todos los jugadores ingresan a un lobby inicial donde se les pedira el nombre y se le pedira que reparta 9 aldeanos en uno de los cuatro roles. El lider del grupo deve ser notificado sobre los nuevos jugadores que ingresen mostrando su nombre

- [ ] *Inicio Juego*: El lider del grupo sera el unico con la opcion de iniciar el juego. Si el es el unico conectado o si alguno  de los jugadores aun no ingresa nombre o no ha repartido los aldeanos, el juego no podra comenzar y se le notificara al lider del grupo que los jugadores no estan listos
## Flujo Del Programa:
Sera por turnos y el turno de cada jugador debe seguir el siguiente orden:

### Recolectar recursos
Para poder crear soldados y crecer la aldea, el jugador debe conseguir y gastar recursos. Los recursos son: oro, comida y ciencia. Al inicio de cada turno , el jugador recibira una cantidad de estos recursos que dependera de la cantidad de aldeanos que tiene asignados a extraer esos recursos multiplicado por su nivel de produccion:
* Oro: cantidad de mineros X nivel de mineros X 2
* Comida: cantidad de agricultores X nivel de agricultores X 2
* Ciencia: cantidad de ingenieros X nivel de ingenieros

### Roles
Durante su turno, cada jugador podra gastar sus recursos para crear nuevos aldeanos. Cada aldeano creado debe tener un rol. El costo de producir cada aldeano y sus caracteristicas son las siguientes:
- [ ] Agricultores: costo creacion 10 de comida
- [ ] Mineros: costo de creacion 10 de comida y 5 de oro
- [ ] Ingenieros: costo de creacion 20 de comida y 10 de oro
- [ ] Guerreros: defienden o atacan aldeas, costo de creacion 10 de comida y 10 de oro

### Realizar Acciones
Luego de obtener recursos, un jugador puede realizar tantas acciones como quiera hasta pasar de turno. Las acciones posibles son:
- [ ] Mostrar informacion: cantidad de cada recurso, cantidad de aldeanos en cada rol, nivel de agricultores, mineros,ingenieros,ataque y defensa
- [ ] Crear aldeano: gastar comida y oro para crear nuevos aldeanos, lo cual dependera del rol a asignar (comprobar que los recursos son suficientes)
- [ ] Subir de nivel: todos los roles tienen asociado un nivel (los guerreros ataque y defensa)
    - Nivel agricultores,mineros,ingenieros
    - Nivel Defensa
    - Nivel Ataque
    Al inicio del juego, todos los niveles comienzan en 1 y el costo para subirlos es:
    - Nivel 2: 10 de comida + 10 de oro + 10 de ciencia
    - Nivel 3: 20 de comida + 20 de oro + 20 de ciencia
    - Nivel 4: 30 de comida + 30 de oro + 30 de ciencia
    - Nivel 5: 40 de comida + 40 de oro + 40 de ciencia

- [ ] Atacar: Un usuario puede atacar la aldea de otro. Se compara la fuerza de sus ejercitos:
    - Fuerza atacante: cantidad de guerreros X nivel de ataque
    - Fuerza defensor: cantidad de guerreros X nivel de defensa X 2
    Si el atacante gana, obtiene toda la comida,oro y ciencia del defensor. Ademas el defensor es eliminado de la partida. Un jugador eliminado no puede ser atacado y debe ser saltado por el servidor. En caso contrario, el atacante pierde la mitad de sus guerreros (floor)
- [ ] Espiar: Se podra espiar una al de enemiga para conocer el estado de su ejercito (cantidad de guerreros, nivel de ataque y defensa). Con un costo de 30 de oro y el gasto debe verse reflejado apenas comience la accion
- [ ] Robar: 10 de ciencia para robar recursos a otros jugadores. Esta accion debe permitir elegir a otro usuario y elegir entre comida y oro. El usuario debera perder el 10% del recurso seleccionado y el usuario que roba, obtendra esa cantidad

- [ ] Pasar: Terminar el turno actual
- [ ] Rendirse: Queda a nuestro criterio si se desconecta o queda como espectador

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
-
-
-
-
-
## Instrucciones de Ejecucion
## Descripcion de Paquetes
## Principales Funciones y Decisiones de Diseño
