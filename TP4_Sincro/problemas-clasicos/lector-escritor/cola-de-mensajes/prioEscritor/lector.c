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
	msgMutex.tipo = MUTEXLECTORES;
	
	while(1){
		msgrcv(cola, &msgRcv, MSG_SIZE, NOLECTORES, 0);
		msgrcv(cola, &msgMutex, MSG_SIZE, MUTEXLECTORES, 0);
		if(msgrcv(cola, &msgRcv, MSG_SIZE, CANTLECTORES, IPC_NOWAIT) != -1){
			msgSnd.tipo = CANTLECTORES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		}
		else{
			msgrcv(cola, &msgRcv, MSG_SIZE, NOESCRITORES, 0);
		}
		msgSnd.tipo = CANTLECTORES;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgsnd(cola, &msgMutex, MSG_SIZE, 0);
		msgSnd.tipo = NOLECTORES;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		printf("lector %d leyendo\n", id);
		fflush(stdout);
		sleep(1);
		printf("lector %d se va\n", id);
		fflush(stdout);
		msgrcv(cola, &msgMutex, MSG_SIZE, MUTEXLECTORES, 0);
		msgrcv(cola, &msgRcv, MSG_SIZE, CANTLECTORES, 0);
		if(msgrcv(cola, &msgRcv, MSG_SIZE, CANTLECTORES, IPC_NOWAIT) != -1){
			msgSnd.tipo = CANTLECTORES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		}
		else{
			msgSnd.tipo = NOESCRITORES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		}
		msgsnd(cola, &msgMutex, MSG_SIZE, 0);
	}
	return 0;
}
