#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>


#define cant 5

int matrizA[cant][cant];
int matrizB[cant][cant];
int matrizC[cant][cant];

void generarMatrizA() {
    for (int i = 0; i < cant; i++) {
        for (int j = 0; j < cant; j++) {
            matrizA[i][j] = random() % 10;
            printf("%d ", matrizA[i][j]);
        }
        printf("\n");
    }
}

void generarMatrizB() {
    for (int i = 0; i < cant; i++) {
        for (int j = 0; j < cant; j++) {
            matrizB[i][j] = random() % 10;
            printf("%d ", matrizB[i][j]);
        }
        printf("\n");
    }
}

void imprimirMatrizC() {
    for (int i = 0; i < cant; i++) {
        for (int j = 0; j < cant; j++) {
            printf("%d ", matrizC[i][j]);
        }
        printf("\n");
    }
}

// Rutina que ejecuta cada hilo: suma una fila completa de matrizA + matrizB,
// guardando el resultado en la misma fila de matrizC.
// pthread_create() exige que esta funcion tenga la firma void *funcion(void *arg):
// recibe y devuelve un puntero generico (void *), sin importar que la funcion
// en si no trabaje con punteros reales.

void *sumarComponentes (void *args){

    // args viaja como void * (siempre [man pthread_create]), pero en realidad adentro tiene empaquetado
    // el numero de fila (un int) que le corresponde a este hilo.
    // int es el tipo que necesito, por eso hago el casteo.

    int fila = (int) args;
    for (int j=0 ; j<cant; j++){
        matrizC[fila][j] = matrizA[fila][j] + matrizB[fila][j];
    }
}


int main() {

    srandom(time(NULL));

    printf("MATRIZ A \n");
    generarMatrizA();

    printf("\n");

    printf("MATRIZ B \n");
    generarMatrizB();;

    pthread_t hilo[cant];   //Se declara un arreglo de Hilos

    for (int i=0; i<cant; i++){
        pthread_create(&hilo[i], NULL, &sumarComponentes, (void *) i); //el cuarto dato es el que se le entrega a la funcion
    }

     for (int i=0; i<cant; i++){
        pthread_join(hilo[i], NULL);        // bloquea al hilo que lo llama, 
                                            // el hilo principal, dentro de main()) hasta que 
                                            // el hilo especificado (hilo[i]) termine su ejecución
    }

    printf("MATRIZ C (resultado de A + B)\n");
    imprimirMatrizC();


    return 0;
}