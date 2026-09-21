#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#define KEY ((key_t) (9999)) // MODIFICAR: elegir una key que no se pise con otro ejercicio

// MODIFICAR SEGÚN EL EJERCICIO: contenido del mensaje
struct mensaje {
    long tipo; // NO TOCAR: siempre va primero, lo exigen msgsnd/msgrcv
    int dato;
};

int main()
{
    // NO TOCAR: creación/asociación de la cola
    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    struct mensaje msg;
    int longitud = sizeof(struct mensaje) - sizeof(long); // NO TOCAR: el tipo no cuenta como dato

    pid_t pid;
    pid = fork();

    if (pid < 0) {
        printf("Error");
        exit(1);
    }

    if (pid > 0) {
        // ===== LÓGICA DEL EMISOR (padre) =====
        // TODO: armar el/los mensajes y mandarlos con msgsnd
        // msg.tipo = 1;                      // MODIFICAR: tipo de mensaje
        // msg.dato = /* ... */;               // MODIFICAR: contenido
        // if (msgsnd(msqid, &msg, longitud, 0) == -1) { perror("msgsnd"); exit(1); }

        wait(NULL);

        // NO TOCAR: limpieza del recurso (solo quien crea la cola la destruye, al final de todo)
        msgctl(msqid, IPC_RMID, NULL);

    } else if (pid == 0) {
        // ===== LÓGICA DEL RECEPTOR (hijo) =====
        // TODO: recibir mensajes con msgrcv y procesarlos
        // Bloqueante, tipo exacto:        msgrcv(msqid, &msg, longitud, 1, 0);
        // No bloqueante, cualquier tipo:  msgrcv(msqid, &msg, longitud, 0, IPC_NOWAIT);
        // Prioridad (tipo <= N):          msgrcv(msqid, &msg, longitud, -N, IPC_NOWAIT);
        // if (msgrcv(msqid, &msg, longitud, 0, 0) == -1) { perror("msgrcv"); exit(1); }
        // printf("Recibi: %d, tipo %ld\n", msg.dato, msg.tipo);

        exit(0);
    }

    return 0;
}
