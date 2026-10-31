#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t semA, semB,semC;
//Sec AB AB AB AC
//    AB AB AB AC
void* taskA(){
    while(1){
        sem_wait(&semA);
        printf("A");
        sem_post(&semC);
    }
}

void* taskB(){
    while(1){
        sem_wait(&semB);
        printf("B");
        sem_post(&semA);
    }
}
void* taskC(){
    while(1){
        sem_wait(&semC);
        sem_post(&semB);
        sem_wait(&semC);
        sem_post(&semB);
        sem_wait(&semC);
        sem_post(&semB);

        sem_wait(&semC);
        printf("C\n");
        sem_post(&semA);
    }
}

int main(){
    pthread_t tasks[3];
    sem_init(&semA, 0,1);
    sem_init(&semB, 0,0);
    sem_init(&semC, 0,0);
    
    pthread_create(&tasks[0],NULL,&taskA,NULL);
    pthread_create(&tasks[1],NULL,&taskB,NULL);
    pthread_create(&tasks[2],NULL,&taskC,NULL);
    
    pthread_join(tasks[0],NULL);
    pthread_join(tasks[1],NULL);
    pthread_join(tasks[2],NULL);
    
    sem_destroy(&semA);
    sem_destroy(&semB);
    sem_destroy(&semC);
    return 0;
}
