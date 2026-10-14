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

#define WRITE 1
#define READ 0

int main(int argc, char *argv[]) {
    int FD1[2];
    pid_t pid;
    char mensaje;

    if(pipe(FD1) == -1 ){
        perror("Error al crear pipe");
        exit(1);
    }
    FILE *archivo;

    pid=fork();

    if(pid<0){
        perror("Error al crear hijo");
        exit(2);
    }else

    if(pid==0){//estoy en el hijo, voy a tener que cerrar escritura del pipe porque va a leer y copiar en su archivo txt
        close(FD1[WRITE]);
        archivo= fopen(argv[2], "w");
        while(1){
            read(FD1[READ], &mensaje, sizeof(mensaje));
            if(mensaje=='@'){
                break;
            }
            fwrite(&mensaje, sizeof(mensaje), 1, archivo);
        }
        close(FD1[READ]);
    }else{//soy el padre, voy a tener que cerrar la lectura porque voy a escribir leyendo caracteres
        close(FD1[READ]);
        int c;
        archivo= fopen(argv[1], "r");
        while((c=fgetc(archivo))!= EOF){
            mensaje= (char) c;
            write(FD1[WRITE], &mensaje, sizeof(mensaje));
        }
        if(c==EOF){
            mensaje='@';
            write(FD1[WRITE], &mensaje, sizeof(mensaje));
        }
        close(FD1[WRITE]);
    }


    return 0;
}