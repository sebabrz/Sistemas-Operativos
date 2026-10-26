#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#define CANT 10

sem_t a, b, c,de,f;


void * fA(void * args){
	
	while(1){
		sem_wait(&a);
		sem_wait(&a);
		printf("A");
		fflush(stdout);
		sem_post(&b);
		sem_post(&c);
		
	
		
	}
	
}

void * fB(void * args){
	
	while(1){
		sem_wait(&b);
		sem_wait(&b);
		printf("B");
		fflush(stdout);
		sem_post(&a);
		sem_post(&de);
		
	}
	
}

void * fC(void * args){
	
	while(1){
		sem_wait(&c);
		sem_wait(&c);
		printf("C");
		fflush(stdout);
		sem_post(&de);
			
		
	}
	
}

void * fD(void * args){
	
	while(1){
			sem_wait(&de);
			printf("D");
			fflush(stdout);
			sem_post(&a);
			sem_post(&f);
		
	}
	
}

void * fE(void * args){
	
	while(1){
	
			sem_wait(&de);
			printf("E");
			fflush(stdout);
			sem_post(&a);
			sem_post(&f);
		
			
		
	}
	
}
void * fF(void * args){
	
	while(1){
		sem_wait(&f);
		sem_wait(&f);
		printf("F");
		fflush(stdout);
		sem_post(&a);
		
	}
	
}


int main(){
	sem_init(&a, 0, 2);
	sem_init(&b, 0, 1);
	sem_init(&c, 0, 0);
		sem_init(&de, 0, 0);
			
				sem_init(&f, 0, 0);

	
	pthread_t aa;
	pthread_t bb;
	pthread_t cc;
	pthread_t dd;
	pthread_t ee;
	pthread_t ff;
	
	

	
		pthread_create(&aa, NULL, fA, NULL);
		pthread_create(&bb, NULL, fB, NULL);
		pthread_create(&cc, NULL, fC, NULL);
		pthread_create(&dd, NULL, fD, NULL);
		pthread_create(&ee, NULL, fE, NULL);
		pthread_create(&ff, NULL, fF, NULL);
	
	

	
	
		pthread_join(aa, NULL);
		pthread_join(bb, NULL);
		pthread_join(cc, NULL);
		pthread_join(dd, NULL);
		pthread_join(ee, NULL);
		pthread_join(ff, NULL);
	
	
	

	sem_destroy(&a);
	sem_destroy(&b);
	sem_destroy(&c);
	sem_destroy(&de);
	
	sem_destroy(&f);
	

	
	
	
	return 0;
}
