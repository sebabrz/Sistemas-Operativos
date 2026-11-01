/*SECUENCIA (f):  ABABABAC ABABABAC ...*/
sem_t a, b, c;//a inicializado en 1 los demas en 0
pthread_t ta, tb, tc;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&c);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
    }
    return NULL;
}