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
    Usar un pipe para mandar datos de un proceso padre 
    a un proceso hijo
*/

int main() {

    int pipeFD[2];

    /*
    pipeFD[1] es la entrada de escritura.
    pipeFD[0] es la entrada de lectura.
    */

    pid_t pid;

    if (pipe(pipeFD) == -1 ) {
        printf ("Error al crear el pipe");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        printf ("Error al crear proceso");

    }

    if (pid > 0){
        //---- Proceso padre ----
        printf ("Soy el proceso padre ID: %d \n", getpid());
        close(pipeFD[0]); 
        char mensaje1[100] = "Hola hijo!";
        char mensaje2[100] = "Como estas?";

        write (pipeFD[1], mensaje1, sizeof (mensaje1));
        write (pipeFD[1], mensaje2, sizeof (mensaje2));

        close(pipeFD[1]); 

        printf ("Mensajes enviados.\n");

    } else if(pid ==0){
        //---- Proceso hijo ----
        close(pipeFD[1]);
        char mensaje_1[100];
        char mensaje_2[100];
        read (pipeFD[0], mensaje_1, sizeof (mensaje_1));
        read (pipeFD[0], mensaje_2, sizeof (mensaje_2));

        close(pipeFD[0]);
        
        printf ("Soy el proceso hijo ID: %d \n", getpid());
        printf ("El mensaje es %s\n", mensaje_1 );
        printf ("El mensaje es %s\n", mensaje_2 );
        printf ("Mensajes recibidos.\n");
        
    }
    return 0;
}