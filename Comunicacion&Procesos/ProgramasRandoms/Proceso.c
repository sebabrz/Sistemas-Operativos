#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//La idea es crear un proceso, y que este muestre su PID

int main() {
    //aca getpid() es el proceso que estoy creando
    printf ("Hola, soy el proceso con ID %d \n", getpid());

    char teclaPresionada;

    printf("Si quiere crear un proceso hijo, del proceso actual presiona s, para salir presione n \n");
    scanf("%c", &teclaPresionada);

    if (teclaPresionada == 's'){

        int pid;

        pid = fork();   //Creo un proceso hijo, y me devuelve el PID.

        if (pid > 0) {
            printf ("Hola, soy el proceso padre con ID %d \n", getpid());
        }   
            else printf ("Hola, soy el proceso hijo con ID %d \n", getpid());

    }   else  if (teclaPresionada == 'n'){
            printf ("Adios!\n");
            return 0;
        } 

    return 0;
}