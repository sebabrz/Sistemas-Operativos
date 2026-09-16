#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>

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

void multiplicar () {
    for (int i=0;i<CANT;i++){
        for (int j=0 ; j<CANT; j++) {       
            matrizC[i][j] = matrizA[i][j] * matrizB[j][i];
        }
    }
    
}


int main() {

    srandom(time(NULL));

    printf("MATRIZ A \n");
    generarMatrizA();

    printf("\n");

    printf("MATRIZ B \n");
    generarMatrizB();

    printf("\n");

    int resultado;
    int pipes[2];
    pipe(pipes);
    pid_t proceso=fork();
    if(proceso==0){
        close(pipes[0]);
        multiplicar();
        write(pipes[1], matrizC, sizeof(matrizC)); // mandamos LA MATRIZ ENTERA
        close(pipes[1]);
        exit(0);
    }else if (proceso>0){
        close(pipes[1]);
        wait(NULL);
        read(pipes[0], matrizC, sizeof(matrizC)); // pisamos la matrizC del padre con la del hijo
        close(pipes[0]);
    }

    printf ("Matriz C resultante \n");
    imprimirMatrizC();

    return 0;
}