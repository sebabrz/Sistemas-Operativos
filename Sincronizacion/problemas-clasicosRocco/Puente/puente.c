
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#define CANT 10
sem_t cruzandoNorte, cruzandoSur, puente;

pthread_mutex_t mutexN, mutexS;

void * sur(void * args){
	
	while(1){
		sleep(1);
		pthread_mutex_lock(&mutexS);
		
		if(sem_trywait(&cruzandoSur) == 0){
			sem_post(&cruzandoSur);
		}
		else{
			printf("Esperando para cruzar desde el sur  \n");
			sem_wait(&puente);
		}
		sem_post(&cruzandoSur);
		
		pthread_mutex_unlock(&mutexS);
		
		printf("Cruzando el puente desde el sur  \n");
		sleep(1);
		
		pthread_mutex_lock(&mutexS);
		sem_wait(&cruzandoSur);
		if(sem_trywait(&cruzandoSur) != 0){
			// soy ultimo
			sem_post(&puente);
		}
		else{
			sem_post(&cruzandoSur);
		}
		pthread_mutex_unlock(&mutexS);
		sleep(2);
	}
	
}

void * norte(void * args){
	
	while(1){
		sleep(1);
		pthread_mutex_lock(&mutexN);
		
		if(sem_trywait(&cruzandoNorte) == 0){
			sem_post(&cruzandoNorte);
		}
		else{
			printf("Esperando para cruzar desde el norte  \n");
			sem_wait(&puente);
		}
		sem_post(&cruzandoNorte);
		
		pthread_mutex_unlock(&mutexN);
		
		printf("Cruzando el puente desde el norte  \n");
		sleep(1);
		
		pthread_mutex_lock(&mutexN);
		sem_wait(&cruzandoNorte);
		if(sem_trywait(&cruzandoNorte) != 0){
			// soy ultimo
			sem_post(&puente);
		}
		else{
			sem_post(&cruzandoNorte);
		}
		pthread_mutex_unlock(&mutexN);
		sleep(2);
	}
	
}

int main(){
	sem_init(&cruzandoNorte, 0, 0);
	sem_init(&cruzandoSur, 0, 0);
	sem_init(&puente, 0, 1);

	
	pthread_t surA[CANT];
	pthread_t norteA[CANT];
	
	pthread_mutex_init(&mutexS,NULL);
	pthread_mutex_init(&mutexN,NULL);

	for(int i = 0; i < CANT;i++){
		pthread_create(&surA[i], NULL, sur, NULL);
	}
	
	for(int i = 0; i < CANT;i++){
		pthread_create(&norteA[i], NULL, norte, NULL);
	}
	
	for(int i = 0; i < CANT;i++){
		pthread_join(norteA[i], NULL);
	}
	for(int i = 0; i < CANT;i++){
		pthread_join(surA[i], NULL);
	}
	

	sem_destroy(&cruzandoNorte);
	sem_destroy(&cruzandoSur);
	sem_destroy(&puente);
	
	pthread_mutex_destroy(&mutexN);
	pthread_mutex_destroy(&mutexS);
	
	
	
	return 0;
}
