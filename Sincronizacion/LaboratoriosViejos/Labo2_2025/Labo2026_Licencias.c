#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define VARIABLE 10
#define USUARIOS 30

pthread_t hilosUsuarios[USUARIOS]; //Muchos hilos del mismo tipo

sem_t licencias;  //semaforo normal (contador o binario)
pthread_mutex_t BD; //semaforo de tipo mutex

    /*
    Un centro de computos controla el uso de licencias de un software especializado

    Se disponen un total de 20 licencias
    Cada usuario puede requerir entre 1 y 3 licencias
    Si consigue las licencias requeridas, realiza su trabajo
    Al terminar guarda sus resultados en una base de datos compartida y por ultimo libera las licencias utilizadas
    Si no puede conseguir todas las licencias requeriodas, se retira sin hacer nada 

    */


void *Usuarios(void *arg){  
    int id = (int)(intptr_t) arg;
    int numRand = (rand() % 3) + 1 ; //0 o 1 o 2 //Cada usuario puede requerir entre 1 y 3 licencias
    int obtenidas = 0;

    printf ("Soy el hilo: %d estoy intentando tomar %d \n", id, numRand);
    for (int i=0; i < numRand; i++){
        //SI LOGRO DECREMENTAR LAS LICENCIAS, SIGNIFICA QUE OBTUVE UNA, SINO LOGRO, SIGNICA QUE NO PUEDO
        if (sem_trywait(&licencias) == 0) {
            printf ("Soy el hilo: %d obtuve %d \n", id, i+1);
            obtenidas++;
            
        }   else {
            //SI NO LOGRE DECREMENTAR DEVUELVO LAS QUE TOME
            printf ("Soy el hilo: %d no logre obtener mis licencias devuelvo %d \n", id, i+1);
            for (int i=0; i<obtenidas; i++){
                sem_post(&licencias);
            }
            break;  //corto el for
            }
    }

    if (obtenidas == numRand) {
        //Si pude conseguir las que obtuve, escribo y despues libero
        printf ("Soy el hilo: %d tome %d \n", id, obtenidas);
        pthread_mutex_lock(&BD);
            printf ("Soy el hilo: %d escribiendo en la base de datos \n", id);
        pthread_mutex_unlock(&BD);
        //una vez realizada la escritura, en la BD devuelvo las que obtuve
        for (int i=0; i < obtenidas; i++){
            sem_post(&licencias);
            printf ("Soy el hilo: %d devuelvo mis licencias %d \n", id, i+1);

        }
    }

    return NULL;
}

int main(){
    srand(time(NULL));
    sem_init(&licencias, 0, 20);  //El primer 0 es para algo de permisos, y el segundo 0 es la inicializacion
    pthread_mutex_init(&BD, NULL);
    
    for(int i = 0; i < USUARIOS; i++){
        pthread_create(&hilosUsuarios[i], NULL, *Usuarios, (void *)(intptr_t)(i+1));
    }

    for(int i = 0; i < USUARIOS; i++){
        pthread_join(hilosUsuarios[i], NULL);
    }

    sem_destroy(&licencias);
    pthread_mutex_destroy(&BD);

    return 0;
}