#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/msg.h>

#define CANT 9
#define KEY ((key_t) (1243))

int matrizA[CANT][CANT];
int matrizB[CANT][CANT];

struct mensaje{
    long tipo;
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

void imprimirMatrizC(struct mensaje *msg) {
    for (int i = 0; i < CANT; i++) {
        for (int j = 0; j < CANT; j++) {
            printf("%d ", msg->matrizC[i][j]);
        }
        printf("\n");
    }
}

void multiplicar(struct mensaje *msg) {
    for (int i=0; i<CANT; i++){
        for (int j=0; j<CANT; j++) {
                msg->matrizC[i][j] = matrizA[i][j] * matrizB[j][i];
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

    struct mensaje msg;
    int idmsg=msgget(KEY, IPC_CREAT | 0666);
    int longitud = sizeof(struct mensaje) - sizeof(long); //long no cuenta en tamaño del msg


    pid_t proceso;
    proceso=fork();//aca tiene en su memoria la variable del id de la cola que guardo el padre
    if(proceso==0){
        msg.tipo=1;
        multiplicar(&msg);
        msgsnd(idmsg, &msg, longitud, 0);//manda mensaje por la cola
        exit(0);
    }else{
        wait(NULL); //esperamos a que el hijo labure
        msgrcv(idmsg, &msg, longitud, 1, 0); // el 1 aca es el tipo que pedis
    }

    printf("Matriz C resultante \n");
    imprimirMatrizC(&msg);
    msgctl(idmsg, IPC_RMID, NULL); //liberamos la cola
    return 0;
}