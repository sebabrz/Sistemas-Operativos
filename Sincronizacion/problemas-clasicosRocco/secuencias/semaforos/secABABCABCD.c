#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 4

sem_t sem_a, sem_b, sem_c, sem_d;

void* print_a(void* arg) {
    while (1) {
        sem_wait(&sem_a);
        printf("A");
        fflush(stdout);
        sem_post(&sem_b);
    }
    return NULL;
}

void* print_b(void* arg) {
    while (1) {
        sem_wait(&sem_b);
        printf("B");
        fflush(stdout);
        sem_post(&sem_a);
        
        for(int i =0; i<2; i++){
            sem_wait(&sem_b);
            printf("B");
            fflush(stdout);
            sem_post(&sem_c);
        }
        
        
    }
    return NULL;
}

void* print_c(void* arg) {
    while (1) {
        sem_wait(&sem_c);
        printf("C");
        fflush(stdout);
        sem_post(&sem_a);
        
        sem_wait(&sem_c);
        printf("C");
        fflush(stdout);
        sem_post(&sem_d);
    }
    return NULL;
}

void* print_d(void* arg) {
    while (1) {
        sem_wait(&sem_d);
        printf("D  \n");
        fflush(stdout);
        sem_post(&sem_a);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    sem_init(&sem_a, 0, 1);  // Inicializa sem_a a 1 para comenzar la secuencia
    sem_init(&sem_b, 0, 0);
    sem_init(&sem_c, 0, 0);
    sem_init(&sem_d, 0, 0);

    pthread_create(&threads[0], NULL, print_a, NULL);
    pthread_create(&threads[1], NULL, print_b, NULL);
    pthread_create(&threads[2], NULL, print_c, NULL);
    pthread_create(&threads[3], NULL, print_d, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem_a);
    sem_destroy(&sem_b);
    sem_destroy(&sem_c);
    sem_destroy(&sem_d);

    return 0;
}
