#include "defH2O.h"

int main(){
	key_t clave = generateKey();
	int id = shmget(clave, 1024, 0);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	
	struct aCompartir* hidrogeno = (struct aCompartir*) shmat(id,0,0);
	if(hidrogeno <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
	for(int k = 0; k < 1; k++){
		for(int i = 0; i < 20; i++){
			sem_wait(&(hidrogeno-> vacioH));
			sem_wait(&(hidrogeno-> mutexH)); //MUTEX
				printf("Genere un hidrogeno\n");
				(hidrogeno -> bufHidrogeno[hidrogeno -> agregarH]) = 'H';
				(hidrogeno -> agregarH) = ((hidrogeno -> agregarH) + 1) % TAM_BUFFER;
				sem_post(&(hidrogeno -> llenoH));
			sem_post(&(hidrogeno-> mutexH)); //MUTEX
			
			sleep(2);
			
		}
	sleep(15);
	}
	return 0;
}
