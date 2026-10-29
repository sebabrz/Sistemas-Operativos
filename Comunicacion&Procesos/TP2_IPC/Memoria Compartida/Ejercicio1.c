#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, exit, random
#include <unistd.h> // fork, exec, getpid, sleep
#include <sys/wait.h> // wait, waitpid
#include <pthread.h> // hilos POSIX
#include <sys/types.h> // pid_t y otros tipos
#include <time.h> // time, medicion de tiempo
#include <stdint.h> // enteros de tamaño fijo
#include <sys/mman.h> // memoria compartida (mmap)
#include <fcntl.h> // flags de apertura (O_CREAT, etc.)
#include <sys/stat.h> // permisos de archivos
#include <sys/shm.h> // memoria compartida System V
#include <string.h> // manejo de strings
#include <sys/time.h> // gettimeofday
#include <math.h> // funciones matematicas
#include <dirent.h> // lectura de directorios

/*
    Memoria compartida para contar vocales: se crea un segmento con
    contadores por vocal. El padre recibe el nombre de un archivo de
    texto y crea procesos hijos que cuentan las vocales del texto; los
    resultados se acumulan en el segmento compartido y el padre
    muestra el total al finalizar.
*/

#define KEY (key_t)(1234)
#define SIZE sizeof(struct vocales)
#define CANT_VOCALES 5

struct vocales {
    int cant[CANT_VOCALES]; // 0=a, 1=e, 2=i, 3=o, 4=u
};

int main() {
    int id = shmget(KEY, SIZE, IPC_CREAT | 0666);

    struct vocales *ptr = (struct vocales *)shmat(id, 0, 0);

    char letras[CANT_VOCALES] = {'a', 'e', 'i', 'o', 'u'};
    pid_t pid;

    for (int i = 0; i < CANT_VOCALES; i++) {
        pid = fork();
        if (pid == 0) {
            // ===== soy uno de los 5 hijos, mi vocal es letras[i] =====
            int c;
            ptr->cant[i] = 0;

            FILE *archivo = fopen("vocales.txt", "r");

            while ((c = fgetc(archivo)) != EOF) {
                if ((char)c == letras[i]) {
                    ptr->cant[i]++;
                }
            }

            printf("Soy el proceso %d termine \n", i);
            fclose(archivo);
            exit(0);
        }
    }

    for (int i = 0; i < CANT_VOCALES; i++) {
        wait(NULL);
    }

    printf("Se encontraron: A: %d, E: %d, I: %d, O: %d, U: %d \n", ptr->cant[0], ptr->cant[1], ptr->cant[2], ptr->cant[3], ptr->cant[4]);

    shmctl(id, IPC_RMID, NULL);

    return 0;
}