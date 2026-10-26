#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t agentSem;
sem_t tobacco, paper, match;
sem_t tobaccoSem, paperSem, matchSem;
sem_t isTobacco, isPaper, isMatch;
pthread_mutex_t mutex;

// Solución insipirada y propuesta por el libro "TheLittleBookOfSemaphores"
// Agrega el uso de pushers para que se pueda trabajar con los ingredientes.

void* agentA(void* arg) {
    while (1) {
        sem_wait(&agentSem);
        printf("\033[1;34mAgent A: Providing tobacco and paper.\033[0m\n");
        sem_post(&tobacco);
        sem_post(&paper);
    }
}

void* agentB(void* arg) {
    while (1) {
        sem_wait(&agentSem);
        printf("\033[1;35mAgent B: Providing paper and match.\033[0m\n");
        sem_post(&paper);
        sem_post(&match);
    }
}

void* agentC(void* arg) {
    while (1) {
        sem_wait(&agentSem);
        printf("\033[1;36mAgent C: Providing tobacco and match.\033[0m\n");
        sem_post(&tobacco);
        sem_post(&match);
    }
}

void* pusherA(void* arg) {
    while (1) {
        sem_wait(&tobacco);
        pthread_mutex_lock(&mutex);
        printf("\033[1;32mPusher A: Detected tobacco.\033[0m\n");

        if (sem_trywait(&isPaper) == 0) {
            printf("\033[1;33mPusher A: Found paper, releasing matchSem.\033[0m\n");
            sem_post(&matchSem);
        } else if (sem_trywait(&isMatch) == 0) {
            printf("\033[1;33mPusher A: Found match, releasing paperSem.\033[0m\n");
            sem_post(&paperSem);
        } else {
            printf("\033[1;33mPusher A: No pair found, storing tobacco.\033[0m\n");
            sem_post(&isTobacco);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void* pusherB(void* arg) {
    while (1) {
        sem_wait(&paper);
        pthread_mutex_lock(&mutex);
        printf("\033[1;32mPusher B: Detected paper.\033[0m\n");

        if (sem_trywait(&isTobacco) == 0) {
            printf("\033[1;33mPusher B: Found tobacco, releasing matchSem.\033[0m\n");
            sem_post(&matchSem);
        } else if (sem_trywait(&isMatch) == 0) {
            printf("\033[1;33mPusher B: Found match, releasing tobaccoSem.\033[0m\n");
            sem_post(&tobaccoSem);
        } else {
            printf("\033[1;33mPusher B: No pair found, storing paper.\033[0m\n");
            sem_post(&isPaper);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void* pusherC(void* arg) {
    while (1) {
        sem_wait(&match);
        pthread_mutex_lock(&mutex);
        printf("\033[1;32mPusher C: Detected match.\033[0m\n");

        if (sem_trywait(&isPaper) == 0) {
            printf("\033[1;33mPusher C: Found paper, releasing tobaccoSem.\033[0m\n");
            sem_post(&tobaccoSem);
        } else if (sem_trywait(&isTobacco) == 0) {
            printf("\033[1;33mPusher C: Found tobacco, releasing paperSem.\033[0m\n");
            sem_post(&paperSem);
        } else {
            printf("\033[1;33mPusher C: No pair found, storing match.\033[0m\n");
            sem_post(&isMatch);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void* smoker_with_matches(void* arg) {
    while (1) {
        sem_wait(&matchSem);
        printf("\033[1;37mSmoker with matches: Making cigarette.\033[0m\n");
        sem_post(&agentSem);
        printf("\033[1;37mSmoker with matches: Smoking.\033[0m\n");
        sleep(1); // Simulate smoking time
    }
}

void* smoker_with_tobacco(void* arg) {
    while (1) {
        sem_wait(&tobaccoSem);
        printf("\033[1;36mSmoker with tobacco: Making cigarette.\033[0m\n");
        sem_post(&agentSem);
        printf("\033[1;36mSmoker with tobacco: Smoking.\033[0m\n");
        sleep(1); // Simulate smoking time
    }
}

void* smoker_with_paper(void* arg) {
    while (1) {
        sem_wait(&paperSem);
        printf("\033[1;33mSmoker with paper: Making cigarette.\033[0m\n");
        sem_post(&agentSem);
        printf("\033[1;33mSmoker with paper: Smoking.\033[0m\n");
        sleep(1); // Simulate smoking time
    }
}

int main() {
    pthread_t agent1, agent2, agent3;
    pthread_t smoker1, smoker2, smoker3;
    pthread_t pusher1, pusher2, pusher3;

    sem_init(&agentSem, 0, 1);
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);
    sem_init(&tobaccoSem, 0, 0);
    sem_init(&paperSem, 0, 0);
    sem_init(&matchSem, 0, 0);
    sem_init(&isTobacco, 0, 0);
    sem_init(&isPaper, 0, 0);
    sem_init(&isMatch, 0, 0);

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&agent1, NULL, agentA, NULL);
    pthread_create(&agent2, NULL, agentB, NULL);
    pthread_create(&agent3, NULL, agentC, NULL);
    
    pthread_create(&pusher1, NULL, pusherA, NULL);
    pthread_create(&pusher2, NULL, pusherB, NULL);
    pthread_create(&pusher3, NULL, pusherC, NULL);

    pthread_create(&smoker1, NULL, smoker_with_tobacco, NULL);
    pthread_create(&smoker2, NULL, smoker_with_paper, NULL);
    pthread_create(&smoker3, NULL, smoker_with_matches, NULL);
    
    pthread_join(agent1, NULL);
    pthread_join(agent2, NULL);
    pthread_join(agent3, NULL);
    pthread_join(smoker1, NULL);
    pthread_join(smoker2, NULL);
    pthread_join(smoker3, NULL);

    sem_destroy(&tobacco);
    sem_destroy(&paper);
    sem_destroy(&match);
    sem_destroy(&tobaccoSem);
    sem_destroy(&paperSem);
    sem_destroy(&matchSem);
    sem_destroy(&isTobacco);
    sem_destroy(&isPaper);
    sem_destroy(&isMatch);
    sem_destroy(&agentSem);    

    pthread_mutex_destroy(&mutex);

    return 0;
}
