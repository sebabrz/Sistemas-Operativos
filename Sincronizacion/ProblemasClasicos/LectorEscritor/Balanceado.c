/*
    LECTORES-ESCRITORES - "JUSTO" CON SLEEP (como en la veterinaria)

    Varios lectores pueden leer a la vez. El escritor necesita acceso exclusivo.
    Para evitar inanicion de escritores: el ULTIMO lector que se va libera la llave y duerme un rato
    con el mutex de lectores tomado, asi los lectores nuevos no pueden entrar y un escritor se puede
    quedar con la llave. Despues todos descansan afuera antes de volver a pedir.

    Herramientas:
      recurso          binario (1)   llave de la base de datos (primer lector / escritor)
      lectoresAdentro  contador (0)  cuantos lectores hay leyendo
      mutexLectores    mutex         primero/ultimo de los lectores

    Ojo: esta politica depende de los TIEMPOS. Solo funciona si los lectores llegan a quedarse en 0
    (si siempre hay alguno adentro, no existe "el ultimo" y el sleep nunca se ejecuta).
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define LECTORES 5
#define ESCRITORES 2

sem_t recurso;
sem_t lectoresAdentro;
pthread_mutex_t mutexLectores;

void *lector(void *arg){
    int id = *(int *)arg;
    while(1){
        pthread_mutex_lock(&mutexLectores);
            if(sem_trywait(&lectoresAdentro) == 0){
                sem_post(&lectoresAdentro);
            } else {
                sem_wait(&recurso);                   // soy el primero: tomo la llave
            }
            sem_post(&lectoresAdentro);
        pthread_mutex_unlock(&mutexLectores);

        printf("Lector %d empieza a leer\n", id);
        sleep(1);

        pthread_mutex_lock(&mutexLectores);
            printf("Lector %d termina de leer\n", id);
            sem_wait(&lectoresAdentro);
            if(sem_trywait(&lectoresAdentro) == 0){
                sem_post(&lectoresAdentro);
            } else {
                printf("Lector %d es el ultimo y libera la llave\n", id);
                sem_post(&recurso);                   // soy el ultimo: suelto la llave
                sleep(3);                             // doy lugar a que la tome un escritor (evito inanicion de escritores)
            }
        pthread_mutex_unlock(&mutexLectores);

        sleep(2);                                     // descanso: sin esto los lectores vuelven enseguida
    }
    return NULL;
}

void *escritor(void *arg){
    int id = *(int *)arg;
    while(1){
        printf("Escritor %d quiere escribir\n", id);
        sem_wait(&recurso);
        printf("Escritor %d empieza a escribir\n", id);
        sleep(1);
        printf("Escritor %d termina de escribir\n", id);
        sem_post(&recurso);

        sleep(2);                                     // descanso: le da lugar a los lectores
    }
    return NULL;
}

int main(){
    pthread_t hilosLectores[LECTORES], hilosEscritores[ESCRITORES];
    int idsLectores[LECTORES], idsEscritores[ESCRITORES];

    sem_init(&recurso, 0, 1);
    sem_init(&lectoresAdentro, 0, 0);
    pthread_mutex_init(&mutexLectores, NULL);

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
    sem_destroy(&lectoresAdentro);
    pthread_mutex_destroy(&mutexLectores);
    return 0;
}