#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#define lectores 10
#define escritores 3


sem_t noLectores,noEscritores,cantEscritores,cantLectores;
sem_t mutexLect, mutexEsc;

void *escritor(void *arg){
	while(1){
	
	sem_wait(&mutexEsc);
		
		if(sem_trywait(&cantEscritores) == 0) 
			sem_post(&cantEscritores);
		else sem_wait(&noLectores);
		
		sem_post(&cantEscritores);
	sem_post(&mutexEsc);

		sem_wait(&noEscritores);
		
		printf("escritor escribiendo\n");
		sleep(3);
	
		sem_post(&noEscritores);
		
	sem_wait(&mutexEsc);
		sem_wait(&cantEscritores);
		if(sem_trywait(&cantEscritores) == 0) 
			sem_post(&cantEscritores);
		else sem_post(&noLectores);
		
	sem_post(&mutexEsc);
	}
	
	pthread_exit(0);
}

void *lector(void *arg){
	do{
		
	sem_wait(&noLectores);
		sem_wait(&mutexLect);
		
		if(sem_trywait(&cantLectores) == 0) 
			sem_post(&cantLectores);
		else sem_wait(&noEscritores);
		
		sem_post(&cantLectores);
		sem_post(&mutexLect);
	sem_post(&noLectores);
	
	
	printf("lector leyendo\n");
	sleep(3);
	sem_wait(&mutexLect);
		sem_wait(&cantLectores);
		if(sem_trywait(&cantLectores) == 0) 
			sem_post(&cantLectores);
		else sem_post(&noEscritores);
		
	sem_post(&mutexLect);
	
	}while(1);
	
	pthread_exit(0);
}

int main(){
	

	
	
	
	pthread_t  hilos_lect[lectores], hilos_esc[escritores];
	
	
	sem_init(&noLectores,0,1);
	sem_init(&noEscritores,0,1);
	sem_init(&cantEscritores,0,0);
	sem_init(&cantLectores,0,0);
	sem_init(&mutexEsc,0,1);
	sem_init(&mutexLect,0,1);
	
	
	for(int i = 0; i<lectores; i++){
	pthread_create(&hilos_lect[i],NULL,lector,NULL);
	
	
	}
	for(int i = 0; i<escritores; i++){
	pthread_create(&hilos_esc[i],NULL,escritor,NULL);
	
	
	}
	
	
	
	for(int i = 0; i<lectores; i++){
	pthread_join(hilos_lect[i],NULL);
	
	
	}
	for(int i = 0; i<escritores; i++){
	pthread_join(hilos_esc[i],NULL);
	
	
	}
	
	sem_close(&noLectores);
	sem_close(&noEscritores);
	sem_close(&cantEscritores);
	sem_close(&cantLectores);
	sem_close(&mutexEsc);
	sem_close(&mutexLect);

	return 0;
	
}
