/*
    SECUENCIA (h):  AB(D o E)AC(D o E)F AB(D o E)AC(D o E)F ...

    A necesita 2 fichas (a = 2) y habilita a B y a C a la vez; B necesita 2 avisos de A (por eso b arranca en 1).
    D y E comparten el semaforo `de`: imprime el que llegue primero. F necesita 2 fichas (las de D o E).
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

sem_t a, b, c, de, f;

void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&de);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&c);
        sem_wait(&c);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&de);
    }
    return NULL;
}
void *imprimirD(void *arg){
    while(1){
        sem_wait(&de);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&f);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&de);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&f);
    }
    return NULL;
}
void *imprimirF(void *arg){
    while(1){
        sem_wait(&f);
        sem_wait(&f);
        printf("F"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
    }
    return NULL;
}

int main(){
    pthread_t ta, tb, tc, td, te, tf;

    sem_init(&a, 0, 2);
    sem_init(&b, 0, 1);
    sem_init(&c, 0, 0);
    sem_init(&de, 0, 0);
    sem_init(&f, 0, 0);

    pthread_create(&ta, NULL, imprimirA, NULL);
    pthread_create(&tb, NULL, imprimirB, NULL);
    pthread_create(&tc, NULL, imprimirC, NULL);
    pthread_create(&td, NULL, imprimirD, NULL);
    pthread_create(&te, NULL, imprimirE, NULL);
    pthread_create(&tf, NULL, imprimirF, NULL);

    pthread_join(ta, NULL); pthread_join(tb, NULL); pthread_join(tc, NULL); pthread_join(td, NULL); pthread_join(te, NULL); pthread_join(tf, NULL);

    sem_destroy(&a); sem_destroy(&b); sem_destroy(&c); sem_destroy(&de); sem_destroy(&f);
    return 0;
}