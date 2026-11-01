#define N_HOMBRES 4
#define N_MUJERES 4
#define CAPACIDAD 3
//--------------------------- DECLARACION DE LOS HILOS ----------------------------//
pthread_t hilosHombres[N_HOMBRES];
pthread_t hilosMujeres[N_MUJERES];
//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
sem_t salon; //binario: exclusion entre generos, ini=1
sem_t cupo; //contador: maximo 3 personas adentro, ini=CAPACIDAD
sem_t hombresAdentro; //contador: cuantos hombres pasaron por la entrada, ini=0
sem_t mujeresAdentro; //contador: cuantas mujeres pasaron por la entrada, ini=0
pthread_mutex_t mutexHombres; //protege hombresAdentro
pthread_mutex_t mutexMujeres; //protege mujeresAdentro
//en el de perros y gatos necesitabas 2 semaforos por tipo de animal, 2 para el cupo de perro y perros adentro, 2 para cupo de gatos y gatos adentro
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
        }
        pthread_mutex_unlock(&mutexHombres);
        printf("Un hombre sale del salon\n");
        sem_post(&cupo);
        sleep(1);
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
        }
        pthread_mutex_unlock(&mutexMujeres);
        printf("Una mujer sale del salon\n");
        sem_post(&cupo);
        sleep(1);
    }
    return NULL;
}