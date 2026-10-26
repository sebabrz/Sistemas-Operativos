#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define NUM_PHILOSOPHERS 5
#define ITERATIONS 100

#define READ 0
#define WRITE 1

#define RED "\033[0;31m"
#define GREEN "\033[1;32m" 
#define ORANGE "\033[38;5;214m"
#define RESET "\033[0m"

int chopsticks[NUM_PHILOSOPHERS][2];
int pipeMutex[2];
char mensaje = 'a';

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


void philosophers(void *args) {
    int id = (int) args;

    int left_stick = id % NUM_PHILOSOPHERS;
    int right_stick = (id + 4) % NUM_PHILOSOPHERS;

    while (1) {
    
        printf(GREEN "Filosofo %d pensando... \n" RESET, id);
        fflush(stdout);
        read(pipeMutex[READ], &mensaje, sizeof(char));
        if (read(chopsticks[left_stick][READ], &mensaje, sizeof(char)) > 0) {
            if (read(chopsticks[right_stick][READ], &mensaje, sizeof(char)) > 0) {
                // Ambos palillos adquiridos con éxito
                printf(RED "Filosofo %d come (s:%d s:%d)\n"RESET, id, left_stick, right_stick);
                fflush(stdout);
                write(pipeMutex[WRITE], &mensaje, sizeof(char));
                sleep(1);
                printf(RED "Filosofo %d termina de comer (s:%d s:%d)\n"RESET, id, left_stick, right_stick);
                write(chopsticks[left_stick][WRITE], &mensaje, sizeof(char));
                write(chopsticks[right_stick][WRITE], &mensaje, sizeof(char));
            } else {
                // No se pudo adquirir el palillo derecho
                printf(ORANGE "Filosofo %d no pudo agarrar el palillo derecho (s:%d)\n" RESET, id, right_stick);
                fflush(stdout);
                write(chopsticks[left_stick][WRITE], &mensaje, sizeof(char));  // Devolver el palillo izquierdo
                write(pipeMutex[WRITE], &mensaje, sizeof(char));
            }
        } 
        else {
            // No se pudo adquirir el palillo izquierdo
            printf(ORANGE "Filosofo %d no pudo agarrar el palillo izquierdo (s:%d)\n" RESET, id, left_stick);
            fflush(stdout);
            write(pipeMutex[WRITE], &mensaje, sizeof(char));
            sleep(1);

        }
        sleep(3);

    }
    exit(0);
}

void main() {
    pid_t pids[NUM_PHILOSOPHERS];

    for(int i = 0; i < NUM_PHILOSOPHERS; i++){
        if (pipe(chopsticks[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        // PARA QUE FUNCIONE TODOS LOS PALITOS (PIPES) TIENEN QUE SER NO BLOQUEANTES (POR LA SIMULACION EDL TRYWAIT ENTONCES ES ESPERA OCUPADA PERO NO HAY OTRA FORMA)
        set_non_blocking(chopsticks[i][READ]);
        write(chopsticks[i][WRITE], &mensaje, sizeof(char));
    }
    pipe(pipeMutex);
    write(pipeMutex[WRITE], &mensaje, sizeof(char));
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            pids[i] = fork();
            if (pids[i] < 0) {
                perror("fork");
                exit(1);
            } else if (pids[i] == 0) {
               philosophers((void *) i);
            }
        }
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        close(chopsticks[i][READ]);
        close(chopsticks[i][WRITE]);
    }
    close(pipeMutex[READ]);
    close(pipeMutex[WRITE]);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        waitpid(pids[i], NULL, 0);
    }

      
}
