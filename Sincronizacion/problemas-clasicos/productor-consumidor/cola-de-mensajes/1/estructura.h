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

#define VACIO 1
#define LLENO 2
#define MUTEX 3
#define MSG_SIZE sizeof(struct mensaje)-sizeof(long)
#define MAX_PRODS 7

struct mensaje{
	long tipo;
	int p;
};

key_t generateKey() {
	int projectId = 14;
	char path[200];
	getcwd(path, 200 * sizeof(char));
	key_t key = ftok(path, projectId);
	return key;
}
