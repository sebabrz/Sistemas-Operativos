#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#define READ_END 0 //aca se lee
#define WRITE_END 1 //aca se escribe

int main(int argc, char *argv[]){
    int fd[2]; //creo un pipe
    pipe(fd);  // fd[0] = extremo de lectura (READ_END), fd[1] = extremo de escritura (WRITE_END)
    struct timeval start, end;

    pid_t pid;
    pid=fork();

    if(pid==0){
        close(fd[READ_END]); //cerramos el extremo de lectura porque vamos a escribir
        gettimeofday(&start, NULL); //guarda el valor del get y lo guarda en la estructura start que guarda timevalues,
                                    //la zona no import y por eso ponemos null
        write(fd[WRITE_END], &start, sizeof(start)); //manda el mensaje por el pipe de lo que se guardo en el start
        close(fd[WRITE_END]);//cerramos el extremo porque ya lo mande
        execvp(argv[1], &argv[1]);//reemplazamos la imagen del proceso con el comando <command> que nos pidan
        fprintf(stderr, "Error al ejecutar el comando\n");//por si fallo
        return 1;
    }else if (pid>0){
        close(fd[WRITE_END]);
        wait(NULL);
        gettimeofday(&end,NULL);
        read(fd[READ_END], &start, sizeof(start));
        close(fd[READ_END]);
        double diferencia=(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        printf("Tiempo transcurrido: %f\n", diferencia);
    }

    return 0;
}