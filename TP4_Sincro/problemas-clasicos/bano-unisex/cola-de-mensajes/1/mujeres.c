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
	
	while(1){
		msgrcv(cola, &msgRcv, MSG_SIZE, TURNO, 0);
		msgrcv(cola, &msgRcv, MSG_SIZE, MAX_M, 0);
		msgrcv(cola, &msgRcv, MSG_SIZE, MUTEX_M, 0);
			msgSnd.tipo = TURNO;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			if(msgrcv(cola, &msgRcv, MSG_SIZE, CANT_MUJERES, IPC_NOWAIT) != -1){
				msgSnd.tipo = CANT_MUJERES;
				msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			}
			else{
				msgrcv(cola, &msgRcv, MSG_SIZE, BANO, 0);
			}
			msgSnd.tipo = CANT_MUJERES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgSnd.tipo = MUTEX_M;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		
		printf("Mujer entra al bano\n");
		sleep(1);
		printf("Mujer sale del bano.\n");
		
		msgrcv(cola, &msgRcv, MSG_SIZE, MUTEX_M, 0);
			msgrcv(cola, &msgRcv, MSG_SIZE, CANT_MUJERES, 0);
			if(msgrcv(cola, &msgRcv, MSG_SIZE, CANT_MUJERES, IPC_NOWAIT) != -1){
				msgSnd.tipo = CANT_MUJERES;
				msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			}
			else{
				msgSnd.tipo = BANO;
				msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			}
		msgSnd.tipo = MUTEX_M;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgSnd.tipo = MAX_M;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
	}
	return 0;
}
