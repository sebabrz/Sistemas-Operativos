#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#define CANT 9

int sudoku[CANT][CANT];

int resultadosFilas[CANT];
int resultadosColumnas[CANT];
int resultadosBloques[CANT];

int validar(int valores[CANT]) {
    int aux[CANT + 1] = {0};

    for (int j = 0; j < CANT; j++) {
        if (aux[valores[j]] == valores[j]) {
            return 0;  // encontre un repetido, invalido
        } else {
            aux[valores[j]] = valores[j];
        }
    }
    return 1;  // recorri todo sin repetidos, valido
}

void *contarFilas(void *args) {
    for (int i = 0; i < CANT; i++) {
        int temp[CANT];
        for (int j = 0; j < CANT; j++) {
            temp[j] = sudoku[i][j];
        }
        resultadosFilas[i] = validar(temp);
    }
    return NULL;
}

void *contarColumnas(void *args) {
    for (int j = 0; j < CANT; j++) {
        int temp[CANT];
        for (int i = 0; i < CANT; i++) {
            temp[i] = sudoku[i][j];
        }
        resultadosColumnas[j] = validar(temp);
    }
    return NULL;
}

void *contarBloque(void *args) {
    int bloque = (intptr_t) args;

    // blockRow/blockCol: en que fila/columna de bloques (0,1,2) esta este bloque
    int blockRow = bloque / 3;
    int blockCol = bloque % 3;

    // filaInicio/colInicio: la celda real de la matriz donde arranca el bloque
    int filaInicio = blockRow * 3;
    int colInicio = blockCol * 3;

    int temp[CANT];
    int k = 0;
    for (int i = filaInicio; i < filaInicio + 3; i++) {
        for (int j = colInicio; j < colInicio + 3; j++) {
            temp[k] = sudoku[i][j];
            k++;
        }
    }

    resultadosBloques[bloque] = validar(temp);
    return NULL;
}

void generarMatriz() {
    for (int i = 0; i < CANT; i++) {
        for (int j = 0; j < CANT; j++) {
            sudoku[i][j] = random() % 9 + 1;
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
}

int main() {

    srandom(time(NULL));

    generarMatriz();
    fflush(stdout);

    pthread_t hiloFilas;
    pthread_t hiloColumnas;
    pthread_t hilosBloques[CANT];

    pthread_create(&hiloFilas, NULL, contarFilas, NULL);
    pthread_create(&hiloColumnas, NULL, contarColumnas, NULL);

    for (int i = 0; i < CANT; i++) {
        pthread_create(&hilosBloques[i], NULL, contarBloque, (void *)(intptr_t) i);
    }

    pthread_join(hiloFilas, NULL);
    pthread_join(hiloColumnas, NULL);
    for (int i = 0; i < CANT; i++) {
        pthread_join(hilosBloques[i], NULL);
    }

    int esValido = 1;

    for (int i = 0; i < CANT; i++) {
        if (resultadosFilas[i] == 0) {
            esValido = 0;
            break;
        }
    }

    if (esValido) {
        for (int j = 0; j < CANT; j++) {
            if (resultadosColumnas[j] == 0) {
                esValido = 0;
                break;
            }
        }
    }

    if (esValido) {
        for (int b = 0; b < CANT; b++) {
            if (resultadosBloques[b] == 0) {
                esValido = 0;
                break;
            }
        }
    }

    if (esValido) {
        printf("\nEl sudoku es VALIDO\n");
    } else {
        printf("\nEl sudoku NO es valido\n");
    }

    return 0;
}