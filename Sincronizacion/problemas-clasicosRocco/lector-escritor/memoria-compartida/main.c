#include "estructura.h"

int main(){
    char *args1[] = {"./reading", NULL};
    char *args2[] = {"./writing", NULL};
	key_t clave = generateKey();
	int id = shmget(clave, 1024, IPC_CREAT | 0666);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	

	//Asociar la memoria a la estructura aCompartir
	struct aCompartir* recursosCompartidos = (struct aCompartir*) shmat(id, 0, 0);
	if(recursosCompartidos <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
	for(int i =0; i<READERS;i++){
        int pidA= fork();
	    if(pidA == 0){
			    execv("./reading", args1);
			    exit(3);
		    }
        }

	for(int i =0; i<WRITERS;i++){
        int pidA= fork();
	    if(pidA == 0){
			    execv("./writing", args1);
			    exit(3);
		    }
        }
	
	
	sem_init(&(recursosCompartidos->sem_readers),1,0);
	sem_init(&(recursosCompartidos->bsem_rw),1,1);
	sem_init(&(recursosCompartidos->mutex),1,1);
	
	for(int i=0; i<READERS;i++){
		wait(NULL);	
	}
	
	for(int i=0; i< WRITERS;i++){
		wait(NULL);	
	}
	
	return 0;
	
}
