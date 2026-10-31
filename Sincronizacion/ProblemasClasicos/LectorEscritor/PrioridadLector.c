/*
    LECTORES-ESCRITORES - PRIORIDAD A LOS LECTORES (1er problema, Silberschatz 7.1.2)

    Varios lectores pueden leer a la vez. El escritor necesita acceso exclusivo
    (ni lectores ni otro escritor mientras escribe).
    Convencion: ningun lector espera a menos que un escritor YA tenga permiso para escribir.

    Herramientas:
      recurso          binario (1)   llave de la base de datos. La toma el PRIMER lector y la suelta el ULTIMO
                                     (por eso es semaforo y no mutex: la suelta otro hilo). El escritor la toma y la suelta el mismo.
      lectoresAdentro  contador (0)  cuantos lectores hay leyendo (sin variable global)
      mutexLectores    mutex         protege el wait+trywait que decide si soy primero o ultimo

    Ojo: puede haber INANICION de escritores (si siempre hay algun lector adentro, la llave nunca se libera).
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
            if(sem_trywait(&lectoresAdentro) == 0){   // hay lectores adentro: no soy el primero
                sem_post(&lectoresAdentro);
            } else {                                  // soy el primero: tomo la llave (si hay un escritor, espero aca)
                sem_wait(&recurso);
            }
            sem_post(&lectoresAdentro);               // me cuento como un lector mas
        pthread_mutex_unlock(&mutexLectores);

        printf("Lector %d empieza a leer\n", id);
        sleep(1);                                     // leer: FUERA del mutex, asi leen en paralelo

        pthread_mutex_lock(&mutexLectores);
            printf("Lector %d termina de leer\n", id);
            sem_wait(&lectoresAdentro);               // me saco de la cuenta
            if(sem_trywait(&lectoresAdentro) == 0){   // quedan lectores: no soy el ultimo
                sem_post(&lectoresAdentro);
            } else {                                  // soy el ultimo: suelto la llave
                sem_post(&recurso);
            }
        pthread_mutex_unlock(&mutexLectores);

        sleep(2);                                     // descanso
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

        sleep(2);
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

    for(int i = 0; i < LECTORES; i++)   pthread_join(hilosLectores[i], NULL);     // nunca terminan
    for(int i = 0; i < ESCRITORES; i++) pthread_join(hilosEscritores[i], NULL);

    sem_destroy(&recurso);
    sem_destroy(&lectoresAdentro);
    pthread_mutex_destroy(&mutexLectores);
    return 0;
}