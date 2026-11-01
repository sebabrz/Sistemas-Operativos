#define NUM_MESAS 2        // cantidad de mesas (generico: se puede cambiar a cualquier numero)
#define CAPACIDAD 10       // lugares de cada mesa
#define PARTICIPANTES 50
// Un juego de herramientas POR MESA: el indice del arreglo es el numero de mesa (0..NUM_MESAS-1)
sem_t lugaresLibres[NUM_MESAS];   // contador: lugares libres de la mesa, arranca en CAPACIDAD (bloquea al que quiere DEJAR si esta llena)
sem_t objetosEnMesa[NUM_MESAS];   // contador: objetos que hay en la mesa, arranca en 0 (bloquea al que quiere RETIRAR si esta vacia)
pthread_mutex_t mutex[NUM_MESAS]; // uno por mesa: solo un participante a la vez modifica ESA mesa

void dejarObjeto(int id, int mesa){
    printf("Soy el participante %d, deje un objeto en la mesa %d\n", id, mesa + 1);
}

void retirarObjeto(int id, int mesa){
    printf("Soy el participante %d, retire un objeto de la mesa %d\n", id, mesa + 1);
}

void *participante(void *arg){
    int id = (int)(intptr_t) arg;

    while(1){
        int mesa = rand() % NUM_MESAS;  // el participante elige en que mesa actuar (0..NUM_MESAS-1)
        int accion = rand() % 2;        // 0 = retirar, 1 = dejar

        printf("Soy el participante %d, elijo la mesa %d\n", id, mesa + 1);

        if(accion == 0){                               // RETIRAR
            sem_wait(&objetosEnMesa[mesa]);            // espero a que haya un objeto
            pthread_mutex_lock(&mutex[mesa]);          // solo uno a la vez toca esta mesa
            retirarObjeto(id, mesa);
            pthread_mutex_unlock(&mutex[mesa]);
            sem_post(&lugaresLibres[mesa]);            // se libero un lugar
        } else {                                       // DEJAR
            sem_wait(&lugaresLibres[mesa]);            // espero a que haya un lugar
            pthread_mutex_lock(&mutex[mesa]);
            dejarObjeto(id, mesa);
            pthread_mutex_unlock(&mutex[mesa]);
            sem_post(&objetosEnMesa[mesa]);            // hay un objeto mas
        }
        sleep(1);
    }
    return NULL;
}

int main(){
    pthread_t partici[PARTICIPANTES];
    srand(time(NULL));

    for(int m = 0; m < NUM_MESAS; m++){
        sem_init(&lugaresLibres[m], 0, CAPACIDAD);
        sem_init(&objetosEnMesa[m], 0, 0);
        pthread_mutex_init(&mutex[m], NULL);
    }

    for(int i = 0; i < PARTICIPANTES; i++){
        pthread_create(&partici[i], NULL, *participante, (void*)(intptr_t)(i+1));
    } 

    for(int i = 0; i < PARTICIPANTES; i++){
        pthread_join(partici[i], NULL);
    }

    for(int m = 0; m < NUM_MESAS; m++){
        sem_destroy(&lugaresLibres[m]);
        sem_destroy(&objetosEnMesa[m]);
        pthread_mutex_destroy(&mutex[m]);
    }

    return 0;
}