/*SECUENCIA (l):  AA B (C o D)  AA B (C o D) ...*/
sem_t a, b, cd;//a=2, los demas 0
pthread_t ta, tb, tc, td;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&cd);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&cd);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&a);
    }
    return NULL;
}
void *imprimirD(void *arg){
    while(1){
        sem_wait(&cd);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&a);
    }
    return NULL;
}