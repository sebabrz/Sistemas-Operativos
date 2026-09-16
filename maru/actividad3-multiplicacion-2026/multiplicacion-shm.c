#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>

#define CANT 9
#define SIZE sizeof(struct info)
#define KEY ((key_t) (1243))

int matrizA[CANT][CANT];
int matrizB[CANT][CANT];

struct info{
    int matrizC[CANT][CANT];
};

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

void imprimirMatrizC(struct info *ctrl) {
    for (int i = 0; i < CANT; i++) {
        for (int j = 0; j < CANT; j++) {
            printf("%d ", ctrl->matrizC[i][j]);
        }
        printf("\n");
    }
}
void multiplicar (struct info *ctrl) {
    for (int i=0;i<CANT;i++){
        for (int j=0 ; j<CANT; j++) {       
            ctrl->matrizC[i][j] = matrizA[i][j] * matrizB[j][i];
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

    struct info *ctrl;
    int id = shmget(KEY, SIZE, IPC_CREAT | 0666);
    if(id < 0){
        printf("Error en shmget\n");
        exit(1);
    }
    ctrl = (struct info*) shmat(id, 0, 0);
    if(ctrl <= (struct info *) 0){
        printf("Error en shmat\n");
        exit(2);
    }

    pid_t proceso;
    proceso = fork();
    if(proceso == 0){
        multiplicar(ctrl); // escribe directo en la memoria compartida
        shmdt(ctrl);
        exit(0);
    }else{
        wait(NULL); // esperamos a que el hijo termine
    }

    printf("Matriz C resultante \n");
    imprimirMatrizC(ctrl);

    shmdt(ctrl);
    shmctl(id, IPC_RMID, NULL); // liberamos el segmento, como hablamos antes

    return 0;
}