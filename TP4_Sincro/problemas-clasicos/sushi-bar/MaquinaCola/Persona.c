#include "def.h"

int main(){
    // Clave única para la cola de pedidos
        key_t key_q = generateKey();

        int cola = msgget(key_q, 0666);

        if (cola == -1) {
            perror("Error al obtener la cola de mensajes");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        srand(time(NULL));

        struct mensaje_t msgRcv, msgSnd ,msgMutex;
    while (1) {
        // Espera a que el tarro esté lleno
        msgrcv(cola, &msgRcv, msgSize, CANT_PERSONAS_MAX, 0);
        msgrcv(cola, &msgMutex, msgSize, MUTEX, 0);

		if((msgrcv(cola, &msgRcv, msgSize, CANT_PERSONAS_USANDO, IPC_NOWAIT) == -1)){
			// soy primero
			msgrcv(cola, &msgRcv, msgSize, MAQUINA, 0);
		}
		else{
			msgSnd.type = CANT_PERSONAS_USANDO;
			msgsnd(cola, &msgSnd, msgSize, 0);
		}
		msgSnd.type = CANT_PERSONAS_USANDO;
		msgsnd(cola, &msgSnd, msgSize, 0);
		msgsnd(cola, &msgMutex, msgSize, 0);
		
		printf("Usando la maquina \n");
		sleep(1);
		
		
        msgrcv(cola, &msgMutex, msgSize, MUTEX, 0);
		printf("Termine de usar la maquina \n");
        msgrcv(cola, &msgRcv, msgSize, CANT_PERSONAS_USANDO, 0);
		if((msgrcv(cola, &msgRcv, msgSize, CANT_PERSONAS_USANDO, IPC_NOWAIT) == -1)){
			printf("Soy el último, libero la máquina  \n");
			for(int i = 0; i<5; i++){
			msgSnd.type = CANT_PERSONAS_MAX;
			msgsnd(cola, &msgSnd, msgSize, 0);
			}
			msgSnd.type = MAQUINA;
			msgsnd(cola, &msgSnd, msgSize, 0);
		}
		else{
			msgSnd.type = CANT_PERSONAS_USANDO;
			msgsnd(cola, &msgSnd, msgSize, 0);
		}
		
		msgsnd(cola, &msgMutex, msgSize, 0);
        
    }

    return 0;
}
