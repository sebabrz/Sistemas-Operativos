/*
    SECUENCIA (c):  (A o B) C D E (A o B) C D E ...

    A y B comparten UN semaforo (ab): el que lo toma primero imprime. Como solo hay una ficha,
    imprime uno solo de los dos por ciclo. E repone la ficha y arranca el ciclo siguiente.
    (Cual de los dos gana en cada vuelta no esta definido, y el enunciado no lo pide.)
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

sem_t ab, c, d, e;

void *imprimirA(void *arg){
    while(1){
        sem_wait(&ab);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&ab);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&c);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&d);
    }
    return NULL;
}
void *imprimirD(void *arg){
    while(1){
        sem_wait(&d);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&e);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&e);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&ab);
    }
    return NULL;
}

int main(){
    pthread_t ta, tb, tc, td, te;

    sem_init(&ab, 0, 1);
    sem_init(&c, 0, 0);
    sem_init(&d, 0, 0);
    sem_init(&e, 0, 0);

    pthread_create(&ta, NULL, imprimirA, NULL);
    pthread_create(&tb, NULL, imprimirB, NULL);
    pthread_create(&tc, NULL, imprimirC, NULL);
    pthread_create(&td, NULL, imprimirD, NULL);
    pthread_create(&te, NULL, imprimirE, NULL);

    pthread_join(ta, NULL); pthread_join(tb, NULL); pthread_join(tc, NULL);
    pthread_join(td, NULL); pthread_join(te, NULL);

    sem_destroy(&ab); sem_destroy(&c); sem_destroy(&d); sem_destroy(&e);
    return 0;
}