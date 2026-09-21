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

struct vocales {
    int cant_a;
    int cant_e;
    int cant_i;
    int cant_o;
    int cant_u;
};
/*
    argc (argument count) → cuántos argumentos se pasaron, contando el nombre del programa.
    argv (argument vector) → array de strings con esos argumentos. argv[0] siempre es el nombre del ejecutable, 
    argv[1] es el primer argumento real (tu archivo), etc.
    esto es para el txt que me van a pasar al ejecutar ./programa archivo.txt, el fopen hace fopen(argv[1], "modo")
*/


int main(int argc, char *argv[])
{
    struct vocales *ptr;
    int id = shmget(KEY, SIZE, IPC_CREAT | 0666);
    int c = 0;

    if (id < 0) {
        printf("fallo el shmget");
        exit(2);
    }

    ptr = (struct vocales *)shmat(id, 0, 0);

    FILE *archivo;

    pid_t vocalA;
    pid_t vocalE;
    pid_t vocalI;
    pid_t vocalO;
    pid_t vocalU;

    vocalA = fork();

    if (vocalA == 0) {
        ptr->cant_a = 0;
        archivo = fopen(argv[1], "r"); //ejemplo pasando como argumento
        while ((c = fgetc(archivo)) != EOF) {
            if ((char)c == 'a') {
                ptr->cant_a++;
            }
        }
        if (shmdt(ptr) == -1) {
            perror("shmdt");
            exit(1);
        }
        exit(0);
    } else {
        vocalE = fork();

        if (vocalE == 0) {
            ptr->cant_e = 0;
            archivo = fopen("texto.txt", "r");
            while ((c = fgetc(archivo)) != EOF) {
                if ((char)c == 'e') {
                    ptr->cant_e++;
                }
            }
            if (shmdt(ptr) == -1) {
                perror("shmdt");
                exit(1);
            }
            exit(0);
        } else {
            vocalI = fork();

            if (vocalI == 0) {
                ptr->cant_i = 0;
                archivo = fopen("texto.txt", "r");
                while ((c = fgetc(archivo)) != EOF) {
                    if ((char)c == 'i') {
                        ptr->cant_i++;
                    }
                }
                if (shmdt(ptr) == -1) {
                    perror("shmdt");
                    exit(1);
                }
                exit(0);
            } else {
                vocalO = fork();

                if (vocalO == 0) {
                    ptr->cant_o = 0;
                    archivo = fopen("texto.txt", "r");
                    while ((c = fgetc(archivo)) != EOF) {
                        if ((char)c == 'o') {
                            ptr->cant_o++;
                        }
                    }
                    if (shmdt(ptr) == -1) {
                        perror("shmdt");
                        exit(1);
                    }
                    exit(0);
                } else {
                    vocalU = fork();

                    if (vocalU == 0) {
                        ptr->cant_u = 0;
                        archivo = fopen("texto.txt", "r");
                        while ((c = fgetc(archivo)) != EOF) {
                            if ((char)c == 'u') {
                                ptr->cant_u++;
                            }
                        }
                        if (shmdt(ptr) == -1) {
                            perror("shmdt");
                            exit(1);
                        }
                        exit(0);
                    }
                }
            }
        }
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("Se encontraro: A: %d, E: %d, I: %d, O: %d, U: %d \n", ptr->cant_a, ptr->cant_e, ptr->cant_i, ptr->cant_o, ptr->cant_u);

    if (shmdt(ptr) == -1) {
        perror("shmdt");
        exit(1);
    }
    shmctl(id, IPC_RMID, NULL);

    return 0;
}