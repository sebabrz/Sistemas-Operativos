#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define REPETITIONS 5
int ox_vacio[2], ox_lleno[2], hid_vacio[2], hid_lleno[2];
int mutex_ox[2], mutex_hid[2];
char mensaje = 'X';

void* gen_oxigeno() {
    close(hid_vacio[PIPE_WRITE]);
    close(hid_vacio[PIPE_READ]);
    close(hid_lleno[PIPE_WRITE]);
    close(hid_lleno[PIPE_READ]);
    close(mutex_hid[PIPE_WRITE]);
    close(mutex_hid[PIPE_READ]);
    close(ox_vacio[PIPE_WRITE]);
    close(ox_lleno[PIPE_READ]);
    
    for(int i = 0; i<REPETITIONS; i++){
        read(ox_vacio[PIPE_READ], &mensaje, sizeof(char));
        read(mutex_ox[PIPE_READ], &mensaje, sizeof(char));

        printf("Generé Oxígeno\n");
        fflush(stdout);

        write(mutex_ox[PIPE_WRITE], &mensaje, sizeof(char));
        write(ox_lleno[PIPE_WRITE], &mensaje, sizeof(char));
    }

    return NULL;
}

void* gen_hidrogeno() {
    close(ox_vacio[PIPE_WRITE]);
    close(ox_vacio[PIPE_READ]);
    close(ox_lleno[PIPE_WRITE]);
    close(ox_lleno[PIPE_READ]);
    close(mutex_ox[PIPE_WRITE]);
    close(mutex_ox[PIPE_READ]);
    close(hid_vacio[PIPE_WRITE]);
    close(hid_lleno[PIPE_READ]);

    for(int i = 0; i<REPETITIONS; i++){
        read(hid_vacio[PIPE_READ], &mensaje, sizeof(char));
        read(mutex_hid[PIPE_READ], &mensaje, sizeof(char));

        printf("Generé Hidrógeno\n");
        fflush(stdout);

        write(mutex_hid[PIPE_WRITE], &mensaje, sizeof(char));
        write(hid_lleno[PIPE_WRITE], &mensaje, sizeof(char));
    }
    return NULL;
}

void* gen_agua() {
    close(ox_vacio[PIPE_READ]);
    close(ox_lleno[PIPE_WRITE]);
    close(hid_vacio[PIPE_READ]);
    close(hid_lleno[PIPE_WRITE]);
    for(int i = 0; i<REPETITIONS; i++){
        // Esperar por un oxígeno
        read(ox_lleno[PIPE_READ], &mensaje, sizeof(char));
        read(mutex_ox[PIPE_READ], &mensaje, sizeof(char));
        printf("Tomé un oxígeno\n");
        fflush(stdout);
        write(mutex_ox[PIPE_WRITE], &mensaje, sizeof(char));
        write(ox_vacio[PIPE_WRITE], &mensaje, sizeof(char));

        // Esperar por dos hidrógenos
        for (int j = 0; j < 2; ++j) {
            read(hid_lleno[PIPE_READ], &mensaje, sizeof(char));
            read(mutex_hid[PIPE_READ], &mensaje, sizeof(char));
            printf("Tomé un hidrógeno\n");
            fflush(stdout);
            write(mutex_hid[PIPE_WRITE], &mensaje, sizeof(char));
            write(hid_vacio[PIPE_WRITE], &mensaje, sizeof(char));
        }

        printf("Se generó una molécula de agua!!\n");
        fflush(stdout);
    }
    return NULL;
}

int main() {
    // Crear los pipes con control de errores
    if (pipe(ox_vacio) == -1 || pipe(ox_lleno) == -1 || 
        pipe(hid_vacio) == -1 || pipe(hid_lleno) == -1 || 
        pipe(mutex_ox) == -1 || pipe(mutex_hid) == -1) {
        perror("Error creando pipes");
        exit(EXIT_FAILURE);
    }

    // Inicializar semáforos
    write(mutex_hid[PIPE_WRITE], &mensaje, sizeof(char));
    write(mutex_ox[PIPE_WRITE], &mensaje, sizeof(char));

    // Inicializar átomos disponibles
    write(ox_vacio[PIPE_WRITE], &mensaje, sizeof(char));
    write(hid_vacio[PIPE_WRITE], &mensaje, sizeof(char));
    write(hid_vacio[PIPE_WRITE], &mensaje, sizeof(char));

    pid_t pid_ox = fork();
    if (pid_ox == -1) {
        perror("Error al crear proceso ox");
        exit(EXIT_FAILURE);
    }
    if (pid_ox == 0) {
        gen_oxigeno();
        exit(0);
    }

    pid_t pid_hid1 = fork();
    if (pid_hid1 == -1) {
        perror("Error al crear proceso hid1");
        exit(EXIT_FAILURE);
    }
    if (pid_hid1 == 0) {
        gen_hidrogeno();
        exit(0);
    }

  
    // Esperar a que se generen todos los átomos antes de crear el proceso de agua
    sleep(1);

    pid_t pid_agua = fork();
    if (pid_agua == -1) {
        perror("Error al crear proceso genagua");
        exit(EXIT_FAILURE);
    }
    if (pid_agua == 0) {
        gen_agua();
        exit(0);
    }

    // Cerrar todos los pipes en el proceso padre
    close(ox_vacio[PIPE_WRITE]);
    close(ox_vacio[PIPE_READ]);
    close(ox_lleno[PIPE_WRITE]);
    close(ox_lleno[PIPE_READ]);
    close(mutex_ox[PIPE_WRITE]);
    close(mutex_ox[PIPE_READ]);
    close(mutex_hid[PIPE_WRITE]);
    close(mutex_hid[PIPE_READ]);
    close(hid_vacio[PIPE_WRITE]);
    close(hid_vacio[PIPE_READ]);
    close(hid_lleno[PIPE_WRITE]);
    close(hid_lleno[PIPE_READ]);

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }

    return 0;
}
