#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"


#define TAM_BUFFER 20

sem_t sem_lleno;
sem_t sem_vacio;
int buf[TAM_BUFFER];
int in;
int out;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* Prod(void * arg) {
    int prod_id = (int) arg;
	printf(MAGENTA "Llegó el productor %d \n" RESET, prod_id);
		unsigned int seed = time(NULL) + prod_id;
		srandom(seed);
	for(int k = 0; k < 100; k++){
			sem_wait(&sem_vacio);
			pthread_mutex_lock(&mutex); //MUTEX
				int prod = random() % 100;
				buf[in] = prod;
				printf(CYAN "Productor %d: Produje un %d \n" RESET,prod_id, prod);
				fflush(stdout);
				in = (in + 1) % TAM_BUFFER;
				sem_post(&sem_lleno);
			pthread_mutex_unlock(&mutex); //MUTEX

			sleep(2);
		}
}

void* Cons(void * arg) {
    int cons_id = (int) arg;
    while(1){

			sem_wait(&sem_lleno);
			pthread_mutex_lock(&mutex); //MUTEX
			printf(GREEN "Consumior %d: Consumi %d \n" RESET,cons_id, buf[out]);
			fflush(stdout);
			out= (out + 1) % TAM_BUFFER;
			pthread_mutex_unlock(&mutex); //MUTEX
			sem_post(&sem_vacio);
			
			sleep(1);
	}
}

void main() {
    pthread_t consum[3];
    pthread_t produc[3];

    sem_init(&sem_vacio, 0, TAM_BUFFER);
    sem_init(&sem_lleno, 0, 0);
    in = 0;
    out = 0;

    for (int i = 0; i < 3; i++) {
        pthread_create(&consum[i], NULL, Cons, (void * ) i+1);
    }
    for (int i = 0; i < 3; i++) {
        pthread_create(&produc[i], NULL, Prod, (void * ) i+1);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(consum[i], NULL);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(produc[i], NULL);
    }

    sem_destroy(&sem_lleno);
    sem_destroy(&sem_vacio);

}
