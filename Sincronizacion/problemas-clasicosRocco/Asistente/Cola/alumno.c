#include "estructura.h"
int main() {
    key_t clave = generateKey();
    int idmsg = msgget(clave, 0666);
    if (idmsg < 0) {
        printf("error al obtener la cola de mensajes\n");
        exit(1);
    }

    struct mensaje msg;
    
    while(1){
        sleep(rand() % 10);
		printf(YELLOW"Alumno llega \n"RESET);
		if(msgrcv(idmsg , &msg, MSG_SIZE, SILLAS, IPC_NOWAIT) != -1){
        
            // si hay sillas, se sienta
            
			msg.tipo=ALUMNO;
			msgsnd(idmsg , &msg, MSG_SIZE, 0);
            msg.tipo=MUTEX;
			msgsnd(idmsg , &msg, MSG_SIZE, 0);

            printf(YELLOW"alumno espera assitente... \n"RESET);
            
            msgrcv(idmsg , &msg, MSG_SIZE, AULA, 0); // espera assistente

            // assitente ayudando alumno
            
            msg.tipo=SILLAS;
			msgsnd(idmsg , &msg, MSG_SIZE, 0);
        } else {
            // si no hay sillas, se va
            msg.tipo=MUTEX;
			msgsnd(idmsg , &msg, MSG_SIZE, 0);
            printf(YELLOW"alumno se va \n"RESET);
			sleep(rand() % 10);
        }
        
        }
        
        
}

      
    
