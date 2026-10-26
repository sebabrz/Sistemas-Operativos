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
	
	while (1){   
        msgrcv(cola, &msgMutex, MSG_SIZE,MUTEX,0);
            if(msgrcv(cola,&msgRcv, MSG_SIZE, LECTORES, IPC_NOWAIT)!=-1) {
                msgSnd.tipo=LECTORES;
                msgsnd(cola,&msgSnd,MSG_SIZE,0);
            }
            else {
                msgrcv(cola,&msgRcv,MSG_SIZE,RW,0);
            }
            msgSnd.tipo=LECTORES;
            msgsnd(cola,&msgSnd,MSG_SIZE,0);// Llega un lector
        msgsnd(cola,&msgMutex,MSG_SIZE,0);

        printf(COLOR_READER "Lector %d  se encuentra leyendo.\n" COLOR_RESET, id);        
        
        msgrcv(cola, &msgMutex, MSG_SIZE,MUTEX, 0);
            msgrcv(cola,&msgRcv,MSG_SIZE,LECTORES,0); // Se va un lector
            printf(COLOR_READER "Lector %d se va.\n" COLOR_RESET, id);   
            if(msgrcv(cola,&msgRcv, MSG_SIZE, LECTORES, IPC_NOWAIT)!=-1) {
                msgSnd.tipo=LECTORES;
                msgsnd(cola,&msgSnd,MSG_SIZE,0);
            }
            else {
				msgSnd.tipo=RW;
                msgsnd(cola,&msgSnd,MSG_SIZE,0);
            }
        msgsnd(cola,&msgMutex,MSG_SIZE,0);
        sleep(1);
    }
    
    return 0;
}
