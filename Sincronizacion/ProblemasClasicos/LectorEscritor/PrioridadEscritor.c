/*
    LECTORES-ESCRITORES - PRIORIDAD A LOS ESCRITORES (2do problema, Silberschatz 7.1.2)

    Varios lectores pueden leer a la vez. El escritor necesita acceso exclusivo.
    Convencion: si hay un escritor esperando, NO empieza ningun lector nuevo.

    Es el mismo esquema que el de lectores, mas una PUERTA que cierran los escritores:
    el primer escritor que quiere escribir la cierra y el ultimo la abre.

    Herramientas:
      recurso           binario (1)   llave de la base de datos (primer lector / escritor la toman)
      puertaLectores    binario (1)   puerta de entrada de los lectores. La cierra el primer escritor y la abre el ultimo
      lectoresAdentro   contador (0)  cuantos lectores hay leyendo
      escritoresQuieren contador (0)  cuantos escritores hay queriendo escribir (esperando o escribiendo)
      mutexLectores     mutex         primero/ultimo de los lectores
      mutexEscritores   mutex         primero/ultimo de los escritores
      mutexEntrada      mutex         deja pasar de a UN lector por la puerta: sin el, varios lectores
                                      se juntan esperando la puerta y compiten con el escritor por la llave

    Ojo: puede haber INANICION de lectores (si siempre hay un escritor queriendo, la puerta no se abre).
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define LECTORES 5
#define ESCRITORES 2

sem_t recurso;
sem_t puertaLectores;
sem_t lectoresAdentro;
sem_t escritoresQuieren;
pthread_mutex_t mutexLectores;
pthread_mutex_t mutexEscritores;
pthread_mutex_t mutexEntrada;

void *lector(void *arg){
    int id = *(int *)arg;
    while(1){
        pthread_mutex_lock(&mutexEntrada);            // de a un lector por la puerta
            sem_wait(&puertaLectores);                // si un escritor la cerro, espero aca
                pthread_mutex_lock(&mutexLectores);
                    if(sem_trywait(&lectoresAdentro) == 0){
                        sem_post(&lectoresAdentro);
                    } else {
                        sem_wait(&recurso);           // soy el primero: tomo la llave
                    }
                    sem_post(&lectoresAdentro);
                pthread_mutex_unlock(&mutexLectores);
            sem_post(&puertaLectores);
        pthread_mutex_unlock(&mutexEntrada);

        printf("Lector %d empieza a leer\n", id);
        sleep(1);

        pthread_mutex_lock(&mutexLectores);
            printf("Lector %d termina de leer\n", id);
            sem_wait(&lectoresAdentro);
            if(sem_trywait(&lectoresAdentro) == 0){
                sem_post(&lectoresAdentro);
            } else {
                sem_post(&recurso);                   // soy el ultimo: suelto la llave
            }
        pthread_mutex_unlock(&mutexLectores);

        sleep(2);
    }
    return NULL;
}

void *escritor(void *arg){
    int id = *(int *)arg;
    while(1){
        printf("Escritor %d quiere escribir\n", id);
        pthread_mutex_lock(&mutexEscritores);
            if(sem_trywait(&escritoresQuieren) == 0){ // ya hay escritores: la puerta ya esta cerrada
                sem_post(&escritoresQuieren);
            } else {
                sem_wait(&puertaLectores);            // soy el primero: cierro la puerta
            }
            sem_post(&escritoresQuieren);
        pthread_mutex_unlock(&mutexEscritores);

        sem_wait(&recurso);                           // espero a que salgan los lectores que ya estaban adentro
        printf("Escritor %d empieza a escribir\n", id);
        sleep(1);
        printf("Escritor %d termina de escribir\n", id);
        sem_post(&recurso);

        pthread_mutex_lock(&mutexEscritores);
            sem_wait(&escritoresQuieren);
            if(sem_trywait(&escritoresQuieren) == 0){
                sem_post(&escritoresQuieren);         // quedan escritores: la puerta sigue cerrada
            } else {
                sem_post(&puertaLectores);            // soy el ultimo: abro la puerta
            }
        pthread_mutex_unlock(&mutexEscritores);

        sleep(2);
    }
    return NULL;
}

int main(){
    pthread_t hilosLectores[LECTORES], hilosEscritores[ESCRITORES];
    int idsLectores[LECTORES], idsEscritores[ESCRITORES];

    sem_init(&recurso, 0, 1);
    sem_init(&puertaLectores, 0, 1);
    sem_init(&lectoresAdentro, 0, 0);
    sem_init(&escritoresQuieren, 0, 0);
    pthread_mutex_init(&mutexLectores, NULL);
    pthread_mutex_init(&mutexEscritores, NULL);
    pthread_mutex_init(&mutexEntrada, NULL);

    for(int i = 0; i < LECTORES; i++){
        idsLectores[i] = i + 1;
        pthread_create(&hilosLectores[i], NULL, lector, &idsLectores[i]);
    }
    for(int i = 0; i < ESCRITORES; i++){
        idsEscritores[i] = i + 1;
        pthread_create(&hilosEscritores[i], NULL, escritor, &idsEscritores[i]);
    }

    for(int i = 0; i < LECTORES; i++)   pthread_join(hilosLectores[i], NULL);
    for(int i = 0; i < ESCRITORES; i++) pthread_join(hilosEscritores[i], NULL);

    sem_destroy(&recurso);
    sem_destroy(&puertaLectores);
    sem_destroy(&lectoresAdentro);
    sem_destroy(&escritoresQuieren);
    pthread_mutex_destroy(&mutexLectores);
    pthread_mutex_destroy(&mutexEscritores);
    pthread_mutex_destroy(&mutexEntrada);
    return 0;
}