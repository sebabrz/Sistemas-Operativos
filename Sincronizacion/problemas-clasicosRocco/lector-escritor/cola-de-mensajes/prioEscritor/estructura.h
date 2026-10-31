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

#define NOLECTORES 1
#define NOESCRITORES 2
#define CANTESCRITORES 3
#define CANTLECTORES 4
#define MUTEXLECTORES 5
#define MUTEXESCRITORES 6
#define MSG_SIZE sizeof(struct mensaje)-sizeof(long)
#define CANT 7

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
