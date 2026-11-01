/*SECUENCIA (b):  A CDE B CDE A CDE B CDE ...*/
sem_t a, b, c, d, e;//a inicializado en 1, los demas en 0
pthread_t ta, tb, tc, td, te;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&c);                 // despues de A viene C (no B)
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
        sem_wait(&e);                 // E del ciclo que empezo A
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);                 // ahora le toca a B

        sem_wait(&e);                 // E del ciclo que empezo B
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);                 // ahora le toca a A
    }
    return NULL;
}
