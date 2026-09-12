#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define CANT 9
    
int sudoku[CANT][CANT];

int resultadosFilas[CANT];
int resultadosColumnas[CANT];
int resultadosBloques[CANT];

int validar(int valores[cant]) {
    int aux[cant+1];
    for (int k = 0; k <= cant; k++) {
        aux[k] = 0;
    }

    for (int j = 0; j < cant; j++) {
        if (aux[valores[j]] == valores[j]) {
            return 0;  // encontre un repetido, invalido
        } else {
            aux[valores[j]] = valores[j];
        }
    }
    return 1;  // recorri todo sin repetidos, valido
}

void *contarFila(*void args){

    int fila = (int) args;


    for (int i=0; i<CANT; i++){
        resultadosFilas [i] = 


    }

}

void *contarColumna(*void args){

}

void *contarBloque(*void args){

}



//void *contarColumnas(*void args);

//void *contarBloque(*void args);

void generarMatriz(struct Sudoku *ptr){

    for (int i=0; i<CANT; i++){
        for (int j=0; j<CANT; j++){
            sudoku[i][j] = random() % 9 + 1;
            printf ("%d ", sudoku[i][j]);
        }
        printf ("\n");
    }
}
 
int main() {

    srandom(time(NULL));


    generarMatriz();

    pthread_t hilosFila;
    pthread_t hiloBloque;
    pthread_t hilosBloque[10]; //arreglo de 11 hilos.

    //Un hilo para las columnas

    for (int i=0; i<CANT; i++) {
        pthread_create(&hilo[0], NULL, &contarFila, i );
    }
    
    //Un hilo para las filas

    //Nueve(9) hilos para los bloques 

    for (int i=0; i<CANT; i++){
        pthread_create();

    }



    return 0;
}