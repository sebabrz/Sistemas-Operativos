#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <time.h>       // time, medicion de tiempo
#include <stdint.h>     // enteros de tamaño fijo
#include <semaphore.h>

#define MESAS 10
#define PARTICIPANTES 50

/*
3. En una feria de trueque funcionan dos mesas de intercambio, 
cada una con capacidad limitada para almacenar objetos dejados por los participantes. 
Los participantes pueden realizar dos tipos de acciones:

Dejar un objeto en una de las mesas.
Retirar un objeto de una de las mesas.

Cada mesa funciona como un buffer acotado:

No se puede dejar un objeto si la mesa está llena. Considere 10 lugares en cada mesa.
No se puede retirar un objeto si la mesa está vacía.
Solo un participante a la vez puede modificar el contenido de una mesa.

a) Implemente el problema planteado utilizando hilos y las herramientas de sincronización aprendidas en clase.

b) Explique el modelo utilizado y las herramientas escogidas. */

pthread_t partici[PARTICIPANTES];

sem_t mesa1; //contador, con maximo de mesa 1
sem_t vacio1; //contador, con 0
sem_t vacio2; //contador, con 0
sem_t mesa2; //contador, con maximo de mesa 2 
pthread_mutex_t MUTEX1; //mutex para la mesa1 
pthread_mutex_t MUTEX2; //mutex para la mesa2

void dejarObjeto1(int id){
        printf("Soy el participante %d , se pudo dejar objeto en la mesa 1 \n", id);
        sem_post(&mesa1);
}

void dejarObjeto2(int id){
        printf("Soy el participante %d Se pudo dejar objeto en la mesa 2 \n", id);
        sem_post(&mesa2);
}

void retirarObjeto1(int id){
        printf("Soy el participante %d, se pudo retirar objeto en la mesa 1 \n", id);
        sem_post(&vacio1);
}

void retirarObjeto2(int id){
        printf("Soy el participante %d pudo retirar objeto en la mesa 2 \n", id);
        sem_post(&vacio2);
}

void *participante(void *arg){
while(1){
    int id = (int) (intptr_t) arg;

    int mesa = (rand() % 2) + 1; //Aca el participante elije donde accionar en cada a mesa 
    int accion = (rand() % 2) + 1; //retirar o deja?


    if (mesa == 1 ) {
        printf("Soy el participante: %d elije la mesa 1 \n", id);
        if (accion == 1) {
            sem_wait(&mesa1);
            pthread_mutex_lock(&MUTEX1);
            retirarObjeto1(id);
            pthread_mutex_unlock(&MUTEX1);
        } else {
            sem_wait(&vacio1);
            pthread_mutex_lock(&MUTEX1);
            dejarObjeto1(id);
            pthread_mutex_unlock(&MUTEX1);
        }
    } else {
        printf("Soy el participante: %d elije la mesa 2 \n", id);
        
        
        if (accion == 1) {
            sem_wait(&mesa2);
            pthread_mutex_lock(&MUTEX2);
            retirarObjeto2(id);
            pthread_mutex_unlock(&MUTEX2);
        } else {
            sem_wait(&vacio2);
            pthread_mutex_lock(&MUTEX2);
            dejarObjeto2(id);
            pthread_mutex_unlock(&MUTEX2);
        }
    }
    sleep(1);
    }
    return NULL;
}

int main(){

    srand(time(NULL));

    sem_init(&mesa1, 0, 0);
    sem_init(&mesa2, 0, 0);
    sem_init(&vacio1, 0, MESAS);
    sem_init(&vacio2, 0, MESAS);
    pthread_mutex_init(&MUTEX1, NULL);
    pthread_mutex_init(&MUTEX2, NULL);

    for(int i = 0; i < PARTICIPANTES; i++){
        pthread_create(&partici[i], NULL, *participante, (void*)(intptr_t) i);
    } 

    for(int i = 0; i < PARTICIPANTES; i++){
        pthread_join(partici[i], NULL);
    }

    sem_destroy(&mesa1);
    sem_destroy(&mesa2);
    sem_destroy(&vacio1);
    sem_destroy(&vacio2);
    
    pthread_mutex_destroy(&MUTEX1);
    pthread_mutex_destroy(&MUTEX2);

    return 0;
}