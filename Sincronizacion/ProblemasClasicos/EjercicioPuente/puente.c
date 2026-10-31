//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define CANTIDAD_NORTE 15
#define CANTIDAD_SUR 15

//--------------------------- DECLARACION DE LOS HILOS ----------------------------//
pthread_t hilosNorte[CANTIDAD_NORTE];
pthread_t hilosSur[CANTIDAD_SUR];

//------------------------------ IDS DE CADA HILO ---------------------------------//
int idsNorte[CANTIDAD_NORTE];
int idsSur[CANTIDAD_SUR];

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t puente; //binario: exclusion entre generos
sem_t nortePuente; //contador: cuantos hombres pasaron por la entrada
sem_t surPuente; //contador: cuantas mujeres pasaron por la entrada
pthread_mutex_t mutexNorte; //protege hombresAdentro
pthread_mutex_t mutexSur; //protege mujeresAdentro

//----------------------------------- RUTINAS -------------------------------------//
void *norte(void *arg){
    int id = *(int *)arg; //id de este hilo del norte
    while(1){
       
        printf("El norte %d quiere entrar al puente \n", id);
        pthread_mutex_lock(&mutexNorte);
            
            if(sem_trywait(&nortePuente) == 0){ //ya habia algun hombre
                sem_post(&nortePuente); //devuelvo lo que saque al preguntar
            } else{ //estaba en 0: soy el primero
                sem_wait(&puente);
            }

            sem_post(&nortePuente); //me sumo

        pthread_mutex_unlock(&mutexNorte);

        printf("Norte %d cruzando puente \n", id);
        sleep(2); //se atiende

        pthread_mutex_lock(&mutexNorte);
        
            sem_wait(&nortePuente);
            if(sem_trywait(&nortePuente) == 0){ //queda algun hombre
                sem_post(&nortePuente); //devuelvo lo que saque al preguntar
                printf("El norte %d sale del puente\n", id);
            } else{ //quedo en 0: era el ultimo
            printf("El norte %d es el ultimo y libera el puente\n", id);
            sem_post(&puente);
            }
            
        pthread_mutex_unlock(&mutexNorte);
        sleep(1);//simula que se va un tiempo antes de volver
    }
    return NULL;
}

void *sur(void *arg){
   int id = *(int *)arg; //id de este hilo del sur
   while(1){
       
        printf("El sur %d quiere entrar al puente \n", id);
        pthread_mutex_lock(&mutexSur);
            
            if(sem_trywait(&surPuente) == 0){ //ya habia algun hombre
                sem_post(&surPuente); //devuelvo lo que saque al preguntar
            } else{ //estaba en 0: soy el primero
                sem_wait(&puente);
            }

            sem_post(&surPuente); //me sumo

        pthread_mutex_unlock(&mutexSur);

        printf("Sur %d cruzando puente \n", id);
        sleep(2); //se atiende

        pthread_mutex_lock(&mutexSur);
        
            sem_wait(&surPuente);
            if(sem_trywait(&surPuente) == 0){ //queda algun hombre
                sem_post(&surPuente); //devuelvo lo que saque al preguntar
                printf("El sur %d sale del puente\n", id);
            } else{ //quedo en 0: era el ultimo
            printf("El sur %d es el ultimo y libera el puente\n", id);
            sem_post(&puente);
            }
            
        pthread_mutex_unlock(&mutexSur);
        sleep(1);
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&puente, 0, 1);
    sem_init(&nortePuente, 0, 0);
    sem_init(&surPuente, 0, 0);
    pthread_mutex_init(&mutexNorte, NULL);
    pthread_mutex_init(&mutexSur, NULL);

    //------ CREACION DE LOS HILOS -------//
    for(int i = 0; i < CANTIDAD_NORTE; i++){
        idsNorte[i] = i + 1;
        pthread_create(&hilosNorte[i], NULL, *norte, &idsNorte[i]);
    }

    for(int i = 0; i < CANTIDAD_SUR; i++){
        idsSur[i] = i + 1;
        pthread_create(&hilosSur[i], NULL, *sur, &idsSur[i]);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    for(int i = 0; i < CANTIDAD_NORTE; i++){
        pthread_join(hilosNorte[i], NULL);
    }

    for(int i = 0; i < CANTIDAD_SUR; i++){
        pthread_join(hilosSur[i], NULL);
    }

    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&puente);
    sem_destroy(&nortePuente);
    sem_destroy(&surPuente);
    pthread_mutex_destroy(&mutexNorte);
    pthread_mutex_destroy(&mutexSur);

    return 0;
}