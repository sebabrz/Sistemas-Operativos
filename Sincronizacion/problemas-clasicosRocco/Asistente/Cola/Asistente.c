

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
		msgrcv(idmsg , &msg, MSG_SIZE, ALUMNO, 0);
		
        printf(PURPLE"Asistente se despierta \n"RESET); // PARA QUE NO SE DESPIERTE SIEMPRE SIMULAR TRYWAIT CON IPC_NOWAIT

        // barbero corta pelo
          printf(PURPLE"Asistente ayudando a alumno\n"RESET);
		  sleep(rand() % 5);

        msg.tipo=AULA;
        msgsnd(idmsg , &msg, MSG_SIZE, 0);
    }
      
}
    
    
