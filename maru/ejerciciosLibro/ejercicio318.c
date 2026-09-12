#include <stdio.h> //funciones estándar de entrada/salida, como printf() y fprintf()
#include <sys/types.h> //define el tipo pid_t donde guardamos el PID que devuelve el fork()
#include <sys/wait.h> //agrega el wait()
#include <unistd.h> 
#include <stdlib.h>

int main(){
    pid_t pid;
    pid=fork();
    if(pid<0){
        perror("FAIL");
        return 0;
    }else if (pid==0){
        printf("Soy el procesos hijo y me voy a dormir \n");
        exit(1);
    }else{
        sleep(10);
        wait(NULL);
        printf("soy el proceso padre y mis hijos terminaron\n");

    }


    return 0;
}