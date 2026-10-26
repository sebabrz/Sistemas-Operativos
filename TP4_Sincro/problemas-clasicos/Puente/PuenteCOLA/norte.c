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
		msgrcv(idmsg , &msg, MSG_SIZE, TURNON, 0);
		msgrcv(idmsg , &msg, MSG_SIZE, NMUTEX, 0);
		
		if(msgrcv(idmsg , &msg, MSG_SIZE, NORTE, IPC_NOWAIT) != -1){
			msg.tipo=NORTE;
			msgsnd(idmsg , &msg, MSG_SIZE,  0);
			
		}
		else{
			
			
			printf("Esperando para cruzar desde el norte  \n");
			msgrcv(idmsg , &msg, MSG_SIZE, PUENTE, 0);
		}
		msg.tipo=NORTE;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		
		msg.tipo=NMUTEX;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		
		printf("Cruzando el puente desde el norte  \n");
		sleep(1);
		
		msgrcv(idmsg , &msg, MSG_SIZE, NMUTEX, 0);
		msgrcv(idmsg , &msg, MSG_SIZE, NORTE, 0);
			
		
		if(msgrcv(idmsg , &msg, MSG_SIZE, NORTE, IPC_NOWAIT) != -1){
			msg.tipo=NORTE;
			msgsnd(idmsg , &msg, MSG_SIZE,  0);
			
		}
		else{
		
			msg.tipo=PUENTE;
			msgsnd(idmsg , &msg, MSG_SIZE,  0);
			
		}
		msg.tipo=TURNOS;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		
		msg.tipo=NMUTEX;
		msgsnd(idmsg , &msg, MSG_SIZE,  0);
		sleep(2);
	}
	
    
}
