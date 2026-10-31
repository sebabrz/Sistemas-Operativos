/*
    SECUENCIA (d):  (A o B) C E (A o B)(A o B) D E (A o B) C E (A o B)(A o B) D E ...

    Un ciclo completo son DOS vueltas:
        vuelta 1:  X C E        (X = A o B)
        vuelta 2:  X X D E
    A y B comparten la ficha "ab"; cada vez que imprimen avisan con "fin".
      - C consume 1 fin  (espera UN A o B)
      - D consume 2 fin  (espera DOS A o B)
    turnoC / turnoD deciden a quien le toca consumir los fin (si no, C podria comerse los fin de la vuelta 2).
    E, que imprime dos veces por ciclo, habilita lo que viene:
      - despues de C: habilita a D y deja pasar DOS A o B (ab x2)
      - despues de D: habilita a C y deja pasar UN A o B (ab x1)
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

sem_t ab, fin, turnoC, turnoD, e;

void *imprimirA(void *arg){
    while(1){
        sem_wait(&ab);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&fin);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&ab);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&fin);
    }
    return NULL;
}
void *imprimirC(void *arg){            // espera 1 A o B
    while(1){
        sem_wait(&turnoC);
        sem_wait(&fin);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&e);
    }
    return NULL;
}
void *imprimirD(void *arg){            // espera 2 A o B
    while(1){
        sem_wait(&turnoD);
        sem_wait(&fin);
        sem_wait(&fin);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&e);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&e);                   // E que viene despues de C
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&turnoD);
        sem_post(&ab); sem_post(&ab);   // habilita los dos A o B seguidos

        sem_wait(&e);                   // E que viene despues de D
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&turnoC);
        sem_post(&ab);                  // habilita el A o B que abre el ciclo
    }
    return NULL;
}

int main(){
    pthread_t ta, tb, tc, td, te;

    sem_init(&ab, 0, 1);
    sem_init(&fin, 0, 0);
    sem_init(&turnoC, 0, 1);
    sem_init(&turnoD, 0, 0);
    sem_init(&e, 0, 0);

    pthread_create(&ta, NULL, imprimirA, NULL);
    pthread_create(&tb, NULL, imprimirB, NULL);
    pthread_create(&tc, NULL, imprimirC, NULL);
    pthread_create(&td, NULL, imprimirD, NULL);
    pthread_create(&te, NULL, imprimirE, NULL);

    pthread_join(ta, NULL); pthread_join(tb, NULL); pthread_join(tc, NULL);
    pthread_join(td, NULL); pthread_join(te, NULL);

    sem_destroy(&ab); sem_destroy(&fin); sem_destroy(&turnoC); sem_destroy(&turnoD); sem_destroy(&e);
    return 0;
}