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

#define TIME 0
#define ITER 100
#define ALUMNOS 10
#define SILLAS 3

#define semAlumno 1
#define semAula 2
#define semSilla 3
#define MUTEX 4


#define YELLOW "\033[1;33m"
#define PURPLE "\033[1;35m"
#define RESET "\033[0m"

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
