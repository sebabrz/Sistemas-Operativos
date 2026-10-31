#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


#define HUESPEDES 30
#define MAXIMA 5
/*
Un hotel tiene una heladera compartida que almacena bebidas de 3 tipos: agua, cerveza y leche. 
Cada tipo tiene un stock máximo y la heladera comienza llena.

Los huéspedes (varios hilos concurrentes) usan la heladera para consumir una bebida del tipo que deseen. 
Solo un huésped puede usar la heladera a la vez, por lo que el acceso debe garantizarse con exclusión mutua.

Si un huésped consume la última unidad de un tipo de bebida, debe avisar al repositor. 
El repositor es un hilo que permanece dormido mientras no haya nada que reponer, 
y al despertarse repone el tipo de bebida agotado hasta completar su stock. 
Mientras el repositor repone, ningún huésped puede acceder a la heladera.

Si un huésped quiere una bebida que no hay en ese momento, no la consume y se retira sin quedarse esperando.


*/

pthread_t huespedes[HUESPEDES];
pthread_t reposin;

sem_t semAgua;      //Semaforo contador, inicilizado en 5
sem_t semCerveza;   //
sem_t semLeche;     //
sem_t avisoAlRepo;        //Le avisa al repositor, cuando tiene que reponer alguna bebida. 
sem_t finRepo;
pthread_mutex_t HELADERA;

void agarroCerveza() {
    
    pthread_mutex_lock(&HELADERA);
    sem_wait(&semCerveza);

    if (sem_trywait(&semCerveza) == 0){
        sem_post(&semCerveza);
        printf ("Agarre una Cerveza de la heladera \n");
    }   else {
        printf ("Agarre la ultima cerveza y le aviso al repositor \n");
        sem_post(&avisoAlRepo);
        sem_wait(&finRepo);
    }
    pthread_mutex_unlock(&HELADERA);

}

void agarroAgua() {

    pthread_mutex_lock(&HELADERA);
    sem_wait(&semAgua);
    if (sem_trywait(&semAgua) == 0){
        sem_post(&semAgua);
        printf ("Agarre una Agua de la heladera \n");
    }   else {
        printf ("Agarre la ultima agua y le aviso al repositor \n");
        sem_post(&avisoAlRepo);
        sem_wait(&finRepo);
    }
    pthread_mutex_unlock(&HELADERA);

}

void agarroLeche() {
    pthread_mutex_lock(&HELADERA);
    sem_wait(&semLeche);
    if (sem_trywait(&semLeche) == 0){
        sem_post(&semLeche);
        printf ("Agarre una Leche de la heladera \n");
    }   else {
        printf ("Agarre la ultima leche y le aviso al repositor \n");
        sem_post(&avisoAlRepo);
        sem_wait(&finRepo);
    }
    pthread_mutex_unlock(&HELADERA);
}


void *huesped(void *arg){
    while(1){
        int bebida = rand() % 3; //0,1,2

        switch (bebida) {
            case 0:
            agarroCerveza();
            break;

            case 1:
            agarroAgua();
            break;
            
            case 2:
            agarroLeche();
            break;
        }
        sleep(1); //para que enseguida tome el control, y le de el lugar a otros 
    }       
    return NULL;
}

void *repositor(void *arg){
    while(1){
    printf("El Repositor esta esperando que le avise para reponer \n");
    sem_wait(&avisoAlRepo);
    printf("Se desperto \n");
    //Si pude decrementar de Cerveza, es que hay cerveza,
    //devuelvo el que obtuve
    //Si no puedo decrementar
    if (sem_trywait(&semCerveza) == 0) {
        sem_post(&semCerveza);
    } else {
        for (int i=0 ; i<MAXIMA; i++){
            sem_post(&semCerveza);
        }
        printf ("No habia mas cerveza repuse \n");
    } 
    
    if (sem_trywait(&semLeche) == 0) {
        sem_post(&semLeche);
    } else {
        for (int i=0 ; i<MAXIMA; i++){
            sem_post(&semLeche);
        }
        printf ("No habia mas Leche repuse \n");
    } 
    
    if (sem_trywait(&semAgua) == 0) {
        sem_post(&semAgua);
    } else {
        for (int i=0 ; i<MAXIMA; i++){
            sem_post(&semAgua);
        }
        printf ("No habia mas Agua repuse \n");
    }

    sem_post(&finRepo);
    printf("Se durmio \n");
    }
    return NULL;
}

int main(){
    srand(time(NULL));

    sem_init(&semLeche, 0, MAXIMA);
    sem_init(&semCerveza, 0, MAXIMA);
    sem_init(&semAgua, 0, MAXIMA);
    sem_init(&finRepo, 0, 0);
    sem_init(&avisoAlRepo, 0, 0);
    pthread_mutex_init(&HELADERA, NULL);

    pthread_create(&reposin, NULL, *repositor, NULL);

    for(int i = 0; i < HUESPEDES; i++){
        pthread_create(&huespedes[i], NULL, *huesped, NULL);
    }

    pthread_join(reposin, NULL);

    for(int i = 0; i < HUESPEDES; i++){
        pthread_join(huespedes[i], NULL);
        printf("Se elimino correctamente el hilo: %d.\n", i+1);
    }

    sem_destroy(&semLeche);
    sem_destroy(&semAgua);
    sem_destroy(&semCerveza);
    sem_destroy(&finRepo);
    sem_destroy(&avisoAlRepo);

    pthread_mutex_destroy(&HELADERA);

    return 0;
}