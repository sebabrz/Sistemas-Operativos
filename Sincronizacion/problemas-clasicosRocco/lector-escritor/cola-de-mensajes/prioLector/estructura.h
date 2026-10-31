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

#define MUTEX 1
#define LECTORES 2
#define RW 3

#define MSG_SIZE sizeof(struct mensaje)-sizeof(long)
#define CANT 15

#define COLOR_WRITER "\033[1;34m"  // Azul
#define COLOR_READER "\033[1;32m"  // Verde
#define COLOR_RESET "\033[0m"      // Reset

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
