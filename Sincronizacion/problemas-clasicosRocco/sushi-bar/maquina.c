
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#define CANT 10
sem_t cantPersonasMax, cantPersonasUsando, maquina;

pthread_mutex_t mutex;

void * persona(void * args){
	
	while(1){
		sleep(rand() % 3);
		sem_wait(&cantPersonasMax);
		pthread_mutex_lock(&mutex);
		
		if(sem_trywait(&cantPersonasUsando) != 0){
			// soy primero
			sem_wait(&maquina);
		}
		else{
			sem_post(&cantPersonasUsando);
		}
		sem_post(&cantPersonasUsando);
		pthread_mutex_unlock(&mutex);
		
		printf("Usando la maquina \n");
		sleep(1);
		
		
		pthread_mutex_lock(&mutex);
		printf("Termine de usar la maquina \n");
		sem_wait(&cantPersonasUsando);
		if(sem_trywait(&cantPersonasUsando) != 0){
			printf("Soy el último, libero la máquina  \n");
			for(int i = 0; i<5; i++){
				sem_post(&cantPersonasMax);
			}
			sem_post(&maquina);
		}
		else{
			sem_post(&cantPersonasUsando);
		}
		
		pthread_mutex_unlock(&mutex);
	
	}
	
}


int main(){
	sem_init(&maquina, 0, 1);
	sem_init(&cantPersonasUsando, 0, 0);
	sem_init(&cantPersonasMax, 0, 5);

	
	pthread_t personas[CANT];
	
	pthread_mutex_init(&mutex,NULL);
	

	for(int i = 0; i < CANT;i++){
		pthread_create(&personas[i], NULL, persona, NULL);
	}

	
	for(int i = 0; i < CANT;i++){
		pthread_join(personas[i], NULL);
	}

	sem_destroy(&maquina);
	sem_destroy(&cantPersonasUsando);
	sem_destroy(&cantPersonasMax);
	
	pthread_mutex_destroy(&mutex);
	
	
	return 0;
}

