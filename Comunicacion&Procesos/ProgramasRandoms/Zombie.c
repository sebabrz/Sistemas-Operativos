#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


/*
    dejar un proceso zombie, por 10 segundos, no hace falta usar wait en el padre
    
*/
int main() {

    pid_t pid;

    pid = fork();

    if (pid < 0) {
        printf ("Error al crear el proceso");
    } else if (pid == 0) {
        printf ("Soy el proceso hijo mi ID es %d \n", getpid());
        exit(1);

    } else if (pid > 0) {
        sleep(10);
        printf ("Soy el proceso padre mi ID es %d \n", getpid());

    }

    return 0;
}