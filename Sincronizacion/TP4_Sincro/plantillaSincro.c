#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t nombreHilo[VARIABLE];
pthread_t nombreHilo2;

sem_t nombreSemaforo;
pthread_mutex_t mutex;

void *RutinaNombre1(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);

        sem_wait(&nombreSemaforo);
        sem_post(&nombreSemaforo);

        if(sem_trywait(&nombreSemaforo) == 0){  //Si puedo decremetar el semaforo, devuelve 0, sino -1

        } else{

        }
    }
    return NULL;
}

void *RutinaNombre2(void *arg){
    while(1){
        int numRand = rand() % 3;
    }
    return NULL;
}

int main(){
    sem_init(&nombreSemaforo, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&nombreHilo2, NULL, *RutinaNombre1, NULL);
    printf("Se creo correctamente el hilo.\n");

    for(int i = 0; i < VARIABLE; i++){
        pthread_create(&nombreHilo[i], NULL, *RutinaNombre2, NULL);
        printf("Se creo correctamente el hilo: %d.\n", i+1);
    }

    pthread_join(nombreHilo2, NULL);
    printf("Se elimino correctamente el hilo.\n");

    for(int i = 0; i < VARIABLE; i++){
        pthread_join(nombreHilo[i], NULL);
        printf("Se elimino correctamente el hilo: %d.\n", i+1);
    }

    sem_destroy(&nombreSemaforo);
    pthread_mutex_destroy(&mutex);

    return 0;
}