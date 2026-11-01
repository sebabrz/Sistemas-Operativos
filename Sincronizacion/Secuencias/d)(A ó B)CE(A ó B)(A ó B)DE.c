/*SECUENCIA (d):  (A o B) C E (A o B)(A o B) D E (A o B) C E (A o B)(A o B) D E ...*/
sem_t ab, fin, turnoC, turnoD, e;//ab inicializado en 1, y turnoC inicializado en 1, los demas en 0
pthread_t a,b,c,d,e;
void *imprimirA(void *arg){
    while(1){
        sem_wait(&ab);
        printf("A"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&fin);
    }
    return NULL;
}
void *imprimirB(void *arg){
    while(1){
        sem_wait(&ab);
        printf("B"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&fin);
    }
    return NULL;
}
void *imprimirC(void *arg){            // espera 1 A o B
    while(1){
        sem_wait(&turnoC);
        sem_wait(&fin);
        printf("C"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&e);
    }
    return NULL;
}
void *imprimirD(void *arg){            // espera 2 A o B
    while(1){
        sem_wait(&turnoD);
        sem_wait(&fin);
        sem_wait(&fin);
        printf("D"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&e);
    }
    return NULL;
}
void *imprimirE(void *arg){
    while(1){
        sem_wait(&e);                   // E que viene despues de C
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&turnoD);
        sem_post(&ab); sem_post(&ab);   // habilita los dos A o B seguidos

        sem_wait(&e);                   // E que viene despues de D
        printf("E"); fflush(stdout); usleep(PAUSA_US);
        sem_post(&turnoC);
        sem_post(&ab);                  // habilita el A o B que abre el ciclo
    }
    return NULL;
}