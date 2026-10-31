//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define ASIENTOS 5
#define N_CLIENTES 12

//--------------------------- DECLARACION DE LOS HILOS ----------------------------//
pthread_t hilosClientes[N_CLIENTES];

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t asientosLibres; //contador: asientos libres, arranca en 5
sem_t comiendo;       //contador: cuantos estan comiendo, arranca en 0
sem_t esperando;      //contador: cuantos esperan que se vaya el grupo, arranca en 0
sem_t cerrado;        //binario: 1 = los 5 asientos se llenaron juntos, 0 = abierto
sem_t bloqueo;        //aca duermen los que esperan al grupo, arranca en 0
pthread_mutex_t mutex; //protege el estado del bar

//----------------------------------- RUTINAS -------------------------------------//
void *cliente(void *arg){
    int id = (int)(intptr_t)arg;

    while(1){
        usleep(rand() % 1000000); //llega al bar en cualquier momento

        //----- LLEGADA -----//
        pthread_mutex_lock(&mutex);
        if(sem_trywait(&cerrado) == 0){ //el bar esta cerrado: hay que esperar al grupo
            sem_post(&cerrado);         //solo estaba preguntando, lo dejo como estaba
            sem_post(&esperando);       //me anoto como esperando
            pthread_mutex_unlock(&mutex); //suelto el mutex ANTES de dormirme
            printf("Cliente %d espera a que se vaya el grupo.\n", id);
            sem_wait(&bloqueo);         //el ultimo del grupo me va a despertar
        } else{                         //el bar esta abierto: me siento
            sem_wait(&asientosLibres);  //no se bloquea: abierto significa que hay asiento
            sem_post(&comiendo);
            if(sem_trywait(&asientosLibres) == 0){ //todavia queda algun asiento
                sem_post(&asientosLibres);         //devuelvo lo que saque al preguntar
            } else{                                //era el quinto: se llenaron los 5
                sem_post(&cerrado);
            }
            pthread_mutex_unlock(&mutex);
        }

        //----- COMER -----//
        printf("Cliente %d come sushi.\n", id);
        sleep(1);
        printf("Cliente %d termina de comer y se va.\n", id);

        //----- SALIDA -----//
        pthread_mutex_lock(&mutex);
        sem_post(&asientosLibres);      //libero mi asiento
        sem_wait(&comiendo);            //me resto de los que comen
        if(sem_trywait(&comiendo) == 0){ //queda alguien comiendo
            sem_post(&comiendo);         //devuelvo lo que saque al preguntar
        } else{                          //no queda nadie: era el ultimo del grupo
            sem_trywait(&cerrado);       //si estaba cerrado lo abro (si no, no pasa nada)
            int pasan = 0;
            while(pasan < ASIENTOS && sem_trywait(&esperando) == 0){
                sem_wait(&asientosLibres); //le ocupo el asiento al que esperaba
                sem_post(&comiendo);
                sem_post(&bloqueo);        //lo despierto
                pasan++;
            }
            if(pasan == ASIENTOS){         //pasaron 5: el bar vuelve a quedar cerrado
                sem_post(&cerrado);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    srand(time(NULL));

    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&asientosLibres, 0, ASIENTOS);
    sem_init(&comiendo, 0, 0);
    sem_init(&esperando, 0, 0);
    sem_init(&cerrado, 0, 0);
    sem_init(&bloqueo, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    //------ CREACION DE LOS HILOS -------//
    for(int i = 0; i < N_CLIENTES; i++){
        pthread_create(&hilosClientes[i], NULL, *cliente, (void *)(intptr_t)(i+1));
        printf("Se creo correctamente el cliente: %d.\n", i+1);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    for(int i = 0; i < N_CLIENTES; i++){
        pthread_join(hilosClientes[i], NULL);
        printf("Se elimino correctamente el cliente: %d.\n", i+1);
    }

    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&asientosLibres);
    sem_destroy(&comiendo);
    sem_destroy(&esperando);
    sem_destroy(&cerrado);
    sem_destroy(&bloqueo);
    pthread_mutex_destroy(&mutex);

    return 0;
}