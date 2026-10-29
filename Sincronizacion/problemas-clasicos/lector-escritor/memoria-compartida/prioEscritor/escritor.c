#include "estructura.h"

int a_producir = 0;

int main(){
	key_t clave = generateKey();
	int id = shmget(clave, 1024, 0);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	
	struct aCompartir* memCompartida = (struct aCompartir*) shmat(id,0,0);
	if(memCompartida <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
	
	while(1){
		sem_wait(&(memCompartida -> mutexEsc));

		if(sem_trywait(&(memCompartida -> cantEscritores)) == 0)
			sem_post(&(memCompartida -> cantEscritores));
		else 
			sem_wait(&(memCompartida -> noLectores));

		sem_post(&(memCompartida -> cantEscritores));
		sem_post(&(memCompartida -> mutexEsc));
		sem_wait(&(memCompartida -> noEscritores));

		printf("escritor escribiendo\n");
		fflush(stdout);
		sleep(1);
		printf("escritor se va\n");
		fflush(stdout);
		
		sem_post(&(memCompartida -> noEscritores));

		sem_wait(&(memCompartida -> mutexEsc));
		sem_wait(&(memCompartida -> cantEscritores));
		if(sem_trywait(&(memCompartida -> cantEscritores)) == 0)
			sem_post(&(memCompartida -> cantEscritores));
		else 
			sem_post(&(memCompartida -> noLectores));

		sem_post(&(memCompartida -> mutexEsc));
		sleep(1);
	}
	return 0;
}
