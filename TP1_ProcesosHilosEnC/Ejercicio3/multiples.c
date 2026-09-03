#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int pid;

    pid = fork();

    if (pid < 0){
        printf("Ocurrio un error");
        exit(1);
    }

    if (pid > 0) {
        wait(NULL);
        printf ("Soy el proceso padre mi id es %d \n", getpid());
    }

    if (pid == 0) {
        printf ("Soy el proces hijo mi id es %d y mi papa es %d\n", getpid(), getppid());
        execlp("usr", "ls", "-al", NULL);
    }; //si soy el hijo, no sigo creando y salgo del proceso. 


    return 0;
}