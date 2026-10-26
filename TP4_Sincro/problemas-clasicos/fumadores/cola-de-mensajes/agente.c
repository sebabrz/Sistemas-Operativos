
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
		msgrcv(id, &msg, MSG_SIZE, ARMANDO, 0);
		int r = rand()%3;
		switch(r){
			case 0:{
				printf(CYAN "El agente puso Tabaco y Papel \n" RESET);
				//sleep(1);
				msg.tipo = T;
				msgsnd(id, &msg, MSG_SIZE, 0);
				msg.tipo = P;
				msgsnd(id, &msg, MSG_SIZE, 0);
				break;
			}
			case 1:{
				printf(CYAN "El agente puso Fosforos y Papel \n" RESET);
				//sleep(1);
				msg.tipo = F;
				msgsnd(id, &msg, MSG_SIZE, 0);
				msg.tipo = P;
				msgsnd(id, &msg, MSG_SIZE, 0);
				break;
			}
			case 2:{
				printf(CYAN "El agente puso Fosforos y Tabaco \n" RESET);
				//sleep(1);
				msg.tipo = T;
				msgsnd(id, &msg, MSG_SIZE, 0);
				msg.tipo = F;
				msgsnd(id, &msg, MSG_SIZE, 0);
				break;
			}
		}
	}
	return 0;
}
