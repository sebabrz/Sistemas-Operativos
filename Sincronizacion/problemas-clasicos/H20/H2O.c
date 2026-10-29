#include "defH2O.h"

int main()
{
	char *argsHidrogeno[] = {"./genHidrogeno", NULL};
	char *argsOxigeno[] = {"./genOxigeno", NULL};
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
	//Valor inicial al segmento de memoria compartida
	sem_init(&(recursosCompartidos->vacioOx),1,100);
	sem_init(&(recursosCompartidos->vacioH),1,100);
	sem_init(&(recursosCompartidos->llenoOx),1,0);
	sem_init(&(recursosCompartidos->llenoH),1,0);
	sem_init(&(recursosCompartidos->mutexH),1,1);
	sem_init(&(recursosCompartidos->mutexOx),1,1);
	
	recursosCompartidos->agregarH = 0;
	recursosCompartidos->sacarH = 0;
	recursosCompartidos->agregarOx = 0;
	recursosCompartidos->sacarOx = 0;
	
	int pidOx = fork();
	if(pidOx == 0){
		execv("./genOxigeno", argsOxigeno);
		exit(3);
	}
	
	int pidH = fork();
	if(pidH == 0){
		execv("./genHidrogeno", argsHidrogeno);
		exit(4);
	}
	
	while(1){
		
			sem_wait(&(recursosCompartidos-> llenoOx));
			printf("Soy el generador de moleculas\n");
			sem_wait(&(recursosCompartidos-> mutexOx)); //MUTEX
				printf("Tome un oxigeno\n");
				(recursosCompartidos -> bufOxigeno[recursosCompartidos -> sacarOx]) = '\0';
				recursosCompartidos -> sacarOx = ((recursosCompartidos -> sacarOx) + 1) % TAM_BUFFER;
			sem_post(&(recursosCompartidos-> mutexOx));	//MUTEX
			sem_post(&(recursosCompartidos -> vacioOx));
			
			for(int j = 0; j < 2; j++){
				sem_wait(&(recursosCompartidos-> llenoH));
				sem_wait(&(recursosCompartidos-> mutexH)); //MUTEX
					printf("Tome un hidrogeno\n");
					(recursosCompartidos -> bufHidrogeno[recursosCompartidos -> sacarH]) = '\0';
					recursosCompartidos -> sacarH = ((recursosCompartidos -> sacarH) + 1) % TAM_BUFFER;
				sem_post(&(recursosCompartidos-> mutexH));	//MUTEX
				sem_post(&(recursosCompartidos -> vacioH));	
			}
				printf("Genere un molecula H2O\n");
				
			sleep(1);
	}	
	return 0;
	
	
}
