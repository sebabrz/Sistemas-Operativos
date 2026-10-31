/*
    SECUENCIA (l):  AA B (C o D)  AA B (C o D) ...

    A arranca con 2 fichas (a = 2): imprime dos veces. B espera 2 avisos de A.
    C y D comparten `cd`: imprime el que llegue primero y devuelve 2 fichas a A.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Pausa solo para poder leer la salida en pantalla (no sincroniza nada)
#ifndef PAUSA_US
#define PAUSA_US 200000
#endif

sem_t a, b, cd;

void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&cd);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&cd);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&a);
    }
    return NULL;
}
void *imprimirD(void *arg){
    while(1){
        sem_wait(&cd);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&a);
    }
    return NULL;
}

int main(){
    pthread_t ta, tb, tc, td;

    sem_init(&a, 0, 2);
    sem_init(&b, 0, 0);
    sem_init(&cd, 0, 0);

    pthread_create(&ta, NULL, imprimirA, NULL);
    pthread_create(&tb, NULL, imprimirB, NULL);
    pthread_create(&tc, NULL, imprimirC, NULL);
    pthread_create(&td, NULL, imprimirD, NULL);

    pthread_join(ta, NULL); pthread_join(tb, NULL); pthread_join(tc, NULL); pthread_join(td, NULL);

    sem_destroy(&a); sem_destroy(&b); sem_destroy(&cd);
    return 0;
}