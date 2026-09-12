#include <stdio.h> //funciones estándar de entrada/salida, como printf() y fprintf()
#include <sys/types.h> //define el tipo pid_t donde guardamos el PID que devuelve el fork()
#include <sys/wait.h> //agrega el wait()
#include <unistd.h> //aca viven las system calss fork(), exec(), getpid()(

int main (){
    pid_t pid;

    pid= fork();//creamos el proceso hijo

    if (pid<0){ //nos fijamos si  algo salio mal al crear al hijo
        fprintf(stderr, "Fork Failed"); //stderr es el flujo standar de error, un canal de salida, y el fprint lo dirige para ahi
        return 1;
    }
    else if(pid==0){ //estamos en el hijo, execlp() reemplaza la imagen de memoria del proceso hijo por el programa /bin/ls
                    //El primer "ls" es el nombre del programa (argv[0]), y NULL marca el final de la lista de argumentos. 
                    //Como exec() no vuelve si tiene éxito, el hijo nunca sigue ejecutando código después de esta línea — pasa a ser directamente el comando ls.
         printf("Soy el hijo, mi PID es %d, mi padre es %d\n", getpid(), getppid());//Soy el hijo, mi PID es 18344, mi padre es 18343
                    execlp("/bin/ls", "ls", NULL); //ejercicioProcesosLibro  ejercicioProcesosLibro.c  ejercicioProcesosLibro.o
         
    }else if(pid>0){ //estamos en el padre
        wait(NULL); //espera a que termine el hijo
        printf("Child Complete");// cuando el hijo termina ejecuta esto
        printf("Soy el padre, mi PID es %d\n", getpid());//Child CompleteSoy el padre, mi PID es 18343
    }


    return 0;//termina el proceso padre
}