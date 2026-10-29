#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Secuencia: A CDE B CDE AB CDE

sem_t semA, semB, semC, semD, semE, semAB, semBC;

void* tareaA(void* arg) {
    while(1) {
        sem_wait(&semA);
        printf("A");
        fflush(stdout);
        usleep(10000);
        sem_post(&semC);
        sem_wait(&semA);
        printf("A");
        fflush(stdout);
        usleep(10000);
        sem_post(&semAB);
    }
}

void* tareaB(void* arg) {
    while(1) {
        sem_wait(&semB);
        printf("B");
        fflush(stdout);
        usleep(10000);
        sem_post(&semBC);
        
        sem_wait(&semAB);
        printf("B");
        fflush(stdout);
        usleep(10000);
        sem_post(&semBC);
    }
}

void* tareaC(void* arg) {
    while(1) {
        sem_wait(&semC);
        printf("C");
        fflush(stdout);
        usleep(10000);
        sem_post(&semD);
        
        for(int i = 0; i < 2; i++) {
            sem_wait(&semBC);
            printf("C");
            fflush(stdout);
            usleep(10000);
            sem_post(&semD);
        }
    }
}

void* tareaD(void* arg) {
    while(1) {
        
        sem_wait(&semD);
        printf("D");
        fflush(stdout);
        usleep(10000);
        sem_post(&semE);

    }
}

void* tareaE(void* arg) {
    while(1) {
        sem_wait(&semE);
        printf("E");
        fflush(stdout);
        usleep(10000);
        sem_post(&semB);
        
        for(int i = 0; i < 2; i++) {
            sem_wait(&semE);
            printf("E");
            fflush(stdout);
            usleep(10000);
            sem_post(&semA);
        }
    }
}

int main() {
    pthread_t threadA, threadB, threadC, threadD, threadE;

    sem_init(&semA, 0, 1);
    sem_init(&semB, 0, 0);
    sem_init(&semC, 0, 0);
    sem_init(&semD, 0, 0);
    sem_init(&semE, 0, 0);
    sem_init(&semAB, 0, 0);
    sem_init(&semBC, 0, 0);

    pthread_create(&threadA, NULL, tareaA, NULL);
    pthread_create(&threadB, NULL, tareaB, NULL);
    pthread_create(&threadC, NULL, tareaC, NULL);
    pthread_create(&threadD, NULL, tareaD, NULL);
    pthread_create(&threadE, NULL, tareaE, NULL);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    pthread_join(threadD, NULL);
    pthread_join(threadE, NULL);

    sem_destroy(&semA);
    sem_destroy(&semB);
    sem_destroy(&semC);
    sem_destroy(&semD);
    sem_destroy(&semE);
    sem_destroy(&semAB);
    sem_destroy(&semBC);

    return 0;
}
