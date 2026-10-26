#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define CANT_PASAJEROS 5

sem_t colaembarque,coladesembarque,todosadentro,todosafuera,embarq,desembarq;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER, mutex2 = PTHREAD_MUTEX_INITIALIZER; //metodos pthread_mutex_lock ,trylock y unlock

void* carro(void* args){
	while(1){
		for(int i = 0; i< CANT_PASAJEROS; i++) sem_post(&colaembarque);
		sem_wait(&todosadentro);
		printf("estoy andando\n");
		for(int i = 0; i< CANT_PASAJEROS; i++) sem_post(&coladesembarque);
		sem_wait(&todosafuera);
	}
	
	pthread_exit(NULL);
}
void* pasajero(void* args){
	//while(1){
	sem_wait(&colaembarque);
	pthread_mutex_lock(&mutex);
	sem_wait(&embarq);
	if(sem_trywait(&embarq) == 0) //
		sem_post(&embarq);
	else {sem_post(&todosadentro); for(int i = 0; i < CANT_PASAJEROS; i++) sem_post(&embarq);}
	
	pthread_mutex_unlock(&mutex);
	

	printf("viajando \n");
	sleep(1) ;
	
	sem_wait(&coladesembarque);
	pthread_mutex_lock(&mutex2);
		sem_wait(&desembarq);
	if(sem_trywait(&desembarq) == 0) 
		sem_post(&desembarq);
	else {sem_post(&todosafuera); for(int i = 0; i < CANT_PASAJEROS; i++) sem_post(&desembarq);}
		
	pthread_mutex_unlock(&mutex2);
		
		
		
	//}
	
	pthread_exit(NULL);
}


int main(int argc, char **argv){
	pthread_t pasaj[10],cayo;
	
	sem_init(&colaembarque,0,0);
	sem_init(&coladesembarque,0,0);
	sem_init(&embarq,0,CANT_PASAJEROS);
	sem_init(&desembarq,0,CANT_PASAJEROS);
	sem_init(&todosadentro,0,0);
	sem_init(&todosafuera,0,0);
	
	pthread_create(&cayo,NULL,carro,NULL);
	for(int i = 0;  i <10; i++){
		pthread_create(&pasaj[i],NULL,pasajero,NULL);
	
	
	}
	
	pthread_create(&cayo,NULL,carro,NULL);
	
	for(int i = 0;  i <10; i++){
		pthread_join(pasaj[i],NULL);
	
	
	}

	
	sem_destroy(&desembarq);
	sem_destroy(&embarq);
	sem_destroy(&coladesembarque);
	sem_destroy(&colaembarque);
	sem_destroy(&todosadentro);
	sem_destroy(&todosafuera);
	return 0;
}
