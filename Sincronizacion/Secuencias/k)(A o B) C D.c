/*SECUENCIA (k):  (A o B) C D (A o B) C D (A o B) C D ...*/
sem_t ab, c, d;//ab=1, los demas 0
pthread_t ta, tb, tc, td;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&ab);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&ab);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&c);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&d);
    }
    return NULL;
}
void *imprimirD(void *arg){
    while(1){
        sem_wait(&d);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&ab);
    }
    return NULL;
}