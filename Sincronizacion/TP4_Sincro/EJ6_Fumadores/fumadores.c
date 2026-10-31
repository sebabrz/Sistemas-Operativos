#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <stdint.h>     // enteros de tamaño fijo
#include <string.h>     // manejo de strings
#include <semaphore.h>
#include <time.h>

sem_t agente, tabaco, papel, fosforo;
pthread_mutex_t mesa;


void* fumt(){
    while(1){
        pthread_mutex_lock(&mesa);
        if(sem_trywait(&papel)==0){
            if(sem_trywait(&fosforo)==0){
                printf("Fumador tabaco toma papel y fosforos y fuma...\n");
                sem_post(&agente);
            }else{
                sem_post(&papel);
                printf("Fumador tabaco devuelve el papel...\n");
            }
        }
        pthread_mutex_unlock(&mesa);
        sleep(2);
    }
}
void* fump(){
    while(1){
        pthread_mutex_lock(&mesa);
        if(sem_trywait(&tabaco)==0){
            if(sem_trywait(&fosforo)==0){
                printf("Fumador papel toma tabaco y fosforos y fuma...\n");
                sem_post(&agente);
            }else{
                sem_post(&tabaco);
                printf("Fumador papel devuelve los fosforos...\n");
            }
        }
        pthread_mutex_unlock(&mesa);
        sleep(2);
    }
}
void* fumf(){
    while(1){
       pthread_mutex_lock(&mesa);
       if(sem_trywait(&tabaco)==0){
        if(sem_trywait(&papel)==0){
            printf("Fumador fosforo toma tabaco y papel y fuma...\n");
            sem_post(&agente);
        }else{     
            sem_post(&tabaco);
            printf("Fumador fosforos devuelve el tabaco...\n");
        }
       }
       pthread_mutex_unlock(&mesa);
       sleep(2);
    }
}
void* ag(){
    int r;
    while(1){
        sem_wait(&agente);
        
        r= rand() % 3;
        switch (r) {
      case 0:                    // pone papel y fósforos
        sem_post(&papel);
        sem_post(&fosforo);
        printf("Agente pone papel y fosforos en la mesa\n");
        break;
      case 1:                    // pone tabaco y fósforos
        sem_post(&tabaco);
        sem_post(&fosforo);
        printf("Agente pone tabaco y fosforos en la mesa\n");
        break;
      case 2:                    // pone tabaco y papel
        sem_post(&tabaco);
        sem_post(&papel);
        printf("Agente pone tabaco y papel en la mesa\n");
        break;
    }
    }
}

int main() {
    sem_init(&agente,0,1);
    sem_init(&tabaco,0,0);
    sem_init(&papel,0,0);
    sem_init(&fosforo,0,0);
    pthread_mutex_init(&mesa, NULL);
    srand(time(NULL));
    pthread_t fumadorTabaco, fumadorPapel, fumadorFosforos, agent;
  
    
    pthread_create(&agent, NULL, &ag, NULL);
    pthread_create(&fumadorTabaco, NULL, &fumt, NULL);
    pthread_create(&fumadorPapel, NULL, &fump, NULL);
    pthread_create(&fumadorFosforos, NULL, &fumf, NULL);

    pthread_join(agent,NULL);
    pthread_join(fumadorTabaco,NULL);
    pthread_join(fumadorPapel,NULL);
    pthread_join(fumadorFosforos,NULL);
    
    sem_close(&agente);
	sem_close(&tabaco);
	sem_close(&papel);
	sem_close(&fosforo);
	return 0;
}