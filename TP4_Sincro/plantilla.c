
//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define VARIABLE 10

//--------------------------- DECLARACION DE LOS HILOS ----------------------------//
pthread_t nombreHilo[VARIABLE]; //Muchos hilos del mismo tipo
pthread_t nombreHilo2;          //Un unico hilo

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t nombreSemaforo;  //semaforo normal (contador o binario)
pthread_mutex_t mutex; //semaforo de tipo mutex

//----------------------------------- RUTINAS -------------------------------------//
void *RutinaNombre1(void *arg){ 
    while(1){
        //Implemento lo que tiene que hacer cada hilos que le toque esta rutina

        //COMO USAR ALGUNAS COSAS:
        pthread_mutex_lock(&mutex);   //Pedir el mutex
        pthread_mutex_unlock(&mutex); //Liberar el mutex

        sem_wait(&nombreSemaforo);  //Hacerle wait() al semaforo
        sem_post(&nombreSemaforo);  //Hacerle signal() al semaforo

        if(sem_trywait(&nombreSemaforo) == 0){  //Si logro decrementar el semaforo
            //Hago tal cosa
        } else{//Sino
            //Hago otra cosa
        }
    }
    return NULL;
}

void *RutinaNombre2(void *arg){ 
    while(1){
        //Implemento lo que tiene que hacer cada hilos que le toque esta rutina

        //Numero random
        int numRand = rand() % 3; //0 o 1 o 2
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&nombreSemaforo, 0, 0);  //El primer 0 es para algo de permisos, y el segundo 0 es la inicializacion
    pthread_mutex_init(&mutex, NULL);

    //------ CREACION DE LOS HILOS -------//
    pthread_create(&nombreHilo2, NULL, *RutinaNombre1, NULL);
    printf("Se creo correctamente el hilo.\n");
    
    for(int i = 0; i < VARIABLE; i++){
        pthread_create(&nombreHilo[i], NULL, *RutinaNombre2, NULL);
        printf("Se creo correctamente el hilo: %d.\n", i+1);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    pthread_join(nombreHilo2, NULL);
    printf("Se elimino correctamente el hilo.\n");

    for(int i = 0; i < VARIABLE; i++){
        pthread_join(nombreHilo[i], NULL);
        printf("Se elimino correctamente el hilo: %d.\n", i+1);
    }
    
    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&nombreSemaforo);
    pthread_mutex_destroy(&mutex);

    return 0;
}