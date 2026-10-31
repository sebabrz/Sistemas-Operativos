#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h> 
#define CANT_LECTORES 5
#define CANT_ESCRITORES 5

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define ORANGE "\033[38;5;214m"

sem_t cantLec, turno, recurso;
pthread_mutex_t mutexL = PTHREAD_MUTEX_INITIALIZER;  

void * reader(void * args){
    
    int id = (int) args;
    while(1){
        sem_wait(&turno);

        pthread_mutex_lock(&mutexL);
        if(sem_trywait(&cantLec) == 0){ 
            sem_post(&cantLec);
            pthread_mutex_unlock(&mutexL);
        }
        else{
            pthread_mutex_unlock(&mutexL);
            sem_wait(&recurso);
        }   
        sem_post(&cantLec);
        
        sem_post(&turno);
        printf(GREEN "Lector %d leyendo \n" RESET, id);
        fflush(stdout);
        sleep(1);

        pthread_mutex_lock(&mutexL);
        sem_wait(&cantLec);
        printf(RED "Lector %d se va \n" RESET, id);
        fflush(stdout);
        if(sem_trywait(&cantLec)== 0){
            sem_post(&cantLec);
        }
        else{
            sem_post(&recurso);
        }

        pthread_mutex_unlock(&mutexL);
        sleep(2);
    }
    pthread_exit(0);
}

void * writer(void * args){
    
    int id = (int) args;
    while(1){
        sem_wait(&turno);
        sem_wait(&recurso);
        sem_post(&turno);
        printf(ORANGE "Escritor %d escribiendo \n" RESET, id);
        fflush(stdout);
        sleep(1);
        printf(RED "Escritor %d se va \n" RESET, id);
        sem_post(&recurso);

        sleep(2);
    }
    pthread_exit(0);
}
int main(){
    sem_init(&cantLec,0,0);
    sem_init(&turno,0,1);
    sem_init(&recurso,0,1);

    pthread_t lectores[CANT_LECTORES];
    for (int r = 0; r < CANT_LECTORES; r++){
        pthread_create(&lectores[r], NULL, reader, (void *) r+1);
    } 
    
    pthread_t escritores[CANT_ESCRITORES];
    for (int e = 0; e < 5; e++){
        pthread_create(&escritores[e], NULL, writer, (void *) e+1);
    }

    for (int r = 0; r < CANT_LECTORES; r++){
        pthread_join(lectores[r], NULL);
    }
    for (int e = 0; e <CANT_ESCRITORES; e++){
        pthread_join(escritores[e], NULL);
    }

    sem_destroy(&turno);
    sem_destroy(&cantLec);
    sem_destroy(&recurso);
    pthread_mutex_destroy(&mutexL);

    return 0;
}
