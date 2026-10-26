#include "estructura.h"

int main(int argsc, char * argv[]){
	int id = atoi(argv[1]);
	key_t clave = generateKey();

	int cola = msgget(clave, 0666);

	if (cola == -1) {
		perror("Error al obtener la cola de mensajes");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));

	struct mensaje msgRcv, msgSnd, msgMutex;
	msgMutex.tipo = MUTEX;
	
	while(1){
		msgrcv(cola, &msgRcv, MSG_SIZE, VACIO, 0);
		msgrcv(cola, &msgMutex, MSG_SIZE, MUTEX, 0);
			int producto = random() % 20 + id;
			printf("El productor %d produjo el producto: %d.\n", id, producto);
		msgsnd(cola, &msgMutex, MSG_SIZE, 0);
		msgSnd.tipo = LLENO;
		msgSnd.p = producto;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		sleep(1);
	}

	return 0;
}
