#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define TIME 0
#define ITER 25
#define CLIENTES 10
#define SILLAS 5

#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RESET "\033[0m"

sem_t sem_sillas;
sem_t sem_bar;
sem_t sem_cli;
pthread_mutex_t mtx_cli;

void cortar_pelo() {
    printf(PURPLE"Cortando pelo \n"RESET);
    sleep(rand() % 5);
}

void cliente_se_va(int id) {
    printf(YELLOW"Cliente %d se va \n"RESET, id);
    sleep(rand() % 10);
}

void cliente_llega(int id) {
    sleep(rand() % 10);
    printf(YELLOW"Cliente %d llega \n"RESET, id);
}

// ---- barbero ----

void *barbero(void *argv) {
    for (int i = 0; i < ITER; i++) {
        // barbero espera clientes
        printf(PURPLE"Barbero durmiendo... \n"RESET);
        sem_wait(&sem_cli);
        printf(PURPLE"Barbero se despierta \n"RESET);

        // barbero corta pelo
        cortar_pelo();

        sem_post(&sem_bar);
    }
    printf(PURPLE"BARBERO TERMINO \n"RESET);

    pthread_exit(NULL);
}

// ---- cliente ----

void *cliente(void *argv) {
    int id = (int) argv;
    for (int i = 0; i < ITER; i++) {
        cliente_llega(id);
        // el cliente ve si hay sillas
        pthread_mutex_lock(&mtx_cli);
        if (sem_trywait(&sem_sillas) == 0) {
            // si hay sillas, se sienta
            sem_post(&sem_cli);
            pthread_mutex_unlock(&mtx_cli);

            printf(YELLOW"Cliente %d espera barbero... \n"RESET, id);
            sem_wait(&sem_bar); // espera barbero

            // cliente se corta el pelo
            
            sem_post(&sem_sillas);
        } else {
            // si no hay sillas, se va
            pthread_mutex_unlock(&mtx_cli);
            cliente_se_va(id);
        }
    }

    printf(YELLOW"CLIENTE %d TERMINO \n"RESET, id);

    pthread_exit(NULL);
} 

void main() {
    pthread_t barbero_tid;
    pthread_t clientes_tid[CLIENTES];

    sem_init(&sem_sillas, 0, SILLAS);
    sem_init(&sem_cli, 0, 0);
    sem_init(&sem_bar, 0, 1);
    pthread_mutex_init(&mtx_cli, NULL);

    srand(time(NULL) + getpid());

    pthread_create(&barbero_tid, NULL, barbero, NULL);

    for (int i = 0; i < CLIENTES; i++) {
        pthread_create(&clientes_tid[i], NULL, cliente, (void *) i);
    }

    for (int i = 0; i < CLIENTES; i++) {
        pthread_join(clientes_tid[i], NULL);
    }

    pthread_join(barbero_tid, NULL);

    sem_destroy(&sem_sillas);
    sem_destroy(&sem_cli);
    sem_destroy(&sem_bar);
    pthread_mutex_destroy(&mtx_cli);
}