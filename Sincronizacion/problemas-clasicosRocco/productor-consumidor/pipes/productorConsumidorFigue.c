#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define maxProducts 100
//#define bufferSize 20
int mensaje = 1;

int pipeProdCons[2];
int pipeVacio[2];
int pipeLleno[2];
int pipeMutex[2];

void productor(){
	while(1){
		read(pipeVacio[0], &mensaje, sizeof(mensaje));
		read(pipeMutex[0], &mensaje, sizeof(mensaje));
			mensaje = random() % 100;
			printf("Produje %d.\n", mensaje);
			write(pipeProdCons[1], &mensaje, sizeof(mensaje));
		write(pipeMutex[1], &mensaje, sizeof(mensaje));
		write(pipeLleno[1], &mensaje, sizeof(mensaje));
		sleep(2);
	}
}

void consumidor(){
	while(1){
		read(pipeLleno[0], &mensaje, sizeof(mensaje));
		read(pipeMutex[0], &mensaje, sizeof(mensaje));
			read(pipeProdCons[0], &mensaje, sizeof(mensaje));
			printf("Consumi %d.\n", mensaje);
		write(pipeMutex[1], &mensaje, sizeof(mensaje));
		write(pipeVacio[1], &mensaje, sizeof(mensaje));
		sleep(2);
	}
}

int main(){
	pipe(pipeProdCons);
	pipe(pipeVacio);
	pipe(pipeLleno);
	pipe(pipeMutex);
	write(pipeMutex[1], &mensaje, sizeof(mensaje));
	for(int i = 0; i < 20; i++){
		write(pipeVacio[1], &mensaje, sizeof(mensaje));
	}
	pid_t productorProc = fork();
	if(productorProc==0){
		//Cierro el canal de lectura
		printf("Se creo el productor\n");
		close(pipeProdCons[0]);
		close(pipeLleno[0]);
		close(pipeVacio[1]);
		productor();
		exit(0);
	}
	pid_t consumidorProd = fork();
	if(consumidorProd == 0){
		printf("Se creo el consumidor\n");
		//Cierro el canal de escritura
		close(pipeProdCons[1]);
		close(pipeLleno[1]);
		close(pipeVacio[0]);
		consumidor();
		exit(0);
	}
	
	close(pipeProdCons[0]);
	close(pipeProdCons[1]);
	close(pipeVacio[0]);
	close(pipeVacio[1]);
	close(pipeLleno[0]);
	close(pipeLleno[1]);
	close(pipeMutex[0]);
	close(pipeMutex[1]);
	wait(NULL);
	wait(NULL);
	
	return 0;
}
