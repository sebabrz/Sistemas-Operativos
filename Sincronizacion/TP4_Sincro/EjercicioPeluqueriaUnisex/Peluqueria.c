//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define N_HOMBRES 4
#define N_MUJERES 4
#define CAPACIDAD 3

//--------------------------- DECLARACION DE LOS HILOS ----------------------------//
pthread_t hilosHombres[N_HOMBRES];
pthread_t hilosMujeres[N_MUJERES];

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t salon; //binario: exclusion entre generos
sem_t cupo; //contador: maximo 3 personas adentro
sem_t hombresAdentro; //contador: cuantos hombres pasaron por la entrada
sem_t mujeresAdentro; //contador: cuantas mujeres pasaron por la entrada
pthread_mutex_t mutexHombres; //protege hombresAdentro
pthread_mutex_t mutexMujeres; //protege mujeresAdentro

//----------------------------------- RUTINAS -------------------------------------//
void *hombres(void *arg){
    while(1){
        sem_wait(&cupo); //lugar libre (maximo 3), suponiendo que hay hombres, podria entrar?
        printf("un hombre entra al salon \n");
        pthread_mutex_lock(&mutexHombres);
        if(sem_trywait(&hombresAdentro) == 0){ //ya habia algun hombre
            sem_post(&hombresAdentro); //devuelvo lo que saque al preguntar
        } else{ //estaba en 0: soy el primero
            sem_wait(&salon);
        }
        sem_post(&hombresAdentro); //me sumo
        pthread_mutex_unlock(&mutexHombres);
        sleep(1); //se atiende
        pthread_mutex_lock(&mutexHombres);
        sem_wait(&hombresAdentro);

        if(sem_trywait(&hombresAdentro) == 0){ //queda algun hombre
            sem_post(&hombresAdentro); //devuelvo lo que saque al preguntar
        } else{ //quedo en 0: era el ultimo
            sem_post(&salon);
            sleep(5);
        }
        pthread_mutex_unlock(&mutexHombres);
        printf("Un hombre sale del salon\n");
        sem_post(&cupo);
    }
    return NULL;
}

void *mujeres(void *arg){
    while(1){
        sem_wait(&cupo);
        printf("una mujer entra al salon \n");
        pthread_mutex_lock(&mutexMujeres);
        if(sem_trywait(&mujeresAdentro) == 0){
            sem_post(&mujeresAdentro);
        } else{
            sem_wait(&salon);
        }
        sem_post(&mujeresAdentro);
        pthread_mutex_unlock(&mutexMujeres);

        pthread_mutex_lock(&mutexMujeres);
        sem_wait(&mujeresAdentro);
        if(sem_trywait(&mujeresAdentro) == 0){
            sem_post(&mujeresAdentro);
        } else{
            sem_post(&salon);
            sleep(3);
        }
        pthread_mutex_unlock(&mutexMujeres);
        printf("Una mujer sale del salon\n");
        sem_post(&cupo);
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&salon, 0, 1);
    sem_init(&cupo, 0, CAPACIDAD);
    sem_init(&hombresAdentro, 0, 0);
    sem_init(&mujeresAdentro, 0, 0);
    pthread_mutex_init(&mutexHombres, NULL);
    pthread_mutex_init(&mutexMujeres, NULL);

    //------ CREACION DE LOS HILOS -------//
    for(int i = 0; i < N_HOMBRES; i++){
        pthread_create(&hilosHombres[i], NULL, *hombres, NULL);
        printf("Se creo correctamente el hombre: %d.\n", i+1);
    }

    for(int i = 0; i < N_MUJERES; i++){
        pthread_create(&hilosMujeres[i], NULL, *mujeres, NULL);
        printf("Se creo correctamente la mujer: %d.\n", i+1);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    for(int i = 0; i < N_HOMBRES; i++){
        pthread_join(hilosHombres[i], NULL);
        printf("Se elimino correctamente el hombre: %d.\n", i+1);
    }

    for(int i = 0; i < N_MUJERES; i++){
        pthread_join(hilosMujeres[i], NULL);
        printf("Se elimino correctamente la mujer: %d.\n", i+1);
    }

    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&salon);
    sem_destroy(&cupo);
    sem_destroy(&hombresAdentro);
    sem_destroy(&mujeresAdentro);
    pthread_mutex_destroy(&mutexHombres);
    pthread_mutex_destroy(&mutexMujeres);

    return 0;
}