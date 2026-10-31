#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <stdint.h>     // enteros de tamaño fijo
#include <string.h>     // manejo de strings
#include <semaphore.h>

sem_t op1, op2, op3, rueda, cuadro, manillar;

void* ruedas(){
    while(1){
        sem_wait(&op1);
        printf("Fabricando rueda \n");
        sem_post(&rueda);
    }
}
void* cuadros(){
    while(1){
        sem_wait(&op2);
        printf("Fabricando cuadro \n");
        sem_post(&cuadro);
    }
}
void* manillares(){
    while(1){
        sem_wait(&op3);
        printf("Fabricando manillar \n");
        sem_post(&manillar);
    }
}
void* montar(){
    while(1){
        sem_wait(&rueda);
        sem_post(&op1);
        sem_wait(&rueda);
        sem_post(&op1);
        sem_wait(&cuadro);
        sem_post(&op2);
        sem_wait(&manillar);
        sem_post(&op3);
        printf("Fabricando bicicleta \n");
    }
}

int main() {
    sem_init(&op1,0,1);
    sem_init(&op2,0,1);
    sem_init(&op3,0,1);
    sem_init(&rueda,0,0);
    sem_init(&cuadro,0,0);
    sem_init(&manillar,0,0);

    pthread_t operando1, operando2, operando3, montador;
    
    pthread_create(&operando1, NULL, &ruedas, NULL);
    pthread_create(&operando2, NULL, &cuadros, NULL);
    pthread_create(&operando3, NULL, &manillares, NULL);
    pthread_create(&montador, NULL, &montar, NULL);

    pthread_join(operando1,NULL);
    pthread_join(operando2,NULL);
    pthread_join(operando3,NULL);
    pthread_join(montador,NULL);
    
    sem_close(&op1);
	sem_close(&op2);
	sem_close(&op3);
	sem_close(&rueda);
	sem_close(&cuadro);
    sem_close(&manillar);
	return 0;
}