

#include "estructura.h"

int main(){
    char *papelArgs[] = {"./papel", NULL};
    char *tabacoArgs[] = {"./tabaco", NULL};
    char *fosforosArgs[] = {"./fosforos", NULL};
    char *agenteArgs[] = {"./agente", NULL};
    
    
	key_t clave = generateKey();
	int id = msgget(clave, 0666 |IPC_CREAT );
	if(id < 0){
		printf("error\n");
		exit(1);
	}
		struct mensaje msg;
		msg.tipo = ARMANDO;
		msgsnd(id, &msg, MSG_SIZE, 0);

		int pidA = fork();
			if(pidA == 0){
				execv("./agente", agenteArgs);
				exit(3);
		}
		
        int pidP= fork();
	    if(pidP == 0){
			    execv("./papel", papelArgs);
			    exit(3);
		}

		int pidT= fork();
			if(pidT == 0){
				execv("./tabaco", tabacoArgs);
				exit(3);
		}
		
		int pidF = fork();
			if(pidF == 0){
				execv("./fosforos", fosforosArgs);
				exit(3);
		}
		
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	return 0;
	
	
}
