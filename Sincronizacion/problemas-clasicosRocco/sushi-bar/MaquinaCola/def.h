
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

#define CANT 10
#define CANT_PERSONAS_MAX 1
#define CANT_PERSONAS_USANDO 2
#define MAQUINA 3
#define MUTEX 8

    // Definir tamano de mensaje
#define msgSize sizeof(struct mensaje_t)-sizeof(long)

    struct mensaje_t {
        long type;
    };



key_t generateKey();

key_t generateKey() {
    int projectId = 14;
    char path[200];
    getcwd(path, sizeof(path));  // Obtener directorio actual
    key_t key = ftok(path, projectId);  // Generar clave unica
    return key;
}
