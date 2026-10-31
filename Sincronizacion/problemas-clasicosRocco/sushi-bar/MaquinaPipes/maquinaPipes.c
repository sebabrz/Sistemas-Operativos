#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define CANT 10

#define READ 0
#define WRITE 1

int pipePersonasMAX[2];
int pipePersonasUsando[2];
int pipeMaquina[2];
int pipeMutex[2];
char mensaje = 'a';

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void * persona(){
	
	while(1){
		sleep(rand() % 3);
		read(pipePersonasMAX[READ], &mensaje, sizeof(char));
		read(pipeMutex[READ], &mensaje, sizeof(char));
		
		if(read(pipePersonasUsando[READ], &mensaje, sizeof(char)) <= 0){ //ESTA INVERTIDO EL TRYWAIT
			
			read(pipeMaquina[READ], &mensaje, sizeof(char)); //SOY EL PRIMERO
		}
		else{
			write(pipePersonasUsando[WRITE], &mensaje, sizeof(char));
		}
		write(pipePersonasUsando[WRITE], &mensaje, sizeof(char));
		write(pipeMutex[WRITE], &mensaje, sizeof(char));
		
		
		printf("Usando la maquina \n");
		sleep(1);
		
		read(pipeMutex[READ], &mensaje, sizeof(char));
		printf("Termine de usar la maquina \n");
		
		read(pipePersonasUsando[READ], &mensaje, sizeof(char));
		if(read(pipePersonasUsando[READ], &mensaje, sizeof(char)) <= 0){ //ESTA INVERTIDO EL TRYWAIT
			printf("Soy el último, libero la máquina  \n");
			for(int i = 0; i<5; i++){
				write(pipePersonasMAX[WRITE], &mensaje, sizeof(char));
			}
			write(pipeMaquina[WRITE], &mensaje, sizeof(char));
		}
		else{
			write(pipePersonasUsando[WRITE], &mensaje, sizeof(char));
		}
		write(pipeMutex[WRITE], &mensaje, sizeof(char));
	
	}
	
}


int main(){
	
	pipe(pipePersonasMAX);
	pipe(pipePersonasUsando);
	pipe(pipeMaquina);
	pipe(pipeMutex);
	set_non_blocking(pipePersonasUsando[READ]);
	
	for(int i = 0; i < CANT; i++){
		pid_t pid_personas = fork();
		if(pid_personas == 0){
			persona();
			exit(0);
		}
	}
	
	write(pipeMaquina[WRITE], &mensaje, sizeof(char));
	write(pipeMutex[WRITE], &mensaje, sizeof(char));
	
	for(int i = 0; i < 3; i++){
		write(pipePersonasMAX[WRITE], &mensaje, sizeof(char));
	}
	close(pipePersonasMAX[READ]);
	close(pipePersonasMAX[WRITE]);
	close(pipePersonasUsando[READ]);
	close(pipePersonasUsando[WRITE]);
	close(pipeMaquina[READ]);
	close(pipeMaquina[WRITE]);
	close(pipeMutex[READ]);
	close(pipeMutex[WRITE]);
	
	for(int i = 0; i < CANT; i++){
		wait(NULL);
	}
	
	return 0;
}

