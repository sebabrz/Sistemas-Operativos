#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// AB(DvE)AC(DvE)F AB(DvE)AC(DvE)F AB(DvE)AC(DvE)F...

sem_t semA, semB, semC, semDvE, semF;

void* tareaA(void* arg) {
    while(1) {
        sem_wait(&semA);
        printf("A");
        fflush(stdout);
        usleep(10000);
        sem_post(&semB);
    }
}

void* tareaB(void* arg) {
    while(1) {
        sem_wait(&semB);
        printf("B");
        fflush(stdout);
        usleep(10000);
        sem_post(&semDvE);
        sem_wait(&semB);
        sem_post(&semC);
    }
}

void* tareaC(void* arg) {
    while(1) {
        sem_wait(&semC);
        printf("C");
        fflush(stdout);
        usleep(10000);
        sem_post(&semDvE);
    }
}

void* tareaD(void* arg) {
    while(1) {
        sem_wait(&semDvE);
        printf("D");
        fflush(stdout);
        usleep(10000);
        sem_post(&semF);
    }
}

void* tareaE(void* arg) {
    while(1) {
        sem_wait(&semDvE);
        printf("E");
        fflush(stdout);
        usleep(10000);
        sem_post(&semF);
    }
}

void* tareaF(void* arg) {
    while(1) {
        sem_wait(&semF);
        sem_post(&semA);
        sem_wait(&semF);
        printf("F\n");
        fflush(stdout);
        usleep(10000);
        sem_post(&semA);
    }
}

int main() {
    pthread_t threadA, threadB, threadC, threadD, threadE, threadF;

    sem_init(&semA, 0, 1);
    sem_init(&semB, 0, 0);
    sem_init(&semC, 0, 0);
    sem_init(&semDvE, 0, 0);
    sem_init(&semF, 0, 0);


    pthread_create(&threadA, NULL, tareaA, NULL);
    pthread_create(&threadB, NULL, tareaB, NULL);
    pthread_create(&threadC, NULL, tareaC, NULL);
    pthread_create(&threadD, NULL, tareaD, NULL);
    pthread_create(&threadE, NULL, tareaE, NULL);
    pthread_create(&threadF, NULL, tareaF, NULL);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    pthread_join(threadD, NULL);
    pthread_join(threadE, NULL);
    pthread_join(threadF, NULL);

    sem_destroy(&semA);
    sem_destroy(&semB);
    sem_destroy(&semC);
    sem_destroy(&semDvE);
    sem_destroy(&semF);

    return 0;
}