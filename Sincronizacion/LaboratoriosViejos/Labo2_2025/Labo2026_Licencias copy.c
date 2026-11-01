#define USUARIOS 30
pthread_t hilosUsuarios[USUARIOS]; //Muchos hilos del mismo tipo
sem_t licencias;  //semaforo normal (contador o binario), ini=20 o NRO_LICENCIAS
pthread_mutex_t BD; //semaforo de tipo mutex
void *Usuarios(void *arg){  
    int id = (int)(intptr_t) arg;
    int numRand = (rand() % 3) + 1 ; //0 o 1 o 2 //Cada usuario puede requerir entre 1 y 3 licencias
    int obtenidas = 0;

    printf ("Soy el hilo: %d estoy intentando tomar %d \n", id, numRand);
    for (int i=0; i < numRand; i++){
        //SI LOGRO DECREMENTAR LAS LICENCIAS, SIGNIFICA QUE OBTUVE UNA, SINO LOGRO, SIGNICA QUE NO PUEDO
        if (sem_trywait(&licencias) == 0) {
            printf ("Soy el hilo: %d obtuve %d \n", id, i+1);
            obtenidas++;
            
        }   else {
            //SI NO LOGRE DECREMENTAR DEVUELVO LAS QUE TOME
            printf ("Soy el hilo: %d no logre obtener mis licencias devuelvo %d \n", id, i+1);
            for (int i=0; i<obtenidas; i++){
                sem_post(&licencias);
            }
            break;  //corto el for
            }
    }

    if (obtenidas == numRand) {
        //Si pude conseguir las que obtuve, escribo y despues libero
        printf ("Soy el hilo: %d tome %d \n", id, obtenidas);
        pthread_mutex_lock(&BD);
            printf ("Soy el hilo: %d escribiendo en la base de datos \n", id);
        pthread_mutex_unlock(&BD);
        //una vez realizada la escritura, en la BD devuelvo las que obtuve
        for (int i=0; i < obtenidas; i++){
            sem_post(&licencias);
            printf ("Soy el hilo: %d devuelvo mis licencias %d \n", id, i+1);

        }
    }

    return NULL;
}
