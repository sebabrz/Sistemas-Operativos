/*  Si entran perros a la veterinaria, no pueden entrar gatos y viceversa (no intercalado)

    la capacidad maxima de perros es 5 y la capacidad maxima de gatos es 7

*/
//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define CUPO_GATOS 7
#define CUPO_PERROS 5
#define GATOS 10
#define PERROS 10

//--------------------------- DECLARACION DE LOS HILOS ----------------------------//
pthread_t hilosPerros[PERROS];
pthread_t hilosGatos[GATOS];

//------------------------------ IDS DE CADA ANIMAL -------------------------------//
int idsPerros[PERROS];
int idsGatos[GATOS];

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t veterinaria; //binario: exclusion entre animales, o hay gatos o hay perros
sem_t cupo_gatos; //contador: maximo 7 gatos adentro
sem_t cupo_perros; //contador: maximo 5 perros adentro
sem_t perrosAdentro; //contador: cuantos perros hay adentro
sem_t gatosAdentro; //contador: cuantos gatos hay adentro
pthread_mutex_t mutexPerros; //protege perrosAdentro
pthread_mutex_t mutexGatos; //protege gatosAdentro

//----------------------------------- RUTINAS -------------------------------------//
void *perro(void *arg){
    int id = *(int *)arg; //id de este perro
    while(1){
        sem_wait(&cupo_perros);//en el caso de que haya perros, hay lugar para uno mas?
        printf("Perro %d ve si puede entrar a la veterinaria ya que hay cupo de perros\n", id);//si lo hay, decremento 1 cupo

        pthread_mutex_lock(&mutexPerros);//tomo mutex para que entre 1 perro a la vez

            if(sem_trywait(&perrosAdentro) == 0){//hay perros adentro?
                sem_post(&perrosAdentro);//si los hay, devuelvo la posicion a ese perro que decremente al consultar, no soy el primero
            } else{
                sem_wait(&veterinaria);//si no hay, es porque la veterinaria esta vacia o hay gatos, intento tomarla por si esta vacia, sino espero a que la vacien
            }
            sem_post(&perrosAdentro);//si habia perros adentro, me sumo como uno mas adentro, sino, soy el primero luego de tomar la veterinaria y me sumo como el primero
            printf("Entro el perro %d a la veterinaria\n", id);

        pthread_mutex_unlock(&mutexPerros);//libero para que entre otro

        printf("Se atiende el perro %d\n", id);

        pthread_mutex_lock(&mutexPerros);//tomo el mutex para que los perros se vayan de a uno a la vez

            sem_wait(&perrosAdentro);//consumo la posicion que estaba ocupando adentro, es decir que hay 1 perro menos adentro
            printf("El perro %d se va de la veterinaria\n", id);//aviso la salida antes de liberar el salon, asi el log respeta el orden real
            if(sem_trywait(&perrosAdentro) == 0){//hay mas perros adentro?
                sem_post(&perrosAdentro);//si hay, devuelvo la posicion que consumi al consultar, no soy el ultimo en irse
            } else{
                sem_post(&veterinaria);//si no hay, libero el salon ya que soy el ultimo
                sleep(3);//doy lugar a que lo tome un gato si quiere asi evito inanicion de gatos
            }
            
        pthread_mutex_unlock(&mutexPerros);//libero el mutex para que si hay otro perro se pueda ir despues de mi

        sem_post(&cupo_perros);//libero el cupo para si un perro llega pueda entrar
    }
    return NULL;
}

void *gato(void *arg){
    int id = *(int *)arg; //id de este gato
    while(1){
        sem_wait(&cupo_gatos);//en el caso de que haya gatos, hay lugar para uno mas?
        printf("Gato %d ve si puede entrar a la veterinaria ya que hay cupo de gatos\n", id);//si lo hay, decremento 1 cupo

        pthread_mutex_lock(&mutexGatos);//tomo mutex para que entre 1 gato a la vez

            if(sem_trywait(&gatosAdentro) == 0){//hay gatos adentro?
                sem_post(&gatosAdentro);//si los hay, devuelvo la posicion a ese gato que decremente al consultar
            } else{
                sem_wait(&veterinaria);//si no hay, es porque la veterinaria esta vacia o hay perros, intento tomarla por si esta vacia, sino espero a que la vacien
            }
            sem_post(&gatosAdentro);//si habia gatos adentro, me sumo como uno mas adentro, sino, soy el primero luego de tomar la veterinaria y me sumo como el primero
            printf("Entro el gato %d a la veterinaria\n", id);

        pthread_mutex_unlock(&mutexGatos);//libero para que entre otro

        printf("Se atiende el gato %d\n", id);

        pthread_mutex_lock(&mutexGatos);//tomo el mutex para que los gatos se vayan de a uno a la vez

            sem_wait(&gatosAdentro);//consumo la posicion que estaba ocupando adentro, es decir que hay 1 gato menos adentro
            printf("El gato %d se va de la veterinaria\n", id);//aviso la salida antes de liberar el salon, asi el log respeta el orden real
            if(sem_trywait(&gatosAdentro) == 0){//hay mas gatos adentro?
                sem_post(&gatosAdentro);//si hay, devuelvo la posicion que consumi al consultar, no soy el ultimo en irse
            } else{
                sem_post(&veterinaria);//si no hay, libero el salon ya que soy el ultimo
                sleep(3);//doy lugar a que lo tome un perro si quiere asi evito inanicion de perros
            }

        pthread_mutex_unlock(&mutexGatos);//libero el mutex para que si hay otro gato se pueda ir despues de mi

        sem_post(&cupo_gatos);//libero el cupo para si un gato llega pueda entrar
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&veterinaria, 0, 1);
    sem_init(&cupo_gatos, 0, CUPO_GATOS);
    sem_init(&cupo_perros, 0, CUPO_PERROS);
    sem_init(&gatosAdentro, 0, 0);
    sem_init(&perrosAdentro, 0, 0);
    pthread_mutex_init(&mutexPerros, NULL);
    pthread_mutex_init(&mutexGatos, NULL);

    //------ CREACION DE LOS HILOS (cada uno recibe su id) -------//
    for(int i = 0; i < PERROS; i++){
        idsPerros[i] = i + 1;
        pthread_create(&hilosPerros[i], NULL, perro, &idsPerros[i]);
    }

    for(int i = 0; i < GATOS; i++){
        idsGatos[i] = i + 1;
        pthread_create(&hilosGatos[i], NULL, gato, &idsGatos[i]);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    for(int i = 0; i < PERROS; i++){
        pthread_join(hilosPerros[i], NULL);
    }

    for(int i = 0; i < GATOS; i++){
        pthread_join(hilosGatos[i], NULL);
    }

    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&veterinaria);
    sem_destroy(&cupo_gatos);
    sem_destroy(&cupo_perros);
    sem_destroy(&gatosAdentro);
    sem_destroy(&perrosAdentro);
    pthread_mutex_destroy(&mutexGatos);
    pthread_mutex_destroy(&mutexPerros);

    return 0;
}