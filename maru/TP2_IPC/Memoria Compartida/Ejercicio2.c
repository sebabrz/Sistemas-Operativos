#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TAMANIO_COL 9
#define TAMANIO_FIL 9
#define CANT_PROCESOS 9
#define KEY ((key_t)(1243))

/*
    Suma paralela de dos matrices de 9x9 utilizando procesos y un
    segmento de memoria compartida para almacenar la matriz resultante.
*/

struct MatricesCompartida {
    int matrizA[TAMANIO_COL][TAMANIO_COL];
    int matrizB[TAMANIO_COL][TAMANIO_COL];
    int matrizC[TAMANIO_COL][TAMANIO_COL];
};

void generarMatrizA(struct MatricesCompartida *ptr)
{
    for (int i = 0; i < TAMANIO_FIL; i++) {
        for (int j = 0; j < TAMANIO_COL; j++) {
            ptr->matrizA[i][j] = random() % 10;
            printf("%d ", ptr->matrizA[i][j]);
        }
        printf("\n");
    }
}

void generarMatrizB(struct MatricesCompartida *ptr)
{
    for (int i = 0; i < TAMANIO_FIL; i++) {
        for (int j = 0; j < TAMANIO_COL; j++) {
            ptr->matrizB[i][j] = random() % 10;
            printf("%d ", ptr->matrizB[i][j]);
        }
        printf("\n");
    }
}

void imprimirMatrizC(struct MatricesCompartida *ptr)
{
    for (int i = 0; i < TAMANIO_FIL; i++) {
        for (int j = 0; j < TAMANIO_COL; j++) {
            printf("%d ", ptr->matrizC[i][j]);
        }
        printf("\n");
    }
}

int main()
{

    srandom(time(NULL));

    // shmget crea (o se asocia a) el segmento de memoria compartida
    // parametros: KEY identifica el segmento, sizeof(...) es el tamaño en bytes,
    // IPC_CREAT crea el segmento si no existe, 0666 son los permisos
    int shmid = shmget(KEY, sizeof(struct MatricesCompartida), IPC_CREAT | 0666);

    if (shmid == -1) {
        printf("Error al crear el segmento de memoria\n");
        exit(1);
    }

    // shmat mapea el segmento al espacio de direcciones de este proceso
    // parametros: shmid es el segmento a mapear, 0 deja que el sistema elija la direccion,
    // 0 en shmflg significa lectura y escritura
    struct MatricesCompartida *ptr = (struct MatricesCompartida *)shmat(shmid, 0, 0);

    if (ptr == (void *)-1) {
        printf("Error al vincular el segmento de memoria\n");
        exit(2);
    }

    printf("MATRIZ A \n");
    generarMatrizA(ptr);

    printf("\n");

    printf("MATRIZ B \n");
    generarMatrizB(ptr);

    int pid;

    for (int i = 0; i < CANT_PROCESOS; i++) {
        pid = fork();

        if (pid == 0) {
            // Estoy en el proceso "i" hijo.
            // Tengo que sumar cada fila de matrizA + matrizB, y guardar la suma en matrizC
            for (int j = 0; j < TAMANIO_COL; j++) {
                ptr->matrizC[i][j] = ptr->matrizA[i][j] + ptr->matrizB[i][j];
            }

            shmdt(ptr); // el hijo se desvincula del segmento antes de terminar
            exit(0);
        }
    }

    if (pid > 0) {
        for (int i = 0; i < CANT_PROCESOS; i++) {
            wait(NULL);
        }

        printf("\nMATRIZ C (resultado de A + B)\n");
        imprimirMatrizC(ptr);

        shmdt(ptr); // el padre se desvincula
        shmctl(shmid, IPC_RMID, NULL); // se elimina el segmento del sistema
    }

    return 0;
}