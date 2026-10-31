#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<string.h>
#include<semaphore.h>
#include<pthread.h>

#define TAM_BUFFER 100

struct aCompartir{
	sem_t vacioOx;
	sem_t llenoOx;
	sem_t vacioH;
	sem_t llenoH;
	
	sem_t mutexOx;
	sem_t mutexH;
	
	char bufOxigeno[TAM_BUFFER];
	char bufHidrogeno[TAM_BUFFER];
	
	int agregarOx,sacarOx,agregarH,sacarH;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
