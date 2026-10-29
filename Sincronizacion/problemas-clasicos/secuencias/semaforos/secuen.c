#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#define CANT 10

// (AoB)CE(AoB)(AoB)DE
sem_t aob, c,d,e;


void * fA(void * args){

	while(1){
		sem_wait(&aob);
		printf("A");
		fflush(stdout);
		sem_post(&c);
		sem_post(&d);
	}

}

void * fB(void * args){

	while(1){
		sem_wait(&aob);
		printf("B");
		fflush(stdout);
		sem_post(&c);
		sem_post(&d);

	}

}

void * fC(void * args){

	while(1){
		sem_wait(&c);
		sem_wait(&c);
		sem_wait(&c);

		printf("C");
		fflush(stdout);
		sem_post(&e);
	}

}

void * fD(void * args){

	while(1){
		sem_wait(&d);
		sem_wait(&d);
		sem_wait(&d);
		printf("D");
		fflush(stdout);
		sem_post(&e);
	}

}

void * fE(void * args){

	while(1){

			sem_wait(&e);
			printf("E");
			fflush(stdout);
			sem_post(&aob);
			sem_post(&aob);
			
			sem_wait(&e);
			printf("E\n");
			fflush(stdout);
			sem_post(&aob);
	}

}


int main(){
	sem_init(&aob, 0, 1);
	sem_init(&c, 0, 2);
	sem_init(&d, 0, 0);
	sem_init(&e, 0, 0);



	pthread_t aa;
	pthread_t bb;
	pthread_t cc;
	pthread_t dd;
	pthread_t ee;




		pthread_create(&aa, NULL, fA, NULL);
		pthread_create(&bb, NULL, fB, NULL);
		pthread_create(&cc, NULL, fC, NULL);
		pthread_create(&dd, NULL, fD, NULL);
		pthread_create(&ee, NULL, fE, NULL);





		pthread_join(aa, NULL);
		pthread_join(bb, NULL);
		pthread_join(cc, NULL);
		pthread_join(dd, NULL);
		pthread_join(ee, NULL);




	sem_destroy(&aob);
	sem_destroy(&d);
	sem_destroy(&c);
	sem_destroy(&e);





	return 0;
}
