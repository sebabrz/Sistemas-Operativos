#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define USUARIOS 15

/*
Suponga tener n usuarios que comparten 2 impresoras. Antes de utilizar la impresora, el U[i] invoca requerir(impresora). 
Esta operación espera hasta que una de las impresoras esté disponible, retornando la identidad de la impresora libre. 
Después de utilizar la impresora, U[i] invoca a liberar(impresora).
a) Resuelva este problema considerando que cada usuario es un thread y la sincronización se realiza utilizando semáforos.
b) Resuelva este problema considerando que cada usuario tiene una prioridad única. Se otorga la impresora al usuario con mayor prioridad que está esperando.

*/

pthread_t usuarios[USUARIOS];
int *numeroImpresora[NUMERO]= { 1, 2};

sem_t impresoras[NUMERO]; 
sem_t impresora1;
sem_t impresora2;
sem_t impresorasLibres; // contador en 2, inicialmente hay 2 libres, el usuario se queda esperando alguna libre
pthread_mutex_t mutex;

int requerirImpresora(int nro){
    sem_wait(&impresorasLibres);
    pthread_mutex_lock(mutex);

    if(sem_trywait(&impresora1)==0){
        nro=1;
    }else if(sem_trywait(&impresora2)==0){  
        nro=2;
    }


    pthread_mutex_unlock(&mutex);
    return nro;
}

void liberarImpresora(){
    pthread_mutex_lock(mutex);

    if(nro==1){
        printf("\n");
        sem_post(&impresora1);
        sem_post(&impresorasLibres);
    }


    pthread_mutex_unlock(&mutex);

}


void *user(void *arg){
    while(1){
        int nro=0;
        nro=requerir(nro);
        printf("La impresora requerida es la impresora%d\n", nro);
        sleep(1);
        liberarImpresora(nro);
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