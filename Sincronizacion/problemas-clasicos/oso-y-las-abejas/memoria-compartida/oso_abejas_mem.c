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

#define TIME 1
#define ITER 25
#define PORCIONES 10
#define ABEJAS 3
#define SEG_SIZE sizeof(struct shared_data)

struct shared_data {
    sem_t sem_tarro_vacio;
    sem_t sem_tarro_lleno;
    sem_t sem_oso;
    sem_t sem_abejas;
    sem_t mtx_abejas;
};

void abeja(key_t key) {
    ///////////////////////////////////////////////////////////
    int id = shmget(key, SEG_SIZE, 0);
    if (id < 0) {
        perror("Error al obtener segmento de memoria compartida \n");
        exit(-1);
    }
    struct shared_data *data = (struct shared_data *) shmat(id,0,0);
    if (data <= (struct shared_data *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
    ///////////////////////////////////////////////////////////

    for (int i = 0; i < ITER; i++) {
        sleep(TIME);

        // abeja produce miel
        sem_wait(&(data->sem_abejas));
        sem_wait(&(data->mtx_abejas));
        sem_wait(&(data->sem_tarro_vacio));

        if (sem_trywait(&(data->sem_tarro_vacio)) == 0) {
            // no soy ultima abeja
            sem_post(&(data->sem_tarro_vacio));
            sem_post(&(data->sem_abejas));
        } else {
            // soy ultima abeja
            printf("\033[1;33mAbeja ULTIMA\033[0m\n");
            sem_post(&(data->sem_oso));        
        }

        // guardo porcion miel
        printf("\033[1;33mAbeja GUARDA miel\033[0m\n");
        sem_post(&(data->sem_tarro_lleno));

        sem_post(&(data->mtx_abejas));
    }

    shmdt(data);
}

void oso(key_t key) {
    ///////////////////////////////////////////////////////////
    int id = shmget(key, SEG_SIZE, 0);
    if (id < 0) {
        perror("Error al obtener segmento de memoria compartida \n");
        exit(-1);
    }
    struct shared_data *data = (struct shared_data *) shmat(id,0,0);
    if (data <= (struct shared_data *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
    ///////////////////////////////////////////////////////////

    for (int i = 0; i < ITER; i++) {
        sleep(TIM   E);

        // oso duerme
        sem_wait(&(data->sem_oso));
        printf("\033[1;31mUN OSO WACHO\033[0m\n");

        for (int i = 0; i < PORCIONES; i++) {
            sem_wait(&(data->sem_tarro_lleno));
            // oso come miel
            printf("\033[0;33mOso COME miel\033[0m\n");
            sem_post(&(data->sem_tarro_vacio));
        }
        sem_post(&(data->sem_tarro_lleno));
        sem_post(&(data->sem_abejas));
    }
    shmdt(data);
}

void main() {
    ///////////////////////////////////////////////////////////
    pid_t abejas_t[ABEJAS];
    pid_t oso_t;

    key_t key = ftok("/tmp", 'A');

    int id = shmget(key, SEG_SIZE, IPC_CREAT | 0666);

    if (id < 0) {
        perror("Error al crear segmento de memoria compartida \n");
        exit(-1);
    }

    struct shared_data *data = (struct shared_data *) shmat(id,0,0);
    if (data <= (struct shared_data *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
    ///////////////////////////////////////////////////////////

    sem_init(&(data->mtx_abejas), 1, 1);
    sem_init(&(data->sem_tarro_vacio), 1, PORCIONES);
    sem_init(&(data->sem_tarro_lleno), 1, 0);
    sem_init(&(data->sem_oso), 1, 0);
    sem_init(&(data->sem_abejas), 1, 1);
    
    // ------ proceso oso ------

    oso_t = fork();

    if (oso_t == 0) {
        oso(key);
        exit(0);
    } else if (oso_t < 0) {
        perror("Error al crear proceso oso \n");
        exit(-1);
    }

    // ------ proceso abejas ------

    for (int i = 0; i < ABEJAS; i++) {
        abejas_t[i] = fork();
        if (abejas_t[i] == 0) {
            abeja(key);
            exit(0);
        } else if (abejas_t[i] < 0) {
            perror("Error al crear proceso abeja \n");
            exit(-1);
        }
    }
    
    for (int i = 0; i < ABEJAS + 1; i++) {
        wait(NULL);
    }

    sem_destroy(&(data->sem_tarro_vacio));
    sem_destroy(&(data->sem_tarro_lleno));
    sem_destroy(&(data->sem_oso));
    sem_destroy(&(data->sem_abejas));
    sem_destroy(&(data->mtx_abejas));

    shmdt(data);
    shmctl(id, IPC_RMID, NULL);
}