#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
int main() {
    int pipeFD[2];  //crea un pipe UNIDERECCIONAL
                    //[0] queda asignado para lectura
                    //[1] queda asignado para escritura
    pid_t pid;
    if (pipe(pipeFD) == -1 ) {
        printf ("Error al crear el pipe");
        exit(1);
    }
    pid = fork();
    if (pid < 0) {
        printf("Error al hacer fork");
    }
    if (pid > 0) {
        //Proceso padre
        close(pipeFD[1]); //Se cierra el conducto de escritura, porque el padre lee
        wait (NULL); //espero a que termine el hijo
        char mensajeRecibido[100];
        printf ("Proceso padre ID: %d \n" , getpid());
        read (pipeFD[0], mensajeRecibido, sizeof(mensajeRecibido));     //leo lo que me llego.
        printf ("Llego el mensaje, es: %s \n", mensajeRecibido);
    } else {
        //Proceso hijo
        close(pipeFD[0]); //Se cierra el conducto de lectura, porque el hijo escribe.
        char mensajeEnviado[100];
        printf("Escriba su mensaje para mandar al padre: \n");
        scanf("%s", mensajeEnviado);
        write(pipeFD[1], mensajeEnviado, sizeof(mensajeEnviado));
        close(pipeFD[1]);   //cierro el extremo de escritura.
        exit(1);
    }

    return 0;
}