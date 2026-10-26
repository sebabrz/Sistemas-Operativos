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
		sleep(1);
		msgrcv(idmsg , &msg, MSG_SIZE, TURNOS, 0);
		msgrcv(idmsg , &msg, MSG_SIZE, SMUTEX, 0);
		
		if(msgrcv(idmsg , &msg, MSG_SIZE, SUR, IPC_NOWAIT) != -1){
			msg.tipo=SUR;
			msgsnd(idmsg , &msg, MSG_SIZE,  0);
			
		}
		else{
			
			printf("Esperando para cruzar desde el sur  \n");
			msgrcv(idmsg , &msg, MSG_SIZE, PUENTE, 0);
		}
		msg.tipo=SUR;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		
		msg.tipo=SMUTEX;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		
		printf("Cruzando el puente desde el sur  \n");
		sleep(1);
		
		msgrcv(idmsg , &msg, MSG_SIZE, SMUTEX, 0);
		msgrcv(idmsg , &msg, MSG_SIZE, SUR, 0);
			
		
		if(msgrcv(idmsg , &msg, MSG_SIZE, SUR, IPC_NOWAIT) != -1){
			msg.tipo=SUR;
			msgsnd(idmsg , &msg, MSG_SIZE,  0);
			
		}
		else{
		
			msg.tipo=PUENTE;
			msgsnd(idmsg , &msg, MSG_SIZE,  0);
			
		}
		msg.tipo=TURNON;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		
		msg.tipo=SMUTEX;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		sleep(2);
	}
	
    
}
