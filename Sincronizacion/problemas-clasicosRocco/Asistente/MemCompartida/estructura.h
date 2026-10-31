#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define ITER 100
#define ALUMNOS 10
#define SILLAS 3

#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RESET "\033[0m"



struct aCompartir{
	sem_t sem_sillas;
	sem_t sem_aula;
	sem_t sem_alumno;
	sem_t mtx;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
