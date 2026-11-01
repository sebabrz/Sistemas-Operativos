#define ASIENTOS_AUTO 4
#define ASIENTOS_COMBI 15 
#define CANTIDAD_PASAJEROS 100
sem_t auto_asientos;//ini=ASIENTOS_AUTO //semaforo normal (contador o binario)
sem_t combi_asientos;//ini= ASIENTOS_COMBI
sem_t fin_auto;//ini=0//el chofer avisa a los pasajeros del auto que termino el viaje
sem_t fin_combi;//ini=0//el chofer avisa a los pasajeros de la combi que termino el viaje
sem_t aviso;//ini=0//el ultimo pasajero le avisa al chofer que un vehiculo esta completo
pthread_mutex_t pasaje; //semaforo de tipo mutex
pthread_t pasajeros[CANTIDAD_PASAJEROS]; //Muchos hilos del mismo tipo
pthread_t chofer_tid;          //Un unico hilo
//----------------------------------- RUTINAS -------------------------------------//
void *pasajero(void *arg){ 
        int r=rand()%2;
        if(r==0){//me toca el auto  
            pthread_mutex_lock(&pasaje);//los clientes pasan de a uno
                if(sem_trywait(&auto_asientos)==0){//me fijo si hay cupo, sino me voy
                    if(sem_trywait(&auto_asientos)==0){//si puedo descontar, no soy el ultimo
                        sem_post(&auto_asientos);
                        printf("Se subio un pasajero al auto\n");
                    }else{//si no puedo descontar, soy el ultimo
                        printf("Se subio el ultimo pasajero al auto y le avisa al chofer\n");
                        sem_post(&aviso);   //le aviso al chofer
                    }
                    pthread_mutex_unlock(&pasaje);
                    sem_wait(&fin_auto);//todos los que subieron esperan el fin del viaje
                    printf("El pasajero finalizo el viaje y se baja del auto\n");
                }else{
                    printf("El pasajero no tuvo pasaje de auto\n");
                    pthread_mutex_unlock(&pasaje);
                }
        }
        else{//me toca la combi
            pthread_mutex_lock(&pasaje);
                if(sem_trywait(&combi_asientos)==0){//me fijo si hay cupo, sino me voy
                    if(sem_trywait(&combi_asientos)==0){//si puedo descontar, no soy el ultimo
                        sem_post(&combi_asientos);
                        printf("Se subio un pasajero a la combi\n");
                    }else{//si no puedo descontar, soy el ultimo
                        printf("Se subio el ultimo pasajero a la combi y le avisa al chofer\n");
                        sem_post(&aviso);   //le aviso al chofer
                    }
                    pthread_mutex_unlock(&pasaje);
                    sem_wait(&fin_combi);//todos los que subieron esperan el fin del viaje
                    printf("El pasajero finalizo el viaje y se baja de la combi\n");
                }else{
                    printf("El pasajero no tuvo pasaje de combi\n");
                    pthread_mutex_unlock(&pasaje);
                }
        }
        
    return NULL;
}

void *chofer(void *arg){ 
    while(1){
        sem_wait(&aviso);

        pthread_mutex_lock(&pasaje);
            if(sem_trywait(&auto_asientos)==0){//pude descontar: el auto no esta completo, el aviso es de la combi
                sem_post(&auto_asientos);
                pthread_mutex_unlock(&pasaje);
                printf("El chofer inicia viaje de combi\n");
                printf("El chofer avisa que termino el viaje de combi\n");
                pthread_mutex_lock(&pasaje);//asi los pasajeros ven los asientos en 0 o todos libres, nunca a medias
                    for(int i=0;i<ASIENTOS_COMBI;i++){
                        sem_post(&fin_combi);
                    }
                    for(int i=0;i<ASIENTOS_COMBI;i++){
                        sem_post(&combi_asientos);
                    }
                pthread_mutex_unlock(&pasaje);
            }else{//no pude descontar: el auto esta completo
                pthread_mutex_unlock(&pasaje);
                printf("El chofer inicia viaje en auto\n");
                printf("El chofer avisa que termino el viaje de auto\n");
                pthread_mutex_lock(&pasaje);
                    for(int i=0;i<ASIENTOS_AUTO;i++){
                        sem_post(&fin_auto);
                    }
                    for(int i=0;i<ASIENTOS_AUTO;i++){
                        sem_post(&auto_asientos);
                    }
                pthread_mutex_unlock(&pasaje);
            }
    }
    return NULL;
}