#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TIME 1
#define USERS_C 10
#define USERS_V 3
#define ITER 20

#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RESET "\033[0m"

sem_t sem_prio;
sem_t sem_printer01;
sem_t sem_printer02;

pthread_mutex_t mtx_printer;
pthread_mutex_t mtx_user_c;
pthread_mutex_t mtx_user_v;

// metodo para ocupara una impresora
int wait_printer() {
    pthread_mutex_lock(&mtx_printer);
    int printer = 0;
    while (printer == 0) {
        if (sem_trywait(&sem_printer01) == 0) {
            // la impresora 01 se libero
            printer = 1;
        } else if (sem_trywait(&sem_printer02) == 0) {
            // la impresora 02 se libero
            printer = 2;
        }
    }
    pthread_mutex_unlock(&mtx_printer);
    return printer;
}

// metodo para liberar una impresora
void post_printer(int printer) {
    switch (printer) {
    case 1:
        sem_post(&sem_printer01);
        break;
    case 2:
        sem_post(&sem_printer02);
        break;
    }
}

// metodo para simular el uso de la impresora
void use_printer() {
    int time = rand()%10;
    sleep(time);
}

void *user_v(void *argv) {
    const char *color = YELLOW;
    
    for (int i = 0; i < ITER; i++) {
        sleep(TIME);

        // entrada
        pthread_mutex_lock(&mtx_user_v);
        
        // hay un usuario con prioridad esperando
        printf("%susuarioVIP quiere usar impresora%s\n", color, RESET);
        sem_post(&sem_prio);
        
        pthread_mutex_unlock(&mtx_user_v);

        // usuario espera por impresora
        int printer = wait_printer();
        printf("%susuario V reserva impresora[%d] %s\n", color, printer, RESET);

        use_printer();
        
        // usuario libera impresora
        post_printer(printer);
        printf("%susuario V libera impresora[%d] %s\n", color, printer, RESET);
        sem_post(&sem_prio);
    }
}


void *user_c(void *argv) {
    const char *color = PURPLE;
    
    for (int i = 0; i < ITER; i++) {
        sleep(TIME);

        // entrada
        pthread_mutex_lock(&mtx_user_c);
        // revisa si hay un usuario con prioridad esperando
        if (sem_trywait(&sem_prio) == 0) {
            printf("%susuario comun espera a usuarioVIP%s\n", color, RESET);
            sem_wait(&sem_prio);
        }
        pthread_mutex_unlock(&mtx_user_c);

        // usuario espera por impresora
        int printer = wait_printer();
        printf("%susuario C reserva impresora[%d] %s\n", color, printer, RESET);

        use_printer();
        
        // usuario libera impresora
        post_printer(printer);
        printf("%susuario C libera impresora[%d] %s\n", color, printer, RESET);
    }
}

void main() {
    pthread_t user_c_t[USERS_C];
    pthread_t user_v_t[USERS_V];

    sem_init(&sem_prio, 0, 0);
    sem_init(&sem_printer01, 0, 1);
    sem_init(&sem_printer02, 0, 1);

    srand(time(NULL) + getpid());

    for (int i = 0; i < USERS_C; i++) {
        pthread_create(&user_c_t[i], NULL, (void*)user_c, NULL);
    }

    for (int i = 0; i < USERS_V; i++) {
        pthread_create(&user_v_t[i], NULL, (void*)user_v, NULL);
    }

    for (int i = 0; i < USERS_C; i++) {
        pthread_join(user_c_t[i], NULL);
    }

    for (int i = 0; i < USERS_V; i++) {
        pthread_join(user_v_t[i], NULL);
    }

    sem_destroy(&sem_prio);
    sem_destroy(&sem_printer01);
    sem_destroy(&sem_printer02);
}