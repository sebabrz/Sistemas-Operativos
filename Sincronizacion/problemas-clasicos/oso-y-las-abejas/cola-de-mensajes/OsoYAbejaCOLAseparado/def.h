
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

#define PORCIONES 10
#define ABEJAS 3
#define VACIO 1
#define LLENO 2
#define DESPERTAR_OSO 3
#define MUTEX 8
#define TURNO 7

    // Definir tamano de mensaje
#define msgSize sizeof(struct mensaje_t)-sizeof(long)

    struct mensaje_t {
        long type;
        int miel;
    };



key_t generateKey();

key_t generateKey() {
    int projectId = 14;
    char path[200];
    getcwd(path, sizeof(path));  // Obtener directorio actual
    key_t key = ftok(path, projectId);  // Generar clave unica
    return key;
}
