#include "estructura.h"

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
		sem_wait(&(memCompartida->turno));
		sem_wait(&(memCompartida->HombresMax));
		sem_wait(&(memCompartida->mutexH));
		sem_post(&(memCompartida->turno));
		if(sem_trywait(&(memCompartida->HombresDentro)) == 0){
			//No soy la primer Hombre
			sem_post(&(memCompartida->HombresDentro));
		}else{
			//Soy la primer Hombre
			//Bloque el recurso
			sem_wait(&(memCompartida->genero));
		}
		//Cuento que una Hombre entro
		sem_post(&(memCompartida->HombresDentro));
		printf("%s EL Hombre  entro\n",LIGHT_BLUE);

		sem_post(&(memCompartida->mutexH));

		printf("%sHombre esta en el baño\n",LIGHT_BLUE);
		//simulo el tiempo
		sleep(1);


		sem_wait(&(memCompartida->mutexH));
		
		printf("%s EL Hombre  se fue\n",LIGHT_BLUE);
		sem_wait(&(memCompartida->HombresDentro));

		if(sem_trywait(&(memCompartida->HombresDentro)) == 0){
			//No soy la ultima Hombre
			sem_post(&(memCompartida->HombresDentro));
		}else{
			//Soy la ultima Hombre
			//Libero el recurso
			sem_post(&(memCompartida->genero));
		}
		sem_post(&(memCompartida->mutexH));
		sem_post(&(memCompartida->HombresMax));
	}


    return 0;		
}	
