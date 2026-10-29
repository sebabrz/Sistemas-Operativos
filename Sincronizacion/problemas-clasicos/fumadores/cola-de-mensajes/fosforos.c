
#include "estructura.h"

int main() {
    key_t clave = generateKey();
    int id = msgget(clave, 0666);
    if (id < 0) {
        printf("error al obtener la cola de mensajes\n");
        exit(1);
    }

    struct mensaje msg;

	while(1){
		msgrcv(id, &msg, MSG_SIZE, T, 0);
		if(msgrcv(id, &msg, MSG_SIZE, P, IPC_NOWAIT) != -1){
			printf(GREEN "Fumador con fosforos arma un cigarrillo \n" RESET);
			//sleep(1);
			msg.tipo = ARMANDO;
			msgsnd(id, &msg, MSG_SIZE, 0);
		}
		else{
			msg.tipo = T;
			msgsnd(id, &msg, MSG_SIZE, 0);
		}
	}
	return 0;
}

