#include "estructura.h"

int main() {
    key_t clave = generateKey();
	int id = shmget(clave, 1024, 0);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	
	struct aCompartir* persona = (struct aCompartir*) shmat(id,0,0);
	if(persona <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
	
 
	while(1){
		sleep(rand() % 3);
		
		sem_wait(&(persona-> cantPersonasMax));
		sem_wait(&(persona-> mutex));
		
		if(sem_trywait(&(persona-> cantPersonasUsando)) != 0){
			// soy primero
			
			sem_wait(&(persona-> maquina));
		}
		else{
			
			sem_post(&(persona-> cantPersonasUsando));
		}
		sem_post(&(persona-> cantPersonasUsando));
		sem_post(&(persona-> mutex));
		
		printf("Usando la maquina \n");
		sleep(1);
		
		
		sem_wait(&(persona-> mutex));
		printf("Termine de usar la maquina \n");
		sem_wait(&(persona-> cantPersonasUsando));
		
		if(sem_trywait(&(persona-> cantPersonasUsando)) != 0){
			printf("Soy el último, libero la máquina  \n");
			for(int i = 0; i<5; i++){
				sem_post(&(persona-> cantPersonasMax));
			}
			sem_post(&(persona-> maquina));
		}
		else{
			sem_post(&(persona-> cantPersonasUsando));
		}
		
		sem_post(&(persona-> mutex));
	
	}


    return 0;
}
