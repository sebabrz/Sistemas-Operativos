#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>


/*
    no escala bien para matrices grandes (crear miles de threads sería ineficiente)
*/

#define CANT 9

int matrizA[CANT][CANT];
int matrizB[CANT][CANT];
int matrizC[CANT][CANT];

void generarMatrizA() {
    for (int i = 0; i < CANT; i++) {
        for (int j = 0; j < CANT; j++) {
            matrizA[i][j] = random() % 10;
            printf("%d ", matrizA[i][j]);
        }
        printf("\n");
    }
}

void generarMatrizB() {
    for (int i = 0; i < CANT; i++) {
        for (int j = 0; j < CANT; j++) {
            matrizB[i][j] = random() % 10;
            printf("%d ", matrizB[i][j]);
        }
        printf("\n");
    }
}

void imprimirMatrizC() {
    for (int i = 0; i < CANT; i++) {
        for (int j = 0; j < CANT; j++) {
            printf("%d ", matrizC[i][j]);
        }
        printf("\n");
    }
}

void *multiplicacion (void *args) {
    int fila = (int) args;

    for (int j = 0; j < CANT; j++) {
        int suma = 0;
        for (int k = 0; k < CANT; k++) {
            suma = suma + (matrizA[fila][k] * matrizB[k][j]);
        }
        matrizC[fila][j] = suma;
    }
    return NULL;
}


int main() {

    srandom(time(NULL));

    printf("MATRIZ A \n");
    generarMatrizA();

    printf("\n");

    printf("MATRIZ B \n");
    generarMatrizB();

    printf("\n");
    pthread_t hilo[CANT];
    for (int i=0; i<CANT; i++){
        pthread_create (&hilo[i], NULL, &multiplicacion, (void*)(intptr_t)i);
    }

    for (int i=0; i<CANT; i++){
        pthread_join(hilo[i], NULL);
    }

    printf ("Matriz C resultante \n");
    imprimirMatrizC();

    return 0;
}