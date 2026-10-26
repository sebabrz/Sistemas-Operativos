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
		sem_wait(&(memCompartida->MujeresMax));
		sem_wait(&(memCompartida->mutexM));
		sem_post(&(memCompartida->turno));
		if(sem_trywait(&(memCompartida->MujeresDentro)) == 0){
			//No soy la primer mujer
			sem_post(&(memCompartida->MujeresDentro));
		}else{
			//Soy la primer mujer
			//Bloque el recurso
			sem_wait(&(memCompartida->genero));
		}
		//Cuento que una mujer entro
		sem_post(&(memCompartida->MujeresDentro));
		printf("%s La mujer  entro\n",PINK);

		sem_post(&(memCompartida->mutexM));

		printf("%sMujer esta en al baño\n",PINK );
		//simulo el tiempo
		sleep(1);


		sem_wait(&(memCompartida->mutexM));
		
		printf("%s La mujer se fue\n",PINK);
		sem_wait(&(memCompartida->MujeresDentro));

		if(sem_trywait(&(memCompartida->MujeresDentro)) == 0){
			//No soy la ultima mujer
			sem_post(&(memCompartida->MujeresDentro));
		}else{
			//Soy la ultima mujer
			//Libero el recurso
			sem_post(&(memCompartida->genero));
		}
		sem_post(&(memCompartida->mutexM));
		sem_post(&(memCompartida->MujeresMax));
	}


    return 0;		
}	
