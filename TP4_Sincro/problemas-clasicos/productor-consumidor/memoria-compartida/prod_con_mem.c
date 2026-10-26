#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>

#define TIME 0
#define SEG_SIZE sizeof(struct message)
#define PRODUCERS 3
#define CONSUMERS 3
#define BUFFER_SIZE 20
#define BUFFER_LENGTH 100
#define ITER 200

#define BLUE "\033[0;34m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

struct message {
    char buffer[BUFFER_SIZE][BUFFER_LENGTH];
    int in;
    int out;
    sem_t sem_empty;
    sem_t sem_full; 
    sem_t mtx;
};

void producer (key_t key, int p_id) {
    // obtiene el identificador del segmento de memoria compartida
    int id = shmget(key, SEG_SIZE, 0);
    
    // mapea el segmento de memoria compartida
    struct message *info = (struct message *) shmat(id,0,0);

    if (info <= (struct message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
    
    for (int i = 0; i < ITER; i++) {
        sem_wait(&(info->sem_empty));
        sem_wait(&(info->mtx));

        // productor produce nuevo item
        char new_item[BUFFER_LENGTH];
        snprintf(new_item, BUFFER_LENGTH, "item [p:%d]", p_id);
        //sleep(TIME);

        // guarda item en buffer
        int prod_in = info->in;
        strcpy(info->buffer[prod_in], new_item);
        printf(BLUE "Productor (p:%d i:%d) genera: %s\n" RESET, p_id, prod_in, new_item);

        // actualiza indice de entrada
        info->in = (info->in + 1) % BUFFER_SIZE;

        sem_post(&(info->mtx));
        sem_post(&(info->sem_full));
    }

    shmdt(info);
}

void consumer (key_t key, int c_id) {
    // obtiene el identificador del segmento de memoria compartida
    int id = shmget(key, SEG_SIZE, 0);

    // mapea el segmento de memoria compartida
    struct message *info = (struct message *) shmat(id,0,0);
    
    if (info <= (struct message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }

    for (int i = 0; i < ITER; i++) {
        sem_wait(&(info->sem_full));
        sem_wait(&(info->mtx));

        //sleep(TIME);
        // consumidor obtiene item
        int cons_out = info->out;
        char item[BUFFER_LENGTH];
        strcpy(item, info->buffer[cons_out]);

        printf(RED "Consumidor (c:%d o:%d) consume: %s \n" RESET, c_id, cons_out, item);
        info -> out = (info->out + 1) % BUFFER_SIZE;

        sem_post(&(info->mtx));
        sem_post(&(info->sem_empty));
    }

    shmdt(info);
}

void main() {
    pid_t producer_t[PRODUCERS];
    pid_t consumer_t[CONSUMERS];

    // genera una clave unica para el segmento de memoria compartido
    key_t key = ftok("/key_t", 'A');

    // crea un segmento de memoria compartida
    int id = shmget(key, SEG_SIZE, IPC_CREAT | 0666);

    if (id < 0) {
        perror("Error al crear segmento de memoria compartida \n");
        exit(-1);
    }

    // mapea el segmento de memoria compartida
    struct message *msg = (struct message *) shmat(id,0,0);
    if (msg <= (struct message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }

    sem_init(&(msg->sem_empty), 1, BUFFER_SIZE);
    sem_init(&(msg->sem_full), 1, 0);
    sem_init(&(msg->mtx), 1, 1);
    msg->in = 0;
    msg->out = 0;

    // ---- procesos productor y consumidor ----

    for (int i = 0; i < PRODUCERS; i++) {
        producer_t[i] = fork();
        if (producer_t[i] == 0) {// producer process
            producer(key, i);
            exit(0);
        } else if (producer_t[i] < 0) { // paret process
            perror("Error al crear proceso productor \n");
        }
    }

    for (int i = 0; i < CONSUMERS; i++) {
        consumer_t[i] = fork();
        if (consumer_t[i] == 0) {
            consumer(key, i);
            exit(0);
        } else if (consumer_t[i] < 0) {
            perror("Error al crear proceso consumidor \n");
            exit(-1);
        }
    }

    for (int i = 0; i < PRODUCERS + CONSUMERS; i++) {
        wait(NULL);
    }

    sem_destroy(&(msg->sem_empty));
    sem_destroy(&(msg->sem_full));
    sem_destroy(&(msg->mtx));

    shmdt(msg);

    shmctl(id, IPC_RMID, NULL);
    exit(0);
}