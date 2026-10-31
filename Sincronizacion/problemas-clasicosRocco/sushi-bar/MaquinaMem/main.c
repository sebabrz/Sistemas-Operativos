

#include "estructura.h"

int main(){
    char *args[] = {"./persona", NULL};
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
	for(int i =0; i<CANT;i++){
            int pidC= fork();
	    if(pidC == 0){
			    execv("./persona", args);
			    exit(3);
		    }
        }
	
	
	sem_init(&(recursosCompartidos->cantPersonasMax),1,5);
	sem_init(&(recursosCompartidos->cantPersonasUsando),1,0);
	sem_init(&(recursosCompartidos->maquina),1,1);
	sem_init(&(recursosCompartidos->mutex),1,1);
	
	for(int i=0; i<CANT;i++){
		wait(NULL);	
	}
	
	
	return 0;
	
	
}
