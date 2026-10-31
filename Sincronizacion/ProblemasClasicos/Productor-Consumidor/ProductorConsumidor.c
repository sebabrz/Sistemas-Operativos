/*
    PRODUCTOR-CONSUMIDOR (buffer acotado, Silberschatz 7.1.1, Fig. 7.1 y 7.2)

    Varios productores ponen items en un buffer de N lugares y varios consumidores los sacan.
    No se puede producir si el buffer esta lleno ni consumir si esta vacio.

    Herramientas:
      vacio  contador (N)  lugares libres.  El productor hace wait(vacio) antes de poner
      lleno  contador (0)  items disponibles. El consumidor hace wait(lleno) antes de sacar
      mutex  mutex         protege el buffer y sus indices (varios productores/consumidores a la vez)

    Orden importante: primero el contador (vacio/lleno) y DESPUES el mutex. Al reves, un productor
    con el buffer lleno se quedaria esperando con el mutex tomado y el consumidor que lo vaciaria no podria entrar.

    El buffer vive en el main y se pasa por puntero (no hay variables globales de datos).
    Si el enunciado NO pide datos reales (solo contar lugares), alcanza con vacio y lleno
    y se saca el arreglo, los indices y el mutex.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
#define PRODUCTORES 3
#define CONSUMIDORES 2

typedef struct {
    int datos[N];
    int in;     // proxima posicion donde poner
    int out;    // proxima posicion de donde sacar
} Buffer;

typedef struct {
    int id;
    Buffer *buffer;
} Args;

sem_t vacio;
sem_t lleno;
pthread_mutex_t mutex;

void *productor(void *arg){
    Args *a = (Args *)arg;
    int k = 0;
    while(1){
        int item = a->id * 100 + k++;                 // producir el item: fuera del mutex

        sem_wait(&vacio);                             // hay lugar? si no, espero
        pthread_mutex_lock(&mutex);
            a->buffer->datos[a->buffer->in] = item;
            printf("Productor %d puso %d en la posicion %d\n", a->id, item, a->buffer->in);
            a->buffer->in = (a->buffer->in + 1) % N;
        pthread_mutex_unlock(&mutex);
        sem_post(&lleno);                             // hay un item mas

        sleep(1);
    }
    return NULL;
}

void *consumidor(void *arg){
    Args *a = (Args *)arg;
    while(1){
        sem_wait(&lleno);                             // hay algo? si no, espero
        pthread_mutex_lock(&mutex);
            int item = a->buffer->datos[a->buffer->out];
            printf("Consumidor %d saco %d de la posicion %d\n", a->id, item, a->buffer->out);
            a->buffer->out = (a->buffer->out + 1) % N;
        pthread_mutex_unlock(&mutex);
        sem_post(&vacio);                             // hay un lugar mas

        sleep(2);                                     // consumir el item: fuera del mutex
    }
    return NULL;
}

int main(){
    Buffer buffer = { .in = 0, .out = 0 };
    pthread_t hilosProd[PRODUCTORES], hilosCons[CONSUMIDORES];
    Args argsProd[PRODUCTORES], argsCons[CONSUMIDORES];

    sem_init(&vacio, 0, N);
    sem_init(&lleno, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < PRODUCTORES; i++){
        argsProd[i] = (Args){ .id = i + 1, .buffer = &buffer };
        pthread_create(&hilosProd[i], NULL, productor, &argsProd[i]);
    }
    for(int i = 0; i < CONSUMIDORES; i++){
        argsCons[i] = (Args){ .id = i + 1, .buffer = &buffer };
        pthread_create(&hilosCons[i], NULL, consumidor, &argsCons[i]);
    }

    for(int i = 0; i < PRODUCTORES; i++)  pthread_join(hilosProd[i], NULL);
    for(int i = 0; i < CONSUMIDORES; i++) pthread_join(hilosCons[i], NULL);

    sem_destroy(&vacio);
    sem_destroy(&lleno);
    pthread_mutex_destroy(&mutex);
    return 0;
}