#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RESET "\033[0m"

#define READ 0
#define WRITE 1

int pipeSillas[2];
int pipeAula[2];
int pipeAlumno[2];
int pipeMutex[2];
char mensaje = 'a';

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void ayudar() {
    printf(PURPLE"Asistente ayudando a alumno\n"RESET);
    sleep(rand() % 5);
}


void * asistente(){
	while(1){
		read(pipeMutex[READ], &mensaje, sizeof(char));
		if(read(pipeAlumno[READ], &mensaje, sizeof(char)) > 0){
			write(pipeMutex[WRITE], &mensaje, sizeof(char));
			read(pipeAlumno[READ], &mensaje, sizeof(char));
			printf(PURPLE"Asistente se despierta \n"RESET);
			ayudar();
			write(pipeAula[WRITE], &mensaje, sizeof(char));
		}
		else{
			write(pipeMutex[WRITE], &mensaje, sizeof(char));
			ayudar();
			write(pipeAula[WRITE], &mensaje, sizeof(char));
		}
	}
}

void * alumno(){
	while(1){
		printf(YELLOW"Alumno llega \n"RESET);
		read(pipeMutex[READ], &mensaje, sizeof(char));
		if(read(pipeSillas[READ], &mensaje, sizeof(char)) > 0){
			write(pipeAlumno[WRITE], &mensaje, sizeof(char));
			write(pipeMutex[WRITE], &mensaje, sizeof(char));
			printf(YELLOW"alumno espera assitente... \n"RESET);
			read(pipeAula[READ], &mensaje, sizeof(char));
			write(pipeSillas[WRITE], &mensaje, sizeof(char));
		}
		else{
			write(pipeMutex[WRITE], &mensaje, sizeof(char));
			printf(YELLOW"alumno se va sin ayuda \n"RESET);
			exit(5);
		}
		sleep(1);
	}
}

int main(){
	pipe(pipeSillas);
	pipe(pipeAula);
	pipe(pipeAlumno);
	pipe(pipeMutex);
	set_non_blocking(pipeSillas[READ]);
	set_non_blocking(pipeAlumno[READ]);
	
	for(int i = 0; i < 10; i++){
		pid_t pid_alumnos = fork();
		if(pid_alumnos == 0){
			//Cierro el canal de escritura
			close(pipeAlumno[READ]);
			close(pipeAula[WRITE]);
			alumno();
			exit(0);
		}
	}
	
	pid_t pid_asistente = fork();
	if(pid_asistente==0){
		//Cierro el canal de lectura
		printf("Se creo el asistente\n");
		close(pipeSillas[READ]);
		close(pipeSillas[WRITE]);
		close(pipeAula[READ]);
		close(pipeAlumno[WRITE]);
		asistente();
		exit(0);
	}
	
	write(pipeMutex[WRITE], &mensaje, sizeof(char));
	write(pipeAula[WRITE], &mensaje, sizeof(char));
	
	for(int i = 0; i < 3; i++){
		write(pipeSillas[WRITE], &mensaje, sizeof(char));
	}
	
	close(pipeSillas[0]);
	close(pipeSillas[1]);
	close(pipeAula[0]);
	close(pipeAula[1]);
	close(pipeAlumno[0]);
	close(pipeAlumno[1]);
	close(pipeMutex[0]);
	close(pipeMutex[1]);
	
	for(int i = 0; i < 11; i++){
		wait(NULL);
	}	
	
	return 0;
}
