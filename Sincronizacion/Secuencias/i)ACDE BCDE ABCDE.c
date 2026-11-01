/*SECUENCIA (i):  A CDE B CDE AB CDE  A CDE B CDE AB CDE ...*/
sem_t a, b, c, d, e;//a=1, los demas 0
pthread_t ta, tb, tc, td, te;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);
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
        sem_post(&e);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&e);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_wait(&e);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_wait(&e);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
    }
    return NULL;
}