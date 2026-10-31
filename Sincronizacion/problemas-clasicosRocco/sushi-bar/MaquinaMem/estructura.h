
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define CANT 10

struct aCompartir{
	sem_t cantPersonasMax;
	sem_t cantPersonasUsando;
	sem_t maquina;
	sem_t mutex;
	
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
