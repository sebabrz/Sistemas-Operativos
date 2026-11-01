/*SECUENCIA (c):  (A o B) C D E (A o B) C D E ...*/
sem_t ab, c, d, e;//ab inicializado en 1, los demas en 0
pthread_t ta, tb, tc, td, te;
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
        sem_post(&e);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&e);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&ab);
    }
    return NULL;
}