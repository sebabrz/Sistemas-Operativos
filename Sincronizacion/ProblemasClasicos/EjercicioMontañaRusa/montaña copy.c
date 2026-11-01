#define ASIENTOS_CARRO 5
#define CANTIDAD_PASAJEROS 50
//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t carro_asientos;//ini=ASIENTOS_CARRO //contador: lugares libres del carro, arranca en 5
sem_t cupo_carro;//ini=ASIENTOS_CARRO//puerta: contador de 5
sem_t fin_viaje;//ini=0//el empleado avisa a los pasajeros del carro que termino el viaje
sem_t aviso;//ini=0
sem_t bajaron;//ini=0//contador: cada pasajero avisa que ya se bajo
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