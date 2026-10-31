#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define CANT_HOMBRES 10
#define CANT_MUJERES 10
#define ITERACIONES 10

sem_t sem_h;
sem_t sem_m;
sem_t mtx_bano;
sem_t mtx_h;
sem_t mtx_m;
sem_t sem_turno;

// uso o desuso del baño
void rand_time() {
    int time = rand() % 10;
    sleep(time);
}

void * mujer(void * data) {
    int m_id = (int) data;
    for (int i = 0; i < ITERACIONES; i++) {
        rand_time(); // tiempo sin usar baño
        // ----- seccion entrada ------
        sem_wait(&sem_turno);
        sem_wait(&mtx_m);
        if (sem_trywait(&sem_m) == 0) {
            // semaforo disponible => hay mujeres
            printf("Mujer %d PASA a usar baño \n", m_id);
            sem_post(&sem_m);
        } else {
            // semaforo no disponible => no hay mujeres
            printf("Mujer %d INTENTA usar baño \n", m_id);
            sem_wait(&mtx_bano); // intento ocupar el baño
            printf("Mujeres %d OCUPAN el baño \n", m_id);
        }
        sem_post(&sem_m); // hay mujeres en el baño
        sem_post(&sem_turno);
        sem_post(&mtx_m);

        // ----- seccion critica ------
        // mujeres usan el baño
        printf("Mujer %d USA el baño\n", m_id);
        rand_time(); // tiempo de uso del baño

        // ----- seccion salida ------
        sem_wait(&mtx_m);
        //mujer sale del baño
        sem_wait(&sem_m);
        printf("Mujer %d SALE del baño\n", m_id);
        if (sem_trywait(&sem_m) == 0) {
            // semaforo disponible => no es la ultima mujer
            sem_post(&sem_m);
        } else {
            // semaforo no disponible => es la ultima mujer
            printf("Mujer %d DESOCUPAN el baño\n", m_id);
            sem_post(&mtx_bano); // desocupo el baño
        }
        sem_post(&mtx_m);
    }
    printf("MUJERES TERMINARON \n");
    pthread_exit(NULL);
}

void * hombre(void * data) {
    int h_id = (int) data;
    for (int i = 0; i < ITERACIONES; i++) {
        rand_time(); // tiempo sin usar el baño
        // ----- seccion entrada ------
        sem_wait(&sem_turno);
        sem_wait(&mtx_h);
        if (sem_trywait(&sem_h) == 0) {
            // semaforo disponible => hay hombres
            printf("Hombre %d PASA a usar baño\n", h_id);
            sem_post(&sem_h);
        } else {
            // semaforo no disponible => no hay hombres
            printf("Hombre %d INTENTA usar baño\n", h_id);
            sem_wait(&mtx_bano); // intento ocupar el baño
            printf("Hombres %d OCUPAN el baño\n", h_id);
        }
        sem_post(&sem_h); // hay hombres en el baño
        sem_post(&sem_turno);
        sem_post(&mtx_h);

        // ----- seccion critica ------
        // hombres usan el baño
        printf("Hombre %d USA el baño\n", h_id);
        rand_time(); // tiempo de uso del baño

        // ----- seccion salida ------
        sem_wait(&mtx_h);
        //hombre sale del baño
        sem_wait(&sem_h);
        printf("Hombre %d SALE del baño\n", h_id);
        if (sem_trywait(&sem_h) == 0) {
            // semaforo disponible => no es el ultimo hombre
            sem_post(&sem_h);
        } else {
            // semaforo no disponible => es el ultimo hombre
            printf("Hombre %d DESOCUPAN el baño\n", h_id);
            sem_post(&mtx_bano); // desocupo el baño
        }
        sem_post(&mtx_h);
    }
    printf("HOMBRES TERMINARON\n");
    pthread_exit(NULL);
}

void main() {
    sem_init(&sem_h, 0, 0);
    sem_init(&sem_m, 0, 0);
    sem_init(&mtx_bano, 0, 1);
    sem_init(&mtx_h, 0, 1);
    sem_init(&mtx_m, 0, 1);
    sem_init(&sem_turno, 0, 1);

    pthread_t hombres[CANT_HOMBRES];
    pthread_t mujeres[CANT_MUJERES];

    for (int i = 0; i < CANT_HOMBRES; i++) {
        pthread_create(&hombres[i], NULL, hombre, (void *) i);
    }

    for (int i = 0; i < CANT_HOMBRES; i++) {
        pthread_create(&mujeres[i], NULL, mujer, (void *) i);
    }

    for (int i = 0; i < CANT_HOMBRES; i++) {
        pthread_join(hombres[i], NULL);
    }

    for (int i = 0; i < CANT_MUJERES; i++) {
        pthread_join(mujeres[i], NULL);
    }

    sem_destroy(&sem_h);
    sem_destroy(&sem_m);
}