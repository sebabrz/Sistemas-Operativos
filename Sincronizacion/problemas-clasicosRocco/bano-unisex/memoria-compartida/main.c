#include "estructura.h"

int main(){
    char *args1[] = {"./mujeres", NULL};
    char *args2[] = {"./hombres", NULL};
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
        int pidA= fork();
	    if(pidA == 0){
			    execv("./mujeres", args1);
			    exit(3);
		    }
        }
	for(int i =0; i<CANT;i++){
        int pidA= fork();
	    if(pidA == 0){
			    execv("./hombres", args2);
			    exit(3);
		    }
        }
        
	
	
	sem_init(&(recursosCompartidos->MujeresDentro),1,0);
	sem_init(&(recursosCompartidos->MujeresMax),1,3);
	sem_init(&(recursosCompartidos->HombresDentro),1,0);
	sem_init(&(recursosCompartidos->HombresMax),1,3);
	sem_init(&(recursosCompartidos->genero),1,1);
	sem_init(&(recursosCompartidos->turno),1,1);
	sem_init(&(recursosCompartidos->mutexM),1,1);
	sem_init(&(recursosCompartidos->mutexH),1,1);
	
	for(int i=0; i<CANT*2;i++){
		wait(NULL);	
	}
	wait(NULL);
	
	return 0;
	
}
