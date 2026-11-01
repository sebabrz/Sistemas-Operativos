/*SECUENCIA (e):  ABABC...(se repite)*/
sem_t a, b, c;//a inicializado en 1, los demas en 0
pthread_t ta, tb, tc;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
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

int main(){
    pthread_t ta, tb, tc;

    sem_init(&a, 0, 1);
    sem_init(&b, 0, 0);
    sem_init(&c, 0, 0);

    pthread_create(&ta, NULL, imprimirA, NULL);
    pthread_create(&tb, NULL, imprimirB, NULL);
    pthread_create(&tc, NULL, imprimirC, NULL);

    pthread_join(ta, NULL); pthread_join(tb, NULL); pthread_join(tc, NULL);

    sem_destroy(&a); sem_destroy(&b); sem_destroy(&c);
    return 0;
}