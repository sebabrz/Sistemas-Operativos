#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>

#define SEG_SIZE sizeof(message)

key_t key;

typedef struct{
	sem_t semA;
	sem_t semB;
	sem_t semCD;
} message;

void funA(){
	int id = shmget(key, SEG_SIZE, 0);
	
	message *info = (message *) shmat(id,0,0);
	if (info <= (message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
	while(1){
		sem_wait(&(info->semA));
		printf("A");
		fflush(stdout);
		sem_post(&(info->semB));
	}
}

void funB(){
	int id = shmget(key, SEG_SIZE, 0);
	message *info = (message *) shmat(id,0,0);
	if (info <= (message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
	while(1){
		sem_wait(&(info->semB));
		sem_wait(&(info->semB));
		printf("B");
		fflush(stdout);
		sem_post(&(info->semCD));
	}
}

void funC(){
	int id = shmget(key, SEG_SIZE, 0);
	message *info = (message *) shmat(id,0,0);
	if (info <= (message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
	while(1){
		sem_wait(&(info->semCD));
		printf("C\n");
		fflush(stdout);
		sleep(2);
		sem_post(&(info->semA));
		sem_post(&(info->semA));
	}
	
}

void funD(){
	int id = shmget(key, SEG_SIZE, 0);
	message *info = (message *) shmat(id,0,0);
	if (info <= (message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
	while(1){
		sem_wait(&(info->semCD));
		printf("D\n");
		fflush(stdout);
		sleep(2);
		sem_post(&(info->semA));
		sem_post(&(info->semA));
	}
	
}

int main(){
	key = ftok("/Sahid_Dulce_se_la_come", 'A');
	
	//Creo el segmento de mem compartida
	int id = shmget(key, SEG_SIZE, IPC_CREAT | 0666);
    
    //Mapea el segmento de mem compartida
    message *info = (message *) shmat(id,0,0);

    if (info <= (message *) (0)) {
        perror("Error al mapear segmento de memoria compartida \n");
        exit(-1);
    }
	
	sem_init(&(info->semA), 1, 2);
	sem_init(&(info->semB), 1, 0);
	sem_init(&(info->semCD), 1, 0); 
	
	int pidA = fork();
	if(pidA == 0){
		funA();
	}
	int pidB = fork();
	if(pidB == 0){
		funB();
	}
	int pidC = fork();
	if(pidC == 0){
		funC();
	}
	int pidD = fork();
	if(pidD == 0){
		funD();
	}
	
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	
	shmdt(info);
	return 0;
}
