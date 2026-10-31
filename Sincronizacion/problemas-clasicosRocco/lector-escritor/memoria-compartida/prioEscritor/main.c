#include "estructura.h"

int main(){
	char nroLector[CANTLECTORES];
    char *args1[] = {"./escritor", NULL};
    char *args2[] = {"./lector", nroLector,NULL};
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
	
    int pidE = fork();
    if(pidE == 0){
		execv("./escritor", args1);
		exit(3);
	}    
	for(int i = 0; i < 3; i++){
		int pidL = fork();
		if(pidL == 0){
			sprintf(nroLector, "%d", i + 1);
			execv("./lector", args2);
			printf("error");
			exit(4);
		}
	}
	
	sem_init(&(recursosCompartidos->noLectores),1,1);
	sem_init(&(recursosCompartidos->noEscritores),1,1);
	sem_init(&(recursosCompartidos->cantEscritores),1,0);
	sem_init(&(recursosCompartidos->cantLectores),1,0);
	sem_init(&(recursosCompartidos->mutexLect),1,1);
	sem_init(&(recursosCompartidos->mutexEsc),1,1);
	
	for(int i = 0; i < 3; i++){
		wait(NULL);
	}
	wait(NULL);
	
	sem_destroy(&(recursosCompartidos->noLectores));
	sem_destroy(&(recursosCompartidos->noEscritores));
	sem_destroy(&(recursosCompartidos->cantEscritores));
	sem_destroy(&(recursosCompartidos->cantLectores));
	sem_destroy(&(recursosCompartidos->mutexLect));
	sem_destroy(&(recursosCompartidos->mutexEsc));

	shmdt(recursosCompartidos);

	shmctl(id, IPC_RMID, NULL);
	
	return 0;
}

