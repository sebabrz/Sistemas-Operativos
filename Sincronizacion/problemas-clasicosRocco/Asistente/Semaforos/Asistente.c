

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define TIME 0
#define ITER 100
#define ALUMNOS 10
#define SILLAS 3

#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RESET "\033[0m"

sem_t sem_sillas;
sem_t sem_aula;
sem_t sem_alumno;
pthread_mutex_t mtx;

void ayudar() {
    printf(PURPLE"Asistente ayudando a alumno\n"RESET);
    sleep(rand() % 5);
}

void alumno_se_va(int id) {
    printf(YELLOW"alumno %d se va \n"RESET, id);
    sleep(rand() % 10);
}

void alumno_llega(int id) {
    sleep(rand() % 10);
    printf(YELLOW"Alumno %d llega \n"RESET, id);
}

// ---- barbero ----

void *asistente(void *argv) {
    for (int i = 0; i < ITER; i++) {
        // barbero espera clientes
        pthread_mutex_lock(&mtx);
        if (sem_trywait(&sem_alumno) != 0) {
            pthread_mutex_unlock(&mtx);
            sem_wait(&sem_alumno);
            
            printf(PURPLE"Asistente se despierta \n"RESET);
            ayudar();

            sem_post(&sem_aula);
        // barbero corta pelo
    }else{
            pthread_mutex_unlock(&mtx);
            ayudar();
            sem_post(&sem_aula);
    }
    
}
    
    printf(PURPLE"ASistente TERMINO \n"RESET);

    pthread_exit(NULL);
}

// ---- cliente ----

void *alumno(void *argv) {
    int id = (int) argv;
    for (int i = 0; i < ITER; i++) {
        alumno_llega(id);
        // el cliente ve si hay sillas
        pthread_mutex_lock(&mtx);
        if (sem_trywait(&sem_sillas) == 0) {
            // si hay sillas, se sienta
            sem_post(&sem_alumno);
            pthread_mutex_unlock(&mtx);

            printf(YELLOW"alumno %d espera assitente... \n"RESET, id);
            sem_wait(&sem_aula); // espera assistente

            // assitente ayudando alumno
            
            sem_post(&sem_sillas);
        } else {
            // si no hay sillas, se va
            pthread_mutex_unlock(&mtx);
            alumno_se_va(id);
        }
        
        
    }

    printf(YELLOW"Alumno %d TERMINO \n"RESET, id);

    pthread_exit(NULL);
} 

void main() {
    pthread_t asistente_tid;
    pthread_t alumnos_tid[ALUMNOS];

    sem_init(&sem_sillas, 0, SILLAS);
    sem_init(&sem_alumno, 0, 0);
    sem_init(&sem_aula, 0, 1);
    pthread_mutex_init(&mtx, NULL);

    srand(time(NULL) + getpid());

    pthread_create(&asistente_tid, NULL, asistente, NULL);

    for (int i = 0; i < ALUMNOS; i++) {
        pthread_create(&alumnos_tid[i], NULL, alumno, (void *) i);
    }

    for (int i = 0; i < ALUMNOS; i++) {
        pthread_join(alumnos_tid[i], NULL);
    }

    pthread_join(asistente_tid, NULL);

    sem_destroy(&sem_sillas);
    sem_destroy(&sem_alumno);
    sem_destroy(&sem_aula);
    pthread_mutex_destroy(&mtx);
}
