#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stddef.h>
#include <time.h>

#define TURNO 1
#define BANO 2
#define CANT_HOMBRES 3
#define CANT_MUJERES 4
#define MAX_H 5
#define MAX_M 6

#define MUTEX_H 7
#define MUTEX_M 8

#define MSG_SIZE sizeof(struct mensaje)-sizeof(long)
 
#define CANT 10
#define MAX_BANO 3
struct mensaje{
	long tipo;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
