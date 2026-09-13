#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int pid;

    int cant;

    printf("Ingrese la cantidad de hijos a crear: \n");
    scanf("%d", &cant);

    for (int i=0; i<cant; i++){
        pid = fork();

        if (pid == 0) {
            printf ("Soy el proces hijo mi id es %d y mi papa es %d\n", getpid(), getppid());
            exit(1);
        }; //si soy el hijo, no sigo creando y salgo del proceso.
    } 

    if (pid < 0){
        printf("Ocurrio un error");
        exit(1);
    }

    if (pid > 0) {
        wait(NULL);
        printf ("Soy el proceos padre mi id es %d \n", getpid());
    }


    return 0;
}