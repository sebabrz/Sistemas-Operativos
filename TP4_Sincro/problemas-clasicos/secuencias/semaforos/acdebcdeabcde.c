#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5
// ACDE BCDE ABCDE
sem_t sem_a, sem_b, sem_c, sem_d, sem_e, sem_ab;

void* print_a(void* arg) {
    while (1) {
        sem_wait(&sem_a);
        sem_wait(&sem_ab);
        printf("A");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_c);
        sem_post(&sem_b);
        
        sem_wait(&sem_a);
        sem_wait(&sem_ab);
        printf("A");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_b);
        
    }
    return NULL;
}

void* print_b(void* arg) {
    while (1) {
        sem_wait(&sem_b);
        sem_wait(&sem_ab);
        printf("B");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_c);
        sem_post(&sem_a);
        
        sem_wait(&sem_b);
        printf("B");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_c);
        sem_post(&sem_a);
        
   
    }
    return NULL;
}

void* print_c(void* arg) {
    while (1) {
        sem_wait(&sem_c);
        printf("C");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_d);
    }
    return NULL;
}

void* print_d(void* arg) {
    while (1) {
        sem_wait(&sem_d);
        printf("D");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_e);
    }
    return NULL;
}

void* print_e(void* arg) {
    while (1) {
        sem_wait(&sem_e);
        printf("E");
        sleep(1);
        fflush(stdout);
        sem_post(&sem_ab);
        
    }
    return NULL;
}
int main() {
    pthread_t threads[NUM_THREADS];

    sem_init(&sem_a, 0, 1);
    sem_init(&sem_b, 0, 0);
    sem_init(&sem_c, 0, 0);
    sem_init(&sem_d, 0, 0);
    sem_init(&sem_e, 0, 0);
    sem_init(&sem_ab, 0, 1);
    
    pthread_create(&threads[0], NULL, print_a, NULL);
    pthread_create(&threads[1], NULL, print_b, NULL);
    pthread_create(&threads[2], NULL, print_c, NULL);
    pthread_create(&threads[3], NULL, print_d, NULL);
    pthread_create(&threads[4], NULL, print_e, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem_a);
    sem_destroy(&sem_b);
    sem_destroy(&sem_c);
    sem_destroy(&sem_d);
    sem_destroy(&sem_e);
    
    return 0;
}
