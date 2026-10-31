#include "estructura.h"

int main(){
    char *args1[] = {"./abejas", NULL};
    char *args2[] = {"./oso", NULL};
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
	for(int i =0; i<ABEJAS;i++){
        int pidA= fork();
	    if(pidA == 0){
			    execv("./abejas", args1);
			    exit(3);
		    }
        }
        int pidO= fork();
        if(pidO == 0){
			    execv("./oso", args2);
			    exit(3);
		}
        
	
	
	sem_init(&(recursosCompartidos->vacio),1,PORCIONES);
	sem_init(&(recursosCompartidos->lleno),1,0);
	sem_init(&(recursosCompartidos->turno),1,ABEJAS);
	sem_init(&(recursosCompartidos->despertar_oso),1,0);
	sem_init(&(recursosCompartidos->mutex),1,1);
	
	for(int i=0; i<ABEJAS;i++){
		wait(NULL);	
	}
	wait(NULL);
	
	return 0;
	
}
