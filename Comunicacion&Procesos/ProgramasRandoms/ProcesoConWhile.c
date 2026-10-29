#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


//La idea es crear un proceso, y que este muestre su PID pero con un while para meter un chiche

int main() {
    printf("Hola, soy el proceso con ID %d \n", getpid());

    char teclaPresionada;

    while (teclaPresionada != 's' && teclaPresionada != 'n') {

        printf("Si quiere crear un proceso hijo, del proceso actual presiona s, para salir presione n \n");

        scanf(" %c", &teclaPresionada);

        if (teclaPresionada != 's' && teclaPresionada != 'n') {
            printf("No se presiono ninguna tecla valida, vuelva a intentarlo\n");
        }
    }

    if (teclaPresionada == 's') {

        int pid;

        pid = fork();

        printf("Hola, soy el proceso con ID %d \n", getpid());

    } else {
        printf("Adios!\n");
        return 0;
    }

    return 0;
}