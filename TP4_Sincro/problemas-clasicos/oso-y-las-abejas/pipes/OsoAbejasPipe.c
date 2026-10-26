#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#define PIPE_READ 0
#define PIPE_WRITE 1

#define TIME 1
#define PORCIONES 10
#define ABEJAS 3

char m ='x';

int pipeVacio[2], pipeLleno[2], pipeOso[2], pipeAbejas[2], pipeMutex[2];

    // Función para configurar un pipe como no bloqueante
    void set_non_blocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

void abeja(){
    
        close(pipeLleno[PIPE_READ]);
        close(pipeOso[PIPE_READ]);
        
    while (1) {
            sleep(TIME);

            read(pipeAbejas[PIPE_READ], &m, sizeof(char));   
            read(pipeMutex[PIPE_READ], &m, sizeof(char));

            read(pipeVacio[PIPE_READ], &m, sizeof(char));  
            int bytes_leidos = read(pipeVacio[PIPE_READ], &m, sizeof(char));
            if (bytes_leidos > 0) {
                write(pipeVacio[PIPE_WRITE], &m, sizeof(char));
                write(pipeAbejas[PIPE_WRITE], &m, sizeof(char));
            }
            else {
                // soy ultima abeja
                write(pipeOso[PIPE_WRITE], &m, sizeof(char));
            }

            // guardo porcion miel
            printf("\033[1;33mAbeja GUARDA miel\033[0m\n");
            write(pipeLleno[PIPE_WRITE], &m, sizeof(char));

            write(pipeMutex[PIPE_WRITE], &m, sizeof(char));
        }
}

void oso(){
        close(pipeAbejas[PIPE_READ]);
        close(pipeVacio[PIPE_READ]);
        close(pipeLleno[PIPE_WRITE]);
        close(pipeMutex[PIPE_READ]);
        close(pipeMutex[PIPE_WRITE]);
        close(pipeOso[PIPE_WRITE]);

        
     while (1) {
        sleep(TIME);
        read(pipeOso[PIPE_READ], &m, sizeof(char));   

        printf("\033[1;31mUN OSO WACHO\033[0m\n");
        for (int i = 0; i < PORCIONES; i++) {
            read(pipeLleno[PIPE_READ], &m, sizeof(char));  
            printf("\033[0;33mOso COME miel %d\033[0m\n",i);
            write(pipeVacio[PIPE_WRITE], &m, sizeof(char));
        }
        write(pipeAbejas[PIPE_WRITE], &m, sizeof(char));
    }
}
int main(){
    pipe(pipeAbejas);
    pipe(pipeOso);
    pipe(pipeMutex);
    pipe(pipeLleno);
    pipe(pipeVacio);
    set_non_blocking(pipeVacio[PIPE_READ]);
    
    write(pipeAbejas[PIPE_WRITE], &m, sizeof(char));
    write(pipeMutex[PIPE_WRITE], &m, sizeof(char));

    for(int i = 0;i < PORCIONES;i++){
        write(pipeVacio[PIPE_WRITE], &m, sizeof(char));
    }
    
    for(int i = 0;i < ABEJAS;i++){
        pid_t abejas = fork();
            if(abejas==0){
                printf("Se creo abeja\n");
                abeja();
                exit(0);
            }    
    }
    

    pid_t oso_pid = fork();
    if(oso_pid == 0){
        printf("Se creo el oso\n");
        //Cierro el canal de escritura
        oso();
        exit(0);
    }
    
    for(int i = 0;i < ABEJAS + 1;i++){
        wait(NULL);
    }    
    
        close(pipeAbejas[PIPE_READ]);
        close(pipeVacio[PIPE_READ]);
        close(pipeLleno[PIPE_WRITE]);
        close(pipeMutex[PIPE_READ]);
        close(pipeMutex[PIPE_WRITE]);
        close(pipeOso[PIPE_WRITE]);
        close(pipeLleno[PIPE_READ]);
        close(pipeOso[PIPE_READ]);
        
    return 0;
}
