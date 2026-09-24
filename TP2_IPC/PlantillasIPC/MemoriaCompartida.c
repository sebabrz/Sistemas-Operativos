#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY ((key_t) (9999)) // MODIFICAR: elegir una key que no se pise con otro ejercicio
#define CANT_PROCESOS 1      // MODIFICAR: cantidad de procesos hijos a crear

// MODIFICAR SEGÚN EL EJERCICIO: qué datos van en el segmento compartido
struct segmento {
    int dato;
};

int main()
{
    // NO TOCAR: creación del segmento
    int shmid = shmget(KEY, sizeof(struct segmento), IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Error al crear el segmento de memoria\n");
        exit(1);
    }

    // NO TOCAR: vinculación del segmento al espacio de direcciones del proceso
    struct segmento *ptr = (struct segmento *) shmat(shmid, 0, 0);
    if (ptr == (void *) -1) {
        printf("Error al vincular el segmento de memoria\n");
        exit(2);
    }

    // ===== INICIALIZACIÓN (antes de crear los hijos) =====
    // TODO: cargar en ptr los datos de partida del ejercicio (ej. generar matrices, valores base)

    int pid;
    for (int i = 0; i < CANT_PROCESOS; i++) {
        pid = fork();

        if (pid == 0) {
            // ===== LÓGICA DEL HIJO "i" =====
            // TODO: implementar lo que pide el ejercicio, escribiendo/leyendo sobre ptr
            // Importante: si hay varios hijos, que cada uno toque una parte distinta de ptr
            // (una fila, un campo propio, etc.) para no pisarse sin necesitar sincronización.

            shmdt(ptr); // NO TOCAR: el hijo se desvincula antes de terminar
            exit(0);
        }
    }

    if (pid > 0) {
        // ===== LÓGICA DEL PADRE (una vez que todos los hijos terminaron) =====
        for (int i = 0; i < CANT_PROCESOS; i++) {
            wait(NULL);
        }

        // TODO: usar/imprimir el resultado final que quedó en ptr

        // NO TOCAR: liberación del recurso
        shmdt(ptr);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}