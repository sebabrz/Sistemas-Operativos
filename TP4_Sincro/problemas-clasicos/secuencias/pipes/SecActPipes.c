#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


//ABABABAC

int pipeA[2];
int pipeC[2];
int pipeB[2];

char mensaje = 'X';

void funA(){

	close(pipeA[1]);
	close(pipeC[0]);
	close(pipeB[0]);
	
	
	while(1){
		for(int i=0;i<3;i++){

		read(pipeA[0],&mensaje,sizeof(char));
		printf("A");
		fflush(stdout);
		write(pipeB[1],&mensaje,sizeof(char));
	}
		read(pipeA[0],&mensaje,sizeof(char));
		printf("A");
		fflush(stdout);
		write(pipeC[1],&mensaje,sizeof(char));
	
	}
}

void funB(){
	close(pipeA[0]);
	close(pipeC[0]);
	close(pipeB[1]);
	close(pipeC[1]);

	
	while(1){
		
		read(pipeB[0],&mensaje,sizeof(char));
		printf("B");
		fflush(stdout);
		write(pipeA[1],&mensaje,sizeof(char));
	}
}

void funC(){

	close(pipeB[1]);
	close(pipeB[0]);
	close(pipeC[1]);
	close(pipeA[0]);
	
	while(1){
		
		read(pipeC[0],&mensaje,sizeof(char));
		printf("C");
		fflush(stdout);
		write(pipeA[1],&mensaje,sizeof(char));
	}
}





int main(){
	pid_t pidA;
	pid_t pidB;
	pid_t pidC;

	
	pipe(pipeA);
	pipe(pipeB);
	pipe(pipeC);
	
	

	
	pidA = fork();
	if(pidA == -1){
		printf("Error al crear pidA\n");
	}
	else if(pidA == 0)
		funA();
		
	pidB = fork();
	if(pidB == -1){
		printf("Error al crear pidB\n");
	}
	else if(pidB == 0)
		funB();
	
	pidC = fork();
	if(pidC == -1){
		printf("Error al crear pidC\n");
	}
	else if(pidC == 0)
		funC();
		
	
		
	close(pipeA[0]);
	close(pipeC[0]);
	close(pipeC[1]);
	close(pipeB[1]);
	close(pipeB[0]);
	write(pipeA[1], &mensaje,sizeof(char));
	close(pipeA[1]);
	
	for(int i = 0; i < 3; i++){
		wait(NULL);
	}
}
