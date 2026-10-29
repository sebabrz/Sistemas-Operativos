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

#define MSG_SIZE 25
#define SEG_SIZE sizeof(struct info)
#define BFR_SIZE 20
struct info {
		int msg[BFR_SIZE];	
		sem_t lleno;
		sem_t vacio;
		sem_t mutex;
		int cant_prod;
};



int main(){
	
	pid_t productor, consumidor;
	int status, id;
	
	key_t key = ftok("/temp",'A');
	

	id = shmget(key,SEG_SIZE,IPC_CREAT | 0666);
	if(id == -1){
		printf("fallo al crear el segmento: %s",strerror(errno));
		exit(0);
		
	}
	
	struct info init = (struct info) shmat(id,0,0);
	
	sem_init(&(init->lleno),1,0);
	sem_init(&(init->vacio),1,BFR_SIZE);
	sem_init(&(init->mutex),1,1);
	init->cant_prod = 0;
	
	
		productor = fork();
	
		if(productor == 0){
			shmdt(init);
			struct info info = (struct info) shmat(id,0,0);
			
			for(int i = 0; i < BFR_SIZE; i++){
				
				sem_wait(&(info->vacio));
				sem_wait(&(info->mutex));
					info->msg[info->cant_prod] = rand();
					
					fflush(stdout);
					(info->cant_prod)++;
				sem_post(&(info->mutex));
				sem_post(&(info->lleno));
			
				
			}
			shmdt(info);
			exit(0);
		}



		consumidor = fork();
		
		if(consumidor == 0){
			shmdt(init);
			struct info info = (struct info) shmat(id,0,0);
			for(int i = 0; i < BFR_SIZE; i++){
				sem_wait(&(info->lleno));
				sem_wait(&(info->mutex));
					(info->cant_prod)--;
					printf("%d %d\n",info->msg[info->cant_prod],info->cant_prod);
					fflush(stdout);
				sem_post(&(info->mutex));
				sem_post(&(info->vacio));
				
				
				
			}
			shmdt(info);
			exit(0);
			
		}
	
	
	waitpid(productor, &status, 0);
	waitpid(consumidor, &status, 0);
	
	sem_close(&(init->lleno));
	sem_close(&(init->vacio));
	sem_close(&(init->mutex));
	
	shmdt(init);
	shmctl(id,IPC_RMID,NULL);
	
	
	exit(0);	
}
