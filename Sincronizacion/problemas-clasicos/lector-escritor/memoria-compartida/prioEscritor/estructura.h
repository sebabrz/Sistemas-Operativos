#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define ROSA "\033[1;35m" // Magenta brillante
#define RESET "\033[0m"

#define CANTLECTORES 3

struct aCompartir{
	sem_t noLectores;
	sem_t noEscritores;
	sem_t cantEscritores;
	sem_t cantLectores;
	sem_t mutexLect;
	sem_t mutexEsc;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
