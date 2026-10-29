#include "estructura.h"

int main(){

   
    char *argsAsistente[] = {"./Asistente", NULL};
    char *argsAlumno[] = {"./Alumno", NULL};
 
	key_t clave = generateKey();
	int idmsg = msgget(clave, 0666| IPC_CREAT);
	if(idmsg < 0){
		printf("error en la creación de la cola de mensajes\n");
		exit(1);
	}
	  
  
	struct mensaje  msg;
        
        for(int i=0; i<3; i++){
         msg.tipo = SILLAS;
         msgsnd(idmsg , &msg, MSG_SIZE,0);
      }
      
      msg.tipo=MUTEX;
      msgsnd(idmsg , &msg, MSG_SIZE,0);
      msg.tipo=AULA;
      msgsnd(idmsg , &msg, MSG_SIZE,0);
      
	
      
      


	int pidO = fork();
		if(pidO == 0){
			execv("./Asistente", argsAsistente);
			exit(3);
		}

		
	for(int i=0;i<CANTALUMNOS;i++){
	int pidH = fork();
		if(pidH == 0){
			execv("./Alumno", argsAlumno);
			exit(3);
		}
	}

     

   for(int i=0; i< CANTALUMNOS+1;i++){
		wait(NULL);	
	}
	
	
	return 0;
	
	
}
