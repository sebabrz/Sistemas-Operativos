
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define ORANGE "\033[38;5;214m"

#define CANT 20
sem_t cantEgipcios, cleopatra;
pthread_mutex_t mutex;

void* egipcio(void* args){
		
	while(1){
		
		pthread_mutex_lock(&mutex);
		if(sem_trywait(&cantEgipcios) != 0){
			sem_wait(&cleopatra);
			printf(MAGENTA "Llego un egipcio\n" RESET);
		}else{
			printf(MAGENTA "Llego un egipcio a la fiesta\n" RESET);
			sem_post(&cantEgipcios);
		}
		
		sem_post(&cantEgipcios);
		pthread_mutex_unlock(&mutex);
		sleep(1);
		
		
		pthread_mutex_lock(&mutex);
		printf(ORANGE "Se va un egipcio\n" RESET);
		sem_wait(&cantEgipcios);
		if(sem_trywait(&cantEgipcios) != 0){
			printf(ORANGE "Termina la fiesta \n" RESET);
			sem_post(&cleopatra);
		}else{
			sem_post(&cantEgipcios);
		}
		pthread_mutex_unlock(&mutex);
		
		sleep(5);
	}	
		
}

void* romano(void* args){
	while(1){
		sem_wait(&cleopatra);
		printf(RED "Llego un romano \n" RESET);
		sleep(1);
		printf(ORANGE "Se va un romano \n" RESET);
		sem_post(&cleopatra);
		
		sleep(5);
	}
}



int main(){
	sem_init(&cleopatra, 0, 1);
	sem_init(&cantEgipcios, 0, 0);

	pthread_t egipcios[CANT];
	pthread_t romanos[CANT];
	
	pthread_mutex_init(&mutex,NULL);
	
		
	
	for(int i = 0; i < CANT;i++){
		pthread_create(&egipcios[i], NULL, egipcio, NULL);
	}
	for(int i = 0; i < CANT;i++){
		pthread_create(&romanos[i], NULL, romano, NULL);
	}

	
	
	for(int i = 0; i < CANT;i++){
		pthread_join(romanos[i], NULL);
	}
	for(int i = 0; i < CANT;i++){
		pthread_join(egipcios[i], NULL);
	}

	sem_destroy(&cantEgipcios);
	sem_destroy(&cleopatra);	
	pthread_mutex_destroy(&mutex);
	
	
	return 0;
}