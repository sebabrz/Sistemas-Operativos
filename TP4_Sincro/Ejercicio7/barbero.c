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
#define CLIENTES 15

sem_t sillas;
sem_t barbero;
sem_t clientes;
pthread_mutex_t mutex;

void *barbudo(){
    while(1){
        sem_wait(&clientes);//si no hay clientes espera aca dormido
        sem_post(&barbero);
        printf("El barbero corta el pelo al cliente \n");
    }
}

void *cliente(void *argv){
    int id=(int)(intptr_t)argv;
   // pthread_mutex_lock(&mutex);
    if(sem_trywait(&sillas)==0){
        printf("Entro el cliente %d a la sala\n", id);       
        sem_post(&clientes);//si el barbero esta dormido lo despierta, sino solo se sienta
       // pthread_mutex_unlock(&mutex);
        printf("El cliente %d se sienta a esperar\n", id);  
        sem_wait(&barbero);
        sem_post(&sillas);      
        printf("El cliente %d se cortó el pelo y se va\n", id);
    }else{
        pthread_mutex_unlock(&mutex);
        printf("El cliente %d se va porque no habia sillas...\n", id);
        sleep(2);
    }
}

int main(){
    int n=0;
    while(n<=0){
        printf("Cantidad de sillas :\n");
        scanf("%d", &n);
    }
    sem_init(&barbero,0,0);
    sem_init(&clientes,0,0);
    sem_init(&sillas,0,n);
    pthread_t clientes_tid[CLIENTES];
    pthread_t barby;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&barby, NULL, &barbudo, NULL);
   for (int i = 0; i < CLIENTES; i++) {
         pthread_create(&clientes_tid[i], NULL, &cliente, (void *)(intptr_t) i);
    }

    for (int i = 0; i < CLIENTES; i++) {
        pthread_join(clientes_tid[i], NULL);
    }
    pthread_cancel(barby);
    pthread_join(barby,NULL);

    sem_close(&barbero);
	sem_close(&sillas);
	sem_close(&clientes);
    pthread_mutex_destroy(&mutex);

    return 0;
}
//hace falta mutex?
//esta bien que la cantidad de sillas no incluya la del barbero?
//ver salida de los printf