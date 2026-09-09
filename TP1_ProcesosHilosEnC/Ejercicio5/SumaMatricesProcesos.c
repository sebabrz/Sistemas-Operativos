#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>

#define TAMANIO_COL 5
#define TAMANIO_FIL 5
#define CANT_PROCESOS 5

struct MatricesCompartida {
    int matrizA[TAMANIO_COL][TAMANIO_COL];
    int matrizB[TAMANIO_COL][TAMANIO_COL];
    int matrizC[TAMANIO_COL][TAMANIO_COL];
};

void generarMatrizA(struct MatricesCompartida *ptr) {
    for (int i = 0; i < TAMANIO_FIL; i++) {
        for (int j = 0; j < TAMANIO_COL; j++) {
            ptr->matrizA[i][j] = random() % 10;
            printf("%d ", ptr->matrizA[i][j]);
        }
        printf("\n");
    }
}

void generarMatrizB(struct MatricesCompartida *ptr) {
    for (int i = 0; i < TAMANIO_FIL; i++) {
        for (int j = 0; j < TAMANIO_COL; j++) {
            ptr->matrizB[i][j] = random() % 10;
            printf("%d ", ptr->matrizB[i][j]);
        }
        printf("\n");
    }
}

void imprimirMatrizC(struct MatricesCompartida *ptr) {
    for (int i = 0; i < TAMANIO_FIL; i++) {
        for (int j = 0; j < TAMANIO_COL; j++) {
            printf("%d ", ptr->matrizC[i][j]);
        }
        printf("\n");
    }
}

int main() {

srandom(time(NULL));

// shm_open crea un objeto de memoria compartida identificado por un nombre
// parametros: nombre del objeto ("/matrices_shm"), O_CREAT crea el objeto si no existe + O_RDWR permite leer y escribir, 0666 son los permisos de acceso. Devuelve un descriptor (fd), un "ticket" para referirse a este objeto despues
int fd = shm_open("/matrices_shm", O_CREAT | O_RDWR, 0666);

// ftruncate configura el tamanio (en bytes) que va a tener el objeto de memoria compartida, que arranca en 0 al crearse
// parametros: fd es el descriptor del objeto (el que devolvio shm_open), sizeof(struct MatricesCompartida) es cuantos bytes reservarle (el tamanio exacto de las tres matrices juntas)
ftruncate(fd, sizeof(struct MatricesCompartida));

// mmap mapea el objeto de memoria compartida al espacio de direcciones de este proceso, devolviendo un puntero utilizable como cualquier variable de C
// parametros: NULL deja que el sistema elija la direccion, sizeof(...) cuantos bytes mapear, PROT_READ|PROT_WRITE permite leer y escribir, MAP_SHARED hace que los cambios sean visibles para otros procesos que mapeen el mismo objeto, fd es el objeto a mapear, 0 es el offset (desde que byte arrancar, en este caso desde el principio)
struct MatricesCompartida *ptr = mmap(NULL, sizeof(struct MatricesCompartida),PROT_READ | PROT_WRITE, MAP_SHARED, fd, NULL);

    printf("MATRIZ A \n");
    generarMatrizA(ptr);

    printf("\n");

    printf("MATRIZ B \n");
    generarMatrizB(ptr);


    int pid;

    for (int i=0; i<CANT_PROCESOS; i++){
        pid = fork();

        if (pid == 0) {
            //Estoy en el proco "i" proceso hijo.
            //Tengo que sumar cada fila de matrizA + matriz B , y guardar la suma en la matrizC
            for (int j=0; j<TAMANIO_COL; j++){
                ptr->matrizC[i][j] = ptr->matrizA[i][j] + ptr->matrizB[i][j];
                //guardo el resultado en
            }

        break; //         //termino y paso a el otro hijo, asi no sigue creando el fork() hijos para abajo
        }
    }

    if (pid > 0 ) {
        wait(NULL);
        wait(NULL);
        wait(NULL);
        wait(NULL);
        wait(NULL);

        printf("\nMATRIZ C (resultado de A + B)\n");
        imprimirMatrizC(ptr);

        shm_unlink("/matrices_shm");

    }

    return 0;
}