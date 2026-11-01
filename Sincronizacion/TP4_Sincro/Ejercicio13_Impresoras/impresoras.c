#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define USUARIOS 15

/*
Suponga tener n usuarios que comparten 2 impresoras. Antes de utilizar la impresora, el U[i] invoca requerir(impresora). 
Esta operación espera hasta que una de las impresoras esté disponible, retornando la identidad de la impresora libre. 
Después de utilizar la impresora, U[i] invoca a liberar(impresora).
a) Resuelva este problema considerando que cada usuario es un thread y la sincronización se realiza utilizando semáforos.
b) Resuelva este problema considerando que cada usuario tiene una prioridad única. Se otorga la impresora al usuario con mayor prioridad que está esperando.

*/

sem_t impresora1;       // binario: 1 = la impresora 1 esta libre, 0 = en uso
sem_t impresora2;       // binario: 1 = la impresora 2 esta libre, 0 = en uso
sem_t impresorasLibres; // contador en 2, inicialmente hay 2 libres, el usuario se queda esperando alguna libre
pthread_mutex_t mutex;  // hace atomica la eleccion de la impresora

// "usuario" es la identidad del USUARIO (solo se usa para los mensajes).
// La impresora NO se pasa: la elige requerir y la devuelve.
int requerirImpresora(int usuario){
    int impresora = 0; // inicializada: si no, se podria devolver basura

    printf("Usuario %d: esperando alguna impresora\n", usuario);
    sem_wait(&impresorasLibres);
    pthread_mutex_lock(&mutex);

    if(sem_trywait(&impresora1)==0){
        impresora=1;
    }else if(sem_trywait(&impresora2)==0){  
        impresora=2;
    }

    pthread_mutex_unlock(&mutex);
    return impresora;
}

void liberarImpresora(int usuario, int impresora){
    pthread_mutex_lock(&mutex);

    if(impresora==1){
        printf("Usuario %d: se libero impresora %d\n", usuario, impresora);
        sem_post(&impresora1);
        sem_post(&impresorasLibres);
    }else{
        printf("Usuario %d: se libero impresora %d\n", usuario, impresora);
        sem_post(&impresora2);
        sem_post(&impresorasLibres);
    }

    pthread_mutex_unlock(&mutex);

}

void imprimir(int usuario, int impresora){
    printf("[Impresora %d] Usuario %d imprimiendo.\n", impresora, usuario);
    sleep(rand()%4);
    printf("[Impresora %d] Usuario %d termino de imprimir.\n", impresora, usuario);
}


void *user(void *arg){
    int usuario = (int)(intptr_t) arg; // id del usuario (1..USUARIOS)

    while(1){
        int impresora = requerirImpresora(usuario);
        printf("Usuario %d: la impresora requerida es la impresora %d\n", usuario, impresora);
        imprimir(usuario, impresora);
        liberarImpresora(usuario, impresora);
    }
    return NULL;
}


int main(){
    pthread_t usuarios[USUARIOS];
    srand(time(NULL));

    sem_init(&impresora1, 0, 1);
    sem_init(&impresora2, 0, 1);
    sem_init(&impresorasLibres, 0, 2);
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < USUARIOS; i++){
        pthread_create(&usuarios[i], NULL, *user, (void *)(intptr_t)(i+1)); // le paso el id 1..USUARIOS
    }

    for(int i = 0; i < USUARIOS; i++){
        pthread_join(usuarios[i], NULL);
    }

    sem_destroy(&impresora1);
    sem_destroy(&impresora2);
    sem_destroy(&impresorasLibres);
    pthread_mutex_destroy(&mutex);

    return 0;
}