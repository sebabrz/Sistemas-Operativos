//------------------------------ AGREGO LOS INCLUDE -------------------------------//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//--------------------- AGREGOS LOS DEFINE (de ser necesario) ---------------------//
#define CANTIDAD_DUENDES 15 //PUEDEN HABER MUCHOS DUENDES
#define DUENDES_GRUPO 3 
#define CANTIDAD_RENOS 9 //SIEMPRE SON 9 NADA MAS

//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t renos_juntos;// contador: 9 lugares para renos
sem_t duendes_grupo;// contador: 3 lugares para duendes
sem_t fin_renos; 
sem_t fin_duendes;     
sem_t aviso;
pthread_mutex_t poloNorte; 

//----------------------------------- RUTINAS -------------------------------------//
void *reno(void *arg){ 
    while(1){
    pthread_mutex_lock(&poloNorte);
    //como somos 9, descuento mi lugar, en este caso no me bloquea el sem_wait adentro del mutex porque se que siempre son 9 RENOS, no va a tomar otro el mutex y quedarse trabado ahi
        sem_wait(&renos_juntos);//sino cambiarlo a trywait como los duendes
        if(sem_trywait(&renos_juntos)==0){//si puedo descontar, no soy el ultimo, tengo que esperar a mas renos
            sem_post(&renos_juntos);
            printf("Llegó un reno y espera a los demas\n");
        }else{//si no puedo descontar, soy el ultimo
            printf("El ultimo reno llego y le avisa a Santa para salir a repartir jueguetes\n");
            sem_post(&aviso);   //le aviso a santa
            }
        pthread_mutex_unlock(&poloNorte);//libero por si queiren unirse mas duendes
        
        sem_wait(&fin_renos);//todos los duendes esperan a que santa los termine de ayudar
        printf("Un reno termino y se va a descansar\n");
        sleep(1);
    }
    return NULL;
}

void *duende(void *arg){
    pthread_mutex_lock(&poloNorte);
        if(sem_trywait(&duendes_grupo)==0){//me fijo si hay cupo de duendes para pedir ayuda, sino me voy
            if(sem_trywait(&duendes_grupo)==0){//si puedo descontar, no soy el ultimo, tengo que esperar a mas duendes
                sem_post(&duendes_grupo);
                printf("Un duende trata de fabricar juguetes\n");
            }else{//si no puedo descontar, soy el ultimo
                printf("El ultimo duende llego y pide ayuda a Santa para fabricar juguetes\n");
                sem_post(&aviso);   //le aviso a santa
            }
            
            pthread_mutex_unlock(&poloNorte);//libero por si queiren unirse mas duendes
                    
            sem_wait(&fin_duendes);//todos los duendes esperan a que santa los termine de ayudar
            printf("El duende termino de fabricar juguetes con Santa\n");
        }else{//no hay cupo de 3 duendes para que los ayude santa porque ya esta con otros duendes
            printf("El duende no espera porque Santa esta ayudando a otros duendes\n");
            pthread_mutex_unlock(&poloNorte);
            }
    return NULL;
}

void *santa(void *arg){ 
    while(1){
        sem_wait(&aviso);

        pthread_mutex_lock(&poloNorte);
            if(sem_trywait(&duendes_grupo)==0){//pude descontar: el grupo de 3 duendes no esta completo, asique me llamaron los renos
                sem_post(&duendes_grupo);
                pthread_mutex_unlock(&poloNorte);
                printf("Santa inicia el viaje con los 9 renos\n");
                printf("Santa avisa que termino el viaje\n");
                pthread_mutex_lock(&poloNorte);//para que si liberoa unos, no vuelvan y conpleten el gurpo devuelta antes de que libere a todos
                    for(int i=0;i<CANTIDAD_RENOS;i++){
                        sem_post(&fin_renos);//les aviso a todos que terminamos de repartir
                    }
                    for(int i=0;i<CANTIDAD_RENOS;i++){
                        sem_post(&renos_juntos);//libero el cupo de renos
                    }
                pthread_mutex_unlock(&poloNorte);
            }else{//no pude descontar duendes: me llamaron los duendes
                pthread_mutex_unlock(&poloNorte);
                printf("Santa se pone a fabricar juguetes con los duendes\n");
                printf("Santa termina de fabricar juguetes con los duendes\n");
                pthread_mutex_lock(&poloNorte);//para que si libero un cupo, no llegue otro duende pensando que es el ultimo y lo complete
                    for(int i=0;i<DUENDES_GRUPO;i++){
                        sem_post(&fin_duendes);//les aviso que terminamos de fabricar
                    }
                    for(int i=0;i<DUENDES_GRUPO;i++){
                        sem_post(&duendes_grupo);//libero los cupos apra que lleguen mas duendes
                    }
                pthread_mutex_unlock(&poloNorte);
            }
            sleep(2);
    }
    return NULL;
}

//------------------------------------- MAIN --------------------------------------//
int main(){
    srand(time(NULL));

    //----- INICIALIZO LOS SEMAFOROS -----//
    sem_init(&renos_juntos, 0, CANTIDAD_RENOS);  // 9
    sem_init(&duendes_grupo, 0, DUENDES_GRUPO);// 3
    sem_init(&aviso, 0, 0);
    sem_init(&fin_duendes, 0, 0);
    sem_init(&fin_renos, 0, 0);
    pthread_mutex_init(&poloNorte, NULL);
    
    //--------------------------- DECLARACION DE LOS HILOS ----------------------------//
    pthread_t renos[CANTIDAD_RENOS]; //Muchos hilos del mismo tipo
    pthread_t duendes[CANTIDAD_DUENDES];
    pthread_t santa_tid;          //Un unico hilo

    //------ CREACION DE LOS HILOS -------//
    pthread_create(&santa_tid, NULL, *santa, NULL);
    
    for(int i = 0; i < CANTIDAD_RENOS; i++){
        pthread_create(&renos[i], NULL, *reno, NULL);
    }
    for(int i = 0; i < CANTIDAD_DUENDES; i++){
        pthread_create(&duendes[i], NULL, *duende, NULL);
    }

    //------ ESPERO A QUE TERMINEN LOS HILOS (nunca) -------//
    pthread_join(santa_tid, NULL);

    for(int i = 0; i < CANTIDAD_RENOS; i++){
        pthread_join(renos[i], NULL);
    }
    for(int i = 0; i < CANTIDAD_DUENDES; i++){
        pthread_join(duendes[i], NULL);
    }
    
    //------ DESTRUYO LOS SEMAFOROS (nunca) -------//
    sem_destroy(&renos_juntos);
    sem_destroy(&duendes_grupo);
    sem_destroy(&aviso);
    sem_destroy(&fin_renos);
    sem_destroy(&fin_duendes);
    pthread_mutex_destroy(&poloNorte);

    return 0;
}