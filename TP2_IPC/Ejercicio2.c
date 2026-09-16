#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <time.h>       // time, medicion de tiempo
#include <stdint.h>     // enteros de tamaño fijo
#include <sys/mman.h>   // memoria compartida (mmap)
#include <fcntl.h>      // flags de apertura (O_CREAT, etc.)
#include <sys/stat.h>   // permisos de archivos
#include <sys/shm.h>    // memoria compartida System V
#include <string.h>     // manejo de strings
#include <sys/time.h>   // gettimeofday
#include <math.h>       // funciones matematicas
#include <dirent.h>     // lectura de directorios

/* 
    Ambos procesos van a enviar varios mensajes entre hijo y padre, es decir
    los dos van a escribirle y leer, lo del otro. El padre va enviar un mensaje
    y va a esperar que el hijo le responda antes de mandar otro mensaje
    El hijo cuando recibe lo escrito por el mama, va a enviar un mensaje al papa
    Cuando uno de los procesos decide terminar, enviar un mensaje de salida
*/

int main() {

    int pipeFD1[2];
    int pipeFD2[2];
    char mensaje[100];
    /*

    pipeFD[0] es la entrada de lectura.
    pipeFD[1] es la entrada de escritura.
    */

    pid_t pid;

    if (pipe(pipeFD1) == -1  && pipe(pipeFD2) == -1 ) {
        printf ("Error al crear dos pipes");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        printf ("Error al crear proceso");
        exit(1);
    }

    if (pid > 0) {
        //---- Proceso padre ----
        printf ("Soy el proceso padre ID: %d \n", getpid());

        close(pipeFD1[0]); //cierro lectura de pipe 1
        close(pipeFD2[1]); //cierro escritura del pipe 2
        //voy a escribir en el pipe 1
        char *mensaje = "Hola mundo";
        write (pipeFD1[1], mensaje, sizeof (mensaje)); //escribo sobre el extremo de lectura
        wait(NULL);
        
    }   else if(pid == 0){
            //---- Proceso hijo ----
        close(pipeFD2[0]); //cierro lectura de pipe 2
        close(pipeFD1[1]); //cierro escritura del pipe 1
        read (pipeFD1[0], mensaje, sizeof (mensaje)); //escribo sobre el extremo de lectura

        printf ("Soy el proceso hijo ID: %d \n", getpid());
        printf ("El mensaje es %s!\n", mensaje);
        }
    return 0;
}