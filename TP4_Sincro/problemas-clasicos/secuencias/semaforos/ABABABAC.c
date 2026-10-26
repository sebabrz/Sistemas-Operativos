#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define REPETICIONES 5
// Semáforos para sincronizar la ejecución de los hilos
sem_t sem_A, sem_B, sem_C;

void* threadA(void* arg) {
    for(int i=0;i<REPETICIONES;i++) {
        for(int i=0;i<3;i++){
            sem_wait(&sem_A); // Espera a que le den permiso de ejecución
            printf("A");
            fflush(stdout);
            sem_post(&sem_B); // Desbloquea al hilo B
        }
        sem_wait(&sem_A);
        printf("A");
        fflush(stdout);
        sem_post(&sem_C); // Desbloquea al hilo C
    }
    return NULL;
}

void* threadB(void* arg) {
    for(int i=0;i<(REPETICIONES*3);i++) {
        sem_wait(&sem_B); // Espera a que le den permiso de ejecución
        printf("B");
        fflush(stdout);
        sem_post(&sem_A); // Desbloquea al hilo A
    }
    return NULL;
}

void* threadC(void* arg) {
    for(int i=0;i<REPETICIONES;i++) {
        sem_wait(&sem_C); // Espera a que le den permiso de ejecución
        printf("C \n");
        fflush(stdout);
        sem_post(&sem_A); // Desbloquea al hilo A para la siguiente iteración
    }
    return NULL;
}

int main() {
    // Declaración de los hilos
    pthread_t tidA, tidB, tidC;

    // Inicialización de los semáforos
    sem_init(&sem_A, 0, 1); // El sem A comienza desbloqueado
    sem_init(&sem_B, 0, 0); // El sem B está bloqueado inicialmente
    sem_init(&sem_C, 0, 0); // El sem C está bloqueado inicialmente

    // Creación de los hilos
    pthread_create(&tidA, NULL, threadA, NULL);
    pthread_create(&tidB, NULL, threadB, NULL);
    pthread_create(&tidC, NULL, threadC, NULL);

    // Esperar a que los hilos terminen
    pthread_join(tidA, NULL);
    pthread_join(tidB, NULL);
    pthread_join(tidC, NULL);
    printf("Destruyendo semaforos. \n");

    // Destruir los semáforos
    sem_destroy(&sem_A);
    sem_destroy(&sem_B);
    sem_destroy(&sem_C);

    return 0;
}
