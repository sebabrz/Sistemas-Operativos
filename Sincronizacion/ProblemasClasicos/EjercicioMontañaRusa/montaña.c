//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define ASIENTOS_CARRO 5
#define CANTIDAD_PASAJEROS 50

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t carro_asientos;  //contador: lugares libres del carro, arranca en 5
sem_t cupo_carro;      //puerta: contador de 5
sem_t fin_viaje;       //el empleado avisa a los pasajeros del carro que termino el viaje
sem_t aviso;
sem_t bajaron;         //contador: cada pasajero avisa que ya se bajo
pthread_mutex_t fila; //semaforo de tipo mutex

//----------------------------------- RUTINAS -------------------------------------//
void *pasajero(void *arg){ 
    while(1){
        sem_wait(&cupo_carro);//si no esta en viaje, podria subirme?
        pthread_mutex_lock(&fila);
            sem_wait(&carro_asientos);
                if(sem_trywait(&carro_asientos)==0){//si puedo descontar, no soy el ultimo
                    sem_post(&carro_asientos);
                    printf("Se subio un pasajero al carro\n");
                }else{//si no puedo descontar, soy el ultimo
                    printf("Se subio el ultimo pasajero al carro y le avisa al empleado\n");
                    sem_post(&aviso);   //le aviso al empleado para que inicie el viaje
                }
        pthread_mutex_unlock(&fila);
        sem_wait(&fin_viaje);//todos los que subieron esperan el fin del viaje
        printf("El pasajero finalizo el viaje y se baja del carro\n");
        sem_post(&bajaron);//aviso que ya me baje
        sleep(1);
    }
    return NULL;
}

void *empleado(void *arg){ 
    while(1){
        sem_wait(&aviso);
        printf("El empleado recibe el aviso de que se subieron todos\n");
        printf("El empleado inicia viaje de montaña rusa\n");
        sleep(1);
        printf("El empleado avisa que termino el viaje de montaña\n");
        pthread_mutex_lock(&fila);//asi los pasajeros ven los asientos en 0 o todos libres, nunca a medias
            for(int i=0;i<ASIENTOS_CARRO;i++){
                sem_post(&fin_viaje);
            }
            for(int i=0;i<ASIENTOS_CARRO;i++){
                sem_post(&carro_asientos);
            }
        pthread_mutex_unlock(&fila);
        for(int i=0;i<ASIENTOS_CARRO;i++){
            sem_wait(&bajaron);//espero a que se bajen los 5 antes de cargar el proximo grupo
        }
        pthread_mutex_lock(&fila);
            for(int i=0;i<ASIENTOS_CARRO;i++){
                sem_post(&cupo_carro);//abro la puerta para que suba el proximo grupo de pasajeros
            }      
        pthread_mutex_unlock(&fila);
        
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    srand(time(NULL));

    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&cupo_carro, 0, ASIENTOS_CARRO);  
    sem_init(&carro_asientos, 0, ASIENTOS_CARRO);
    sem_init(&fin_viaje, 0, 0);
    sem_init(&aviso, 0, 0);
    sem_init(&bajaron, 0, 0);
    pthread_mutex_init(&fila, NULL);
    
    //--------------------------- DECLARACION DE LOS HILOS ----------------------------//
    pthread_t pasajeros[CANTIDAD_PASAJEROS]; //Muchos hilos del mismo tipo
    pthread_t empleado_tid;          //Un unico hilo

    //------ CREACION DE LOS HILOS -------//
    pthread_create(&empleado_tid, NULL, *empleado, NULL);
    
    for(int i = 0; i < CANTIDAD_PASAJEROS; i++){
        pthread_create(&pasajeros[i], NULL, *pasajero, NULL);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    pthread_join(empleado_tid, NULL);

    for(int i = 0; i < CANTIDAD_PASAJEROS; i++){
        pthread_join(pasajeros[i], NULL);
    }
    
    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&carro_asientos);
    sem_destroy(&cupo_carro);
    sem_destroy(&fin_viaje);
    sem_destroy(&aviso);
    sem_destroy(&bajaron);
    pthread_mutex_destroy(&fila);

    return 0;
}