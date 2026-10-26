#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define PINK "\033[38;5;206m" 
#define LIGHT_BLUE "\033[38;5;117m" 

#define CANT 10

struct aCompartir{
	sem_t HombresDentro;
	sem_t HombresMax;
	sem_t MujeresDentro;
	sem_t MujeresMax;
	sem_t turno;
	sem_t genero;
	sem_t mutexH;
	sem_t mutexM;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
