#include "defH2O.h"

int main(){
	key_t clave = generateKey();
	int id = shmget(clave, 1024, 0);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	
	struct aCompartir* oxigeno = (struct aCompartir*) shmat(id,0,0);
	if(oxigeno <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
	for(int k = 0; k < 1; k++){
		for(int i = 0; i < 15; i++){
			sem_wait(&(oxigeno-> vacioOx));
			sem_wait(&(oxigeno-> mutexOx)); //MUTEX
				printf("Genere un oxigeno\n");
				(oxigeno -> bufOxigeno[oxigeno -> agregarOx]) = 'O';
				(oxigeno -> agregarOx) = ((oxigeno -> agregarOx) + 1) % TAM_BUFFER;
				sem_post(&(oxigeno -> llenoOx));
			sem_post(&(oxigeno-> mutexOx)); //MUTEX
			
			sleep(2);
			
		}
	sleep(15);
	}
	return 0;
}
