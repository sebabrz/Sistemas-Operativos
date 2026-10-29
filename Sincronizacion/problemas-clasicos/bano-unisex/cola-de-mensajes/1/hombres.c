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
		msgrcv(cola, &msgRcv, MSG_SIZE, MAX_H, 0);
		msgrcv(cola, &msgRcv, MSG_SIZE, MUTEX_H, 0);
			msgSnd.tipo = TURNO;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			if(msgrcv(cola, &msgRcv, MSG_SIZE, CANT_HOMBRES, IPC_NOWAIT) != -1){
				msgSnd.tipo = CANT_HOMBRES;
				msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			}
			else{
				msgrcv(cola, &msgRcv, MSG_SIZE, BANO, 0);
			}
			msgSnd.tipo = CANT_HOMBRES;
			msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgSnd.tipo = MUTEX_H;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		
		printf("Hombre entra al bano\n");
		sleep(1);
		printf("Hombre sale del bano.\n");
		
		msgrcv(cola, &msgRcv, MSG_SIZE, MUTEX_H, 0);
			msgrcv(cola, &msgRcv, MSG_SIZE, CANT_HOMBRES, 0);
			if(msgrcv(cola, &msgRcv, MSG_SIZE, CANT_HOMBRES, IPC_NOWAIT) != -1){
				msgSnd.tipo = CANT_HOMBRES;
				msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			}
			else{
				msgSnd.tipo = BANO;
				msgsnd(cola, &msgSnd, MSG_SIZE, 0);
			}
		msgSnd.tipo = MUTEX_H;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
		msgSnd.tipo = MAX_H;
		msgsnd(cola, &msgSnd, MSG_SIZE, 0);
	}
	return 0;
}
