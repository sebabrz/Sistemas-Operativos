#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#define CANT 10



char m ='x';

int a[2],b[2],c[2];

void *fA(){

	close(c[1]);
	close(c[0]);
	close(b[0]);
	close(a[1]);

	while(1){
		for(int i=0;i<2;i++){
			read(a[0], &m, sizeof(char));
			printf("A");
			fflush(stdout);
			write(b[1], &m, sizeof(char));
	}
		read(a[0], &m, sizeof(char));
	
		
	}
	
}

void* fB(){


	close(c[0]);
	close(b[1]);
	close(a[0]);
	
	
	while(1){
			read(b[0], &m, sizeof(char));
			printf("B");
			fflush(stdout);
			write(c[1], &m, sizeof(char));
			write(a[1], &m, sizeof(char));
		
	}
	
}

void *fC(){
	close(c[1]);
	close(b[1]);
	
	close(b[0]);
	close(a[0]);
	
	
	while(1){
			read(c[0], &m, sizeof(char));
			read(c[0], &m, sizeof(char));
			printf("C");
			fflush(stdout);
			write(a[1], &m, sizeof(char));
		
	}
	
}


int main(){
	
	pid_t pidA;
pid_t pidB;
pid_t pidC;


pipe(a);
pipe(b);
pipe(c);




pidA = fork();
if(pidA == -1){
printf("Error al crear pidA\n");
}
else if(pidA == 0)
fA();

pidB = fork();
if(pidB == -1){
printf("Error al crear pidB\n");
}
else if(pidB == 0)
fB();

pidC = fork();
if(pidC == -1){
printf("Error al crear pidC\n");
}
else if(pidC == 0)
fC();



close(a[0]);
close(c[0]);
close(c[1]);
close(b[1]);
close(b[0]);
write(a[1], &m,sizeof(char));
close(a[1]);

for(int i = 0; i < 3; i++){
wait(NULL);
}
}
