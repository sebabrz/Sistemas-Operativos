#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#define CANT 10

sem_t a, b, c;


void * fA(void * args){
	
	while(1){
		for(int i=0;i<2;i++){
			sem_wait(&a);
			printf("A");
			sem_post(&b);
	}
		sem_wait(&a);
	
		
	}
	
}

void * fB(void * args){
	
	while(1){
			sem_wait(&b);
			printf("B");
			sem_post(&c);
			sem_post(&a);
		
	}
	
}

void * fC(void * args){
	
	while(1){
			sem_wait(&c);
			sem_wait(&c);
			printf("C");
			sem_post(&a);
		
	}
	
}


int main(){
	sem_init(&a, 0, 1);
	sem_init(&b, 0, 0);
	sem_init(&c, 0, 0);

	
	pthread_t aa;
	pthread_t bb;
	pthread_t cc;
	
	

	
		pthread_create(&aa, NULL, fA, NULL);
		pthread_create(&bb, NULL, fB, NULL);
		pthread_create(&cc, NULL, fC, NULL);
	
	

	
	
		pthread_join(aa, NULL);
		pthread_join(bb, NULL);
		pthread_join(cc, NULL);
	
	
	

	sem_destroy(&a);
	sem_destroy(&b);
	sem_destroy(&c);
	

	
	
	
	return 0;
}
