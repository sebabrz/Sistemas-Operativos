#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define IMPRESORAS 2
#define N_USUARIOS 20

pthread_t hilosUsuarios[N_USUARIOS];

sem_t impresorasLibres; //contador: cuantas impresoras estan libres, arranca en 2
sem_t impresora1;       //binario: 1 = la impresora 1 esta libre, 0 = en uso
pthread_mutex_t mutex;  //hace atomica la eleccion de la impresora

int requerir(){
    int id = 0;
    
    sem_wait(&impresorasLibres); //espera hasta que haya una impresora disponible

    pthread_mutex_lock(&mutex);
    if(sem_trywait(&impresora1) == 0){ //si logro tomar la 1, esa es la mia
        id = 1;
    } else {                            //la 1 esta ocupada, entonces la libre es la 2
        id = 2;
    }
    pthread_mutex_unlock(&mutex);

    return id; //identidad de la impresora que me toco (1 o 2)
}

void liberar(int id){
    if(id == 1){
        sem_post(&impresora1);   //la impresora 1 vuelve a estar libre
    }
    sem_post(&impresorasLibres); //aviso que hay una disponible (la 2 solo usa este)
}

void *usuario(void *arg){
    int u = (int)(intptr_t) arg;

    while(1){
        sleep(1); //el usuario hace otras cosas antes de imprimir

        int imp = requerir();
        printf("Usuario %d usa la impresora %d.\n", u, imp);
        sleep(1);                 //imprime
        printf("Usuario %d termina con la impresora %d.\n", u, imp);
        liberar(imp);
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    srand(time(NULL));

    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&impresorasLibres, 0, IMPRESORAS);
    sem_init(&impresora1, 0, 1);
    pthread_mutex_init(&mutex, NULL);

    //------ CREACION DE LOS HILOS -------//
    for(int i = 0; i < N_USUARIOS; i++){
        pthread_create(&hilosUsuarios[i], NULL, *usuario, (void *)(intptr_t)(i+1));
        printf("Se creo correctamente el usuario: %d.\n", i+1);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    for(int i = 0; i < N_USUARIOS; i++){
        pthread_join(hilosUsuarios[i], NULL);
        printf("Se elimino correctamente el usuario: %d.\n", i+1);
    }

    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&impresorasLibres);
    sem_destroy(&impresora1);
    pthread_mutex_destroy(&mutex);

    return 0;
}