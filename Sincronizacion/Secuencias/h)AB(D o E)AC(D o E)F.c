/*SECUENCIA (h):  AB(D o E)AC(D o E)F AB(D o E)AC(D o E)F ...*/
sem_t a, b, c, de, f;//a=2, b=1 los demas en 0
pthread_t ta, tb, tc, td, te, tf;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&a);
        sem_wait(&a);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&b);
        sem_post(&c);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&b);
        sem_wait(&b);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&de);
    }
    return NULL;
}
void *imprimirC(void *arg){
    while(1){
        sem_wait(&c);
        sem_wait(&c);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&de);
    }
    return NULL;
}
void *imprimirD(void *arg){
    while(1){
        sem_wait(&de);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&f);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&de);
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
        sem_post(&f);
    }
    return NULL;
}
void *imprimirF(void *arg){
    while(1){
        sem_wait(&f);
        sem_wait(&f);
        printf("F"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&a);
    }
    return NULL;
}

int main(){
    pthread_t ta, tb, tc, td, te, tf;

    sem_init(&a, 0, 2);
    sem_init(&b, 0, 1);
    sem_init(&c, 0, 0);
    sem_init(&de, 0, 0);
    sem_init(&f, 0, 0);

    pthread_create(&ta, NULL, imprimirA, NULL);
    pthread_create(&tb, NULL, imprimirB, NULL);
    pthread_create(&tc, NULL, imprimirC, NULL);
    pthread_create(&td, NULL, imprimirD, NULL);
    pthread_create(&te, NULL, imprimirE, NULL);
    pthread_create(&tf, NULL, imprimirF, NULL);

    pthread_join(ta, NULL); pthread_join(tb, NULL); pthread_join(tc, NULL); pthread_join(td, NULL); pthread_join(te, NULL); pthread_join(tf, NULL);

    sem_destroy(&a); sem_destroy(&b); sem_destroy(&c); sem_destroy(&de); sem_destroy(&f);
    return 0;
}