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
b) Resuelva este problema considerando que cada usuario tiene una prioridad única. Se otorga la impresora al usuario con mayor prioridad que está esperando.

Idea:
 - Prioridad unica: el usuario i tiene prioridad i (MAYOR numero = MAYOR prioridad).
 - Un semaforo contador comun no sirve: despierta a cualquiera, no al de mayor prioridad.
   Por eso cada usuario duerme en SU PROPIO semaforo (turno[prioridad]) y el que libera
   elige a quien despertar.
 - Para que el que libera sepa quienes esperan, cada usuario que espera se anota en
   quiere[prioridad] (binario). El que libera recorre de la mayor prioridad a la menor.
 - Si al liberar hay alguien esperando, la impresora se le ENTREGA directamente (no se
   suma a impresorasLibres): asi nadie que recien llega se la puede colar.
*/

sem_t impresora1;          // binario: 1 = la impresora 1 esta libre, 0 = en uso
sem_t impresora2;          // binario: 1 = la impresora 2 esta libre, 0 = en uso
sem_t impresorasLibres;    // contador: impresoras libres QUE NO TIENEN DUENO (nadie esperando), arranca en 2
sem_t quiere[USUARIOS+1];  // binario por prioridad: 1 = el usuario con esa prioridad esta esperando, arranca en 0
sem_t turno[USUARIOS+1];   // aca duerme cada usuario hasta que le toque, arranca en 0 (indice 0 sin usar)
pthread_mutex_t mutex;     // hace atomica la eleccion de la impresora y la eleccion del que espera

// busca una impresora libre y la toma. Se llama SIEMPRE con el mutex tomado.
int elegirImpresora(){
    int impresora = 0; // inicializada: si no, se podria devolver basura

    if(sem_trywait(&impresora1)==0){
        impresora=1;
    }else if(sem_trywait(&impresora2)==0){  
        impresora=2;
    }
    return impresora;
}

// "prioridad" es la prioridad del usuario (aca SI hace falta, y no es la impresora: la sigue eligiendo requerir)
int requerirImpresora(int prioridad){
    int impresora = 0;

    pthread_mutex_lock(&mutex);
    if(sem_trywait(&impresorasLibres)==0){ // hay una libre y nadie esperando (si hubiera, no estaria en impresorasLibres)
        impresora = elegirImpresora();
        pthread_mutex_unlock(&mutex);
    }else{                                 // no hay ninguna libre: me anoto y espero mi turno
        printf("Usuario %d: esperando alguna impresora\n", prioridad);
        sem_post(&quiere[prioridad]);      // me anoto como esperando (con el mutex tomado)
        pthread_mutex_unlock(&mutex);      // suelto el mutex ANTES de dormirme (si no, nadie podria liberar)
        sem_wait(&turno[prioridad]);       // duermo hasta que el que libera me elija a mi
        pthread_mutex_lock(&mutex);
        impresora = elegirImpresora();     // la impresora ya quedo libre y reservada para mi
        pthread_mutex_unlock(&mutex);
    }
    return impresora;
}

void liberarImpresora(int prioridad, int impresora){
    int elegido = 0; // prioridad del usuario que recibe la impresora (0 = nadie esperaba)

    pthread_mutex_lock(&mutex);
    printf("Usuario %d: se libero impresora %d\n", prioridad, impresora);

    if(impresora==1){
        sem_post(&impresora1);             // primero la marco libre...
    }else{
        sem_post(&impresora2);
    }

    for(int p = USUARIOS; p >= 1 && elegido == 0; p--){ // de la mayor prioridad a la menor
        if(sem_trywait(&quiere[p])==0){    // el primero que encuentro esperando es el de mayor prioridad
            elegido = p;
            sem_post(&turno[p]);           // ...y lo despierto: la impresora es para el
        }
    }

    if(elegido == 0){
        sem_post(&impresorasLibres);       // nadie esperaba: queda libre para el que llegue
    }else{
        printf("Usuario %d: la impresora %d es para el usuario %d\n", prioridad, impresora, elegido);
    }
    pthread_mutex_unlock(&mutex);
}

void imprimir(int usuario, int impresora){
    printf("[Impresora %d] Usuario %d imprimiendo.\n", impresora, usuario);
    sleep(rand()%4);
    printf("[Impresora %d] Usuario %d termino de imprimir.\n", impresora, usuario);
}


void *user(void *arg){
    int prioridad = (int)(intptr_t) arg; // el usuario i tiene prioridad i (1..USUARIOS)

    while(1){
        int impresora = requerirImpresora(prioridad);
        printf("Usuario %d: la impresora requerida es la impresora %d\n", prioridad, impresora);
        imprimir(prioridad, impresora);
        liberarImpresora(prioridad, impresora);
    }
    return NULL;
}


int main(){
    pthread_t usuarios[USUARIOS];
    srand(time(NULL));

    sem_init(&impresora1, 0, 1);
    sem_init(&impresora2, 0, 1);
    sem_init(&impresorasLibres, 0, 2);
    for(int i = 0; i <= USUARIOS; i++){
        sem_init(&quiere[i], 0, 0);
        sem_init(&turno[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < USUARIOS; i++){
        pthread_create(&usuarios[i], NULL, *user, (void *)(intptr_t)(i+1)); // prioridad = i+1
    }

    for(int i = 0; i < USUARIOS; i++){
        pthread_join(usuarios[i], NULL);
    }

    sem_destroy(&impresora1);
    sem_destroy(&impresora2);
    sem_destroy(&impresorasLibres);
    for(int i = 0; i <= USUARIOS; i++){
        sem_destroy(&quiere[i]);
        sem_destroy(&turno[i]);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}