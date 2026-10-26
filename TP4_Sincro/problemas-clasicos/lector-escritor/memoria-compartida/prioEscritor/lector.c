#include "estructura.h"

int a_producir = 0;

int main(int argc, char* argv[]){
	int idL = atoi(argv[1]);
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
		sem_wait(&(memCompartida -> noLectores));
		sem_wait(&(memCompartida -> mutexLect));

		if(sem_trywait(&(memCompartida -> cantLectores)) == 0)
			sem_post(&(memCompartida -> cantLectores));
		else 
			sem_wait(&(memCompartida -> noEscritores));

		sem_post(&(memCompartida -> cantLectores));
		sem_post(&(memCompartida -> mutexLect));
		sem_post(&(memCompartida -> noLectores));

		printf("lector %d leyendo\n", idL);
		fflush(stdout);
		sleep(1);
		printf("lector %d se va\n", idL);
		fflush(stdout);
		
		sem_wait(&(memCompartida -> mutexLect));
		sem_wait(&(memCompartida -> cantLectores));
		if(sem_trywait(&(memCompartida -> cantLectores)) == 0)
			sem_post(&(memCompartida -> cantLectores));
		else 
			sem_post(&(memCompartida -> noEscritores));

		sem_post(&(memCompartida -> mutexLect));	
		
	}
	return 0;
}
