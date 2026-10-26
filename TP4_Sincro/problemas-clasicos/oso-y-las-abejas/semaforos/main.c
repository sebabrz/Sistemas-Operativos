#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TIME 1
#define PORCIONES 10
#define ABEJAS 3

sem_t sem_tarro_vacio;
sem_t sem_tarro_lleno;
sem_t sem_oso;
sem_t sem_abejas;
pthread_mutex_t mutex_abejas;

void* abeja() {
    while (1) {
        sleep(TIME);

        sem_wait(&sem_abejas);
        pthread_mutex_lock(&mutex_abejas);

        sem_wait(&sem_tarro_vacio);
        if (sem_trywait(&sem_tarro_vacio) == 0) {
            // no soy ultima abeja
            sem_post(&sem_tarro_vacio);
            sem_post(&sem_abejas);
        } else {
            // soy ultima abeja
            sem_post(&sem_oso);
        }

        // guardo porcion miel
        printf("\033[1;33mAbeja GUARDA miel\033[0m\n");
        sem_post(&sem_tarro_lleno);

        pthread_mutex_unlock(&mutex_abejas);
    }
}

void* oso() {
    while (1) {
        sleep(TIME);
        sem_wait(&sem_oso);

        printf("\033[1;31mUN OSO WACHO\033[0m\n");
        for (int i = 0; i < PORCIONES; i++) {
            sem_wait(&sem_tarro_lleno);
            printf("\033[0;33mOso COME miel %d\033[0m\n",i);
            sem_post(&sem_tarro_vacio);
        }
        
        sem_post(&sem_abejas);
    }
}

void main() {
    pthread_t abejas_t[ABEJAS];
    pthread_t oso_t;

    sem_init(&sem_tarro_vacio, 0, PORCIONES);
    sem_init(&sem_tarro_lleno, 0, 0);
    sem_init(&sem_abejas, 0, 1);
    sem_init(&sem_oso, 0, 0);

    for (int i = 0; i < ABEJAS; i++) {
        pthread_create(&abejas_t[i], NULL, abeja, NULL);
    }

    pthread_create(&oso_t, NULL, oso, NULL);

    pthread_join(oso_t, NULL);
    
    for (int i = 0; i < ABEJAS; i++) {
        pthread_join(abejas_t[i], NULL);
    }

    sem_destroy(&sem_tarro_vacio);
    sem_destroy(&sem_tarro_lleno);
    sem_destroy(&sem_abejas);
    sem_destroy(&sem_oso);
}