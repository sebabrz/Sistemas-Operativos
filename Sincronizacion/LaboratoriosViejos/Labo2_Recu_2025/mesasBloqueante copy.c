#define MESAS 10
#define PARTICIPANTES 50
pthread_t partici[PARTICIPANTES];
sem_t mesa1; //contador, con maximo de mesa 1, ini=0
sem_t vacio1; //contador, ini=MESAS lugares libres
sem_t vacio2; //contador, ini=MESAS lugares libres
sem_t mesa2; //contador, con maximo de mesa 2 , ini=0
pthread_mutex_t MUTEX1; //mutex para la mesa1 
pthread_mutex_t MUTEX2; //mutex para la mesa2

void dejarObjeto1(int id){
        printf("Soy el participante %d , se pudo dejar objeto en la mesa 1 \n", id);
}

void dejarObjeto2(int id){
        printf("Soy el participante %d Se pudo dejar objeto en la mesa 2 \n", id);
}

void retirarObjeto1(int id){
        printf("Soy el participante %d, se pudo retirar objeto en la mesa 1 \n", id);
}

void retirarObjeto2(int id){
        printf("Soy el participante %d pudo retirar objeto en la mesa 2 \n", id);
}

void *participante(void *arg){
while(1){
    int id = (int) (intptr_t) arg;

    int mesa = (rand() % 2) + 1; //Aca el participante elije donde accionar en cada a mesa 
    int accion = (rand() % 2) + 1; //retirar o deja?


    if (mesa == 1 ) {
        printf("Soy el participante: %d elije la mesa 1 \n", id);
        if (accion == 1) {
            sem_wait(&mesa1);
            pthread_mutex_lock(&MUTEX1);
            retirarObjeto1(id);
            pthread_mutex_unlock(&MUTEX1);
            sem_post(&vacio1);
        } else {
            sem_wait(&vacio1);
            pthread_mutex_lock(&MUTEX1);
            dejarObjeto1(id);
            pthread_mutex_unlock(&MUTEX1);
            sem_post(&mesa1);
        }
    } else {
        printf("Soy el participante: %d elije la mesa 2 \n", id);
        
        
        if (accion == 1) {
            sem_wait(&mesa2);
            pthread_mutex_lock(&MUTEX2);
            retirarObjeto2(id);
            pthread_mutex_unlock(&MUTEX2);
            sem_post(&vacio2);
        } else {
            sem_wait(&vacio2);
            pthread_mutex_lock(&MUTEX2);
            dejarObjeto2(id);
            pthread_mutex_unlock(&MUTEX2);
            sem_post(&mesa2);
        }
    }
    sleep(1);
    }
    return NULL;
}
