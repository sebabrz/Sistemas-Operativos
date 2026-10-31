/*
    SECUENCIA (b):  A CDE B CDE A CDE B CDE ...

    C, D y E se repiten en cada vuelta; lo que alterna es A y B.
    Entonces A y B le hacen post a C (no uno al otro), y E, que imprime dos veces por ciclo,
    alterna a quien habilita: la primera vez post(B), la segunda post(A).

    OJO: en el pseudocodigo del apunte A hacia signal(B); eso da "A B C D E..." (B imprimiria enseguida).
    Tiene que ser post(C).
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

sem_t a, b, c, d, e;

void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);                 // despues de A viene C (no B)
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
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
        sem_wait(&e);                 // E del ciclo que empezo A
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);                 // ahora le toca a B

        sem_wait(&e);                 // E del ciclo que empezo B
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);                 // ahora le toca a A
    }
    return NULL;
}

int main(){
    pthread_t ta, tb, tc, td, te;

    sem_init(&a, 0, 1);
    sem_init(&b, 0, 0);
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

    sem_destroy(&a); sem_destroy(&b); sem_destroy(&c); sem_destroy(&d); sem_destroy(&e);
    return 0;
}