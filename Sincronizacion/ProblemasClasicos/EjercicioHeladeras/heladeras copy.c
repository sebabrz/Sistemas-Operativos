#define HUESPEDES 30
#define MAXIMA 5
pthread_t huespedes[HUESPEDES];
pthread_t reposin;
sem_t semAgua;//Semaforo contador, inicilizado en MAXIMA
sem_t semCerveza;//inicilizado en MAXIMA
sem_t semLeche;//inicilizado en MAXIMA
sem_t avisoAlRepo;//Le avisa al repositor, cuando tiene que reponer alguna bebida. INI=0
sem_t finRepo;//INI=0
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
