#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <stdint.h>     // enteros de tamaño fijo
#include <string.h>     // manejo de strings
#include <semaphore.h>  // semaforos
#include <time.h>       // time

sem_t vacio;
sem_t comida;
sem_t terminaPapa;
pthread_mutex_t nido;

void *rutina_pichon(){
    while(1){
        printf("Un pichon se fija si tiene para comer\n");

        pthread_mutex_lock(&nido);
        sem_wait(&comida);
        printf("Pichon come porcion\n");

        if(sem_trywait(&comida)==0){
            sem_post(&comida);
        }else{
            printf("El ultimo pichon que comio avisa que no hay mas comida\n");
            sem_post(&vacio);
            sem_wait(&terminaPapa);
        }
        pthread_mutex_unlock(&nido);

        sleep(2);
    }
}

void *rutina_papa(void *arg){
    int M=*(int *)arg;
    while(1){
        printf("El papa espera que le avisen que no hay comida\n");
        sem_wait(&vacio);
        printf("El papa recibe el aviso de que no hay mas comida\n");

        for(int i=0;i<M;i++){
            sem_post(&comida);
        }
        printf("El papa avisa que ya repuso la comida\n");
        sem_post(&terminaPapa);//post terminooso
        sleep(2);
    }
}


int main() {
    int M=0;
    int cantidadPichones=0;
    while(M<=0){
        printf("Indique cantidad de porciones\n");
        scanf("%d", &M);
    }
    while(cantidadPichones<=0){
        printf("Indique cantidad de pichones\n");
        scanf("%d", &cantidadPichones);
    }
    sem_init(&vacio,0,0);//semaforo binario para avsiar si esta vacio
    sem_init(&comida,0,M);//contador para consumir comida
    sem_init(&terminaPapa,0,0); //para saber cuando el papa termino de reponer comida
    pthread_mutex_init(&nido,NULL);
    srand(time(NULL));
    pthread_t papa;
    pthread_t pichones[cantidadPichones];
    

    pthread_create(&papa, NULL, &rutina_papa, &M);
    for(int i=0; i<cantidadPichones; i++){
        pthread_create(&pichones[i], NULL, &rutina_pichon, NULL);
    }

    pthread_join(papa,NULL);
    for(int i=0; i<cantidadPichones; i++){
        pthread_join(pichones[i], NULL);
    }

    sem_destroy(&vacio);
    sem_destroy(&lleno);
    sem_destroy(&terminaPapa);
    pthread_mutex_destroy(&nido);
    return 0;
}