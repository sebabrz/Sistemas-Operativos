#include "estructura.h"

int main(){
	key_t clave = generateKey();

	int cola = msgget(clave, 0666);

	if (cola == -1) {
		perror("Error al obtener la cola de mensajes");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));
	
	struct mensaje msgRcv, msgSnd, msgMutex;
	msgMutex.tipo = MUTEXESCRITORES;
	
	while(1){
		msgrcv(cola, &msgMutex, MSG_SIZE, MUTEXESCRITORES, 0);
		if(msgrcv(cola, &msgRcv, MSG_SIZE, CANTESCRITORES, IPC_NOWAIT) != -1){
			msgSnd.tipo = CANTESCRITORES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		}
		else{
			msgrcv(cola, &msgRcv, MSG_SIZE, NOLECTORES, 0);
		}
		msgSnd.tipo = CANTESCRITORES;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgsnd(cola, &msgMutex, MSG_SIZE, 0);
		msgrcv(cola, &msgRcv, MSG_SIZE, NOESCRITORES, 0);
		printf("escritor escribiendo\n");
		fflush(stdout);
		sleep(1);
		printf("escritor se va\n");
		fflush(stdout);
		msgSnd.tipo = NOESCRITORES;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgrcv(cola, &msgMutex, MSG_SIZE, MUTEXESCRITORES, 0);
		msgrcv(cola, &msgRcv, MSG_SIZE, CANTESCRITORES, 0);
		if(msgrcv(cola, &msgRcv, MSG_SIZE, CANTESCRITORES, IPC_NOWAIT) != -1){
			msgSnd.tipo = CANTESCRITORES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		}
		else{
			msgSnd.tipo = NOLECTORES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		}
		msgsnd(cola, &msgMutex, MSG_SIZE, 0);
	}

	return 0;
}

