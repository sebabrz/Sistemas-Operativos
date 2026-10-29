#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MUJERES 10
#define HOMBRES 10

#define READ 0
#define WRITE 1
char mensaje = 'a';

int pipeMujeres[2];
int pipeMujeresMax[2];
int pipeHombres[2];
int pipeHombresMax[2];
int pipeBaño[2];
int pipeMutex1[2];
int pipeMutex2[2];
int pipeTurno[2];

// Definición de colores
#define COLOR_MUJER "\033[1;34m"  // Azul
#define COLOR_HOMBRE "\033[1;32m"  // Verde
#define COLOR_RESET "\033[0m"      // Reset

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void *mujeres()
{
    while (1){  
        read(pipeTurno[READ], &mensaje, sizeof(char));
        read(pipeMujeresMax[READ], &mensaje, sizeof(char));  
        read(pipeMutex2[READ], &mensaje, sizeof(char));  
        write(pipeTurno[WRITE], &mensaje, sizeof(char));
            if(read(pipeMujeres[READ], &mensaje, sizeof(char)) > 0){
                write(pipeMujeres[WRITE], &mensaje, sizeof(char));
            }
            else {
                read(pipeBaño[READ], &mensaje, sizeof(char)); 
                printf(COLOR_MUJER "Mujer es la primera.\n" COLOR_RESET);
            }
            write(pipeMujeres[WRITE], &mensaje, sizeof(char));
        write(pipeMutex2[WRITE], &mensaje, sizeof(char));

        printf(COLOR_MUJER "Mujer se encuentra en el baño.\n" COLOR_RESET);     
        sleep(1);   
        
        read(pipeMutex2[READ], &mensaje, sizeof(char));
            read(pipeMujeres[READ], &mensaje, sizeof(char));
            printf(COLOR_MUJER "Mujer se va.\n" COLOR_RESET);   
            if(read(pipeMujeres[READ], &mensaje, sizeof(char)) > 0){
                write(pipeMujeres[WRITE], &mensaje, sizeof(char));
            }
            else{
                write(pipeBaño[WRITE], &mensaje, sizeof(char));
                printf(COLOR_MUJER "Mujer es la ultima.\n" COLOR_RESET);
            }
        write(pipeMutex2[WRITE], &mensaje, sizeof(char));
        write(pipeMujeresMax[WRITE], &mensaje, sizeof(char));
        sleep(3);
    }
}

void *hombres() 
{
    while (1){ 
        read(pipeTurno[READ], &mensaje, sizeof(char));
        read(pipeHombresMax[READ], &mensaje, sizeof(char));  
        read(pipeMutex1[READ], &mensaje, sizeof(char));  
        write(pipeTurno[WRITE], &mensaje, sizeof(char));
            if(read(pipeHombres[READ], &mensaje, sizeof(char)) > 0){
                write(pipeHombres[WRITE], &mensaje, sizeof(char));
            }
            else {
                read(pipeBaño[READ], &mensaje, sizeof(char)); 
                printf(COLOR_HOMBRE "Hombre es el primero.\n" COLOR_RESET);
            }
            write(pipeHombres[WRITE], &mensaje, sizeof(char));
        write(pipeMutex1[WRITE], &mensaje, sizeof(char));

        printf(COLOR_HOMBRE "Hombre se encuentra en el baño.\n" COLOR_RESET);        
        sleep(1);
        
        read(pipeMutex1[READ], &mensaje, sizeof(char));
            read(pipeHombres[READ], &mensaje, sizeof(char));
            printf(COLOR_HOMBRE "Hombre se va.\n" COLOR_RESET);   
            if(read(pipeHombres[READ], &mensaje, sizeof(char)) > 0){
                write(pipeHombres[WRITE], &mensaje, sizeof(char));
            }
            else{
                write(pipeBaño[WRITE], &mensaje, sizeof(char));
                printf(COLOR_HOMBRE "Hombre es el ultimo.\n" COLOR_RESET);
            }
        write(pipeMutex1[WRITE], &mensaje, sizeof(char));
        write(pipeHombresMax[WRITE], &mensaje, sizeof(char));
        sleep(3);
    }
}

int main()
{
    pipe(pipeMujeres);
    pipe(pipeHombres);
    pipe(pipeBaño);
    pipe(pipeMutex1);
    pipe(pipeMutex2);
    pipe(pipeHombresMax);
    pipe(pipeMujeresMax);
    pipe(pipeTurno);
    set_non_blocking(pipeMujeres[READ]);
    set_non_blocking(pipeHombres[READ]);
    write(pipeBaño[WRITE], &mensaje, sizeof(char));
    write(pipeTurno[WRITE], &mensaje, sizeof(char));
    write(pipeMutex1[WRITE], &mensaje, sizeof(char));
    write(pipeMutex2[WRITE], &mensaje, sizeof(char));
    for(int i = 0; i < 3; i++){
         write(pipeMujeresMax[WRITE], &mensaje, sizeof(char));
         write(pipeHombresMax[WRITE], &mensaje, sizeof(char));
    }
    
    for(int i = 0; i < HOMBRES; i++){
		pid_t pid_hombres = fork();
		if(pid_hombres == 0){
            close(pipeMutex2[READ]);
            close(pipeMutex2[WRITE]);
            close(pipeMujeres[READ]);
            close(pipeMujeres[WRITE]);
            close(pipeMujeresMax[WRITE]);
            close(pipeMujeresMax[READ]);
			hombres();
			exit(0);
		}
	}
    for(int i = 0; i < MUJERES; i++){
		pid_t pid_mujeres = fork();
		if(pid_mujeres == 0){
            close(pipeMutex1[READ]);
            close(pipeMutex1[WRITE]);
            close(pipeHombres[READ]);
            close(pipeHombres[WRITE]);
            close(pipeHombresMax[WRITE]);
            close(pipeHombresMax[READ]);
			mujeres();
			exit(0);
		}
	}

    for (int i = 0; i < HOMBRES; i++) {
       wait(NULL);  
    }
    
    for (int i = 0; i < MUJERES; i++) {
        wait(NULL); 
    }

    close(pipeMujeres[READ]);
    close(pipeMujeres[WRITE]);
    close(pipeHombres[READ]);
    close(pipeHombres[WRITE]);
    close(pipeBaño[READ]);
    close(pipeBaño[WRITE]);
    close(pipeMutex1[READ]);
    close(pipeMutex1[WRITE]);
    close(pipeMutex2[READ]);
    close(pipeMutex2[WRITE]);
             close(pipeHombresMax[WRITE]);
            close(pipeHombresMax[READ]);
             close(pipeMujeresMax[WRITE]);
            close(pipeMujeresMax[READ]);
             close(pipeTurno[WRITE]);
            close(pipeTurno[READ]);

    return 0;
}
