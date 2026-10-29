#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAXUSERS 10

sem_t impresora1;
sem_t impresora2;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int requerir(int id){
    while(1){
        pthread_mutex_lock(&mutex);
        if(sem_trywait(&impresora1) == 0){
            printf("Impresora 1 requerida por usuario %d \n", id);
            pthread_mutex_unlock(&mutex);
            return 1;
        }
        else if(sem_trywait(&impresora2) == 0){
            printf("Impresora 2 requerida por usuario %d \n", id);
            pthread_mutex_unlock(&mutex);
            return 2;
        }
        pthread_mutex_unlock(&mutex);
        
    }
}

void liberar(int impresora){//desbloqueo la impresora
    if(impresora==1){
        sem_post(&impresora1);
    }
    if(impresora==2){
        sem_post(&impresora2);
    }
}

void imprimir(int impresora){
    printf("[Impresora %d] Imprimiendo.\n",impresora);
    sleep(rand()%4);
    printf("[Impresora %d] Termine de imprimir.\n",impresora);
}

void *usuario(void *args){
    int id = (int) args;
    int n =requerir(id);// espero por impresora
    imprimir(n);//imprimo
    liberar(n);//desbloqueo la impresora
    pthread_exit(EXIT_SUCCESS);
}

int main(){
    pthread_t threads[MAXUSERS];

    sem_init(&impresora1,0,1);//semaforo de la impresora 1
    sem_init(&impresora2,0,1);//semaforo de la impresora 2

    int i;
    for(i=0;i<MAXUSERS;i++){
        pthread_create(&threads[i],NULL,usuario,(void*)i);
    }
    for (i = 0; i < MAXUSERS; i++) {
		pthread_join(threads[i], NULL);
    }
    sem_destroy(&impresora1);
    sem_destroy(&impresora2);
    pthread_mutex_destroy(&mutex);
    return 0;
}
