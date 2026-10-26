#include "estructura.h"

int main(){

   
    char *argsN[] = {"./n", NULL};
    char *argsS[] = {"./s", NULL};
 
	key_t clave = generateKey();
	int idmsg = msgget(clave, 0666| IPC_CREAT);
	if(idmsg < 0){
		printf("error en la creación de la cola de mensajes\n");
		exit(1);
	}
	  
  
	struct mensaje  msg;
        
      for(int i=0; i<3; i++){
         msg.tipo = TURNON;
         msgsnd(idmsg , &msg, MSG_SIZE,0);
      }
      
      msg.tipo=SMUTEX;
      msgsnd(idmsg , &msg, MSG_SIZE,0);
      msg.tipo=NMUTEX;
      msgsnd(idmsg , &msg, MSG_SIZE,0);
      msg.tipo=PUENTE;
      msgsnd(idmsg , &msg, MSG_SIZE,0);
      
	
      
      


	for(int i=0;i<CANT;i++){
	int pidN = fork();
		if(pidN == 0){
			execv("./n", argsN);
			exit(3);
		}
	}

		
	for(int i=0;i<CANT;i++){
	int pidH = fork();
		if(pidH == 0){
			execv("./s", argsS);
			exit(3);
		}
	}

     

   for(int i=0; i< CANT*2;i++){
		wait(NULL);	
	}
	
	
	return 0;
	
	
}
