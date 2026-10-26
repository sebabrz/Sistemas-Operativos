
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define READERS 3
#define WRITERS 3

// Definición de colores
#define COLOR_WRITER "\033[1;34m"  // Azul
#define COLOR_READER "\033[1;32m"  // Verde
#define COLOR_RESET "\033[0m"      // Reset

struct aCompartir{
	sem_t sem_readers;
	sem_t bsem_rw;
	sem_t mutex;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
