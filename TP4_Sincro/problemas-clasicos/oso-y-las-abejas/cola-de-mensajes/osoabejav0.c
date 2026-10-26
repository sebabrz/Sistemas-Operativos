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

key_t key;
int msgid;

typedef struct {
    long type;
    int miel;
} mensaje_t;

size_t msgSize = sizeof(mensaje_t) - sizeof(long);

void abeja(int id) {
    mensaje_t msgRcv, msgSnd, msgMutex;
    while (1) {
        sleep(1);  // Simula el tiempo de producción de miel

        // Espera a que haya espacio en el tarro
        msgrcv(msgid, &msgRcv, msgSize, VACIO, 0);

        msgrcv(msgid, &msgRcv, msgSize, TURNO, 0); //Para colocar miel necesito que haya algun turno habilitado.
        msgSnd.type = TURNO;
        msgsnd(msgid,&msgSnd,msgSize,0); // devuelvo el turno inmediatamente.

        // Produce miel
        msgSnd.type = LLENO;
        msgSnd.miel = 1;
        printf("Abeja %d GUARDA miel\n", id);
        msgsnd(msgid, &msgSnd, msgSize, 0);

        msgrcv(msgid, &msgMutex, msgSize, MUTEX, 0); // tomo el mutex para realizar una operacion que debe estar protegida

        msgrcv(msgid, &msgRcv, msgSize, TURNO, 0); //Para chequear si debo despertar o no al oso tambien necesito que haya algun turno habilitado.
        msgSnd.type = TURNO;
        msgsnd(msgid,&msgSnd,msgSize,0); // devuelvo el turno inmediatamente.

        // Verifica si el tarro está lleno
        if (msgrcv(msgid, &msgRcv, msgSize, VACIO, IPC_NOWAIT) == -1) {
            // Si no hay más mensajes VACIO, el tarro está lleno
            printf("Abeja %d despierta al oso\n", id);
            msgSnd.type = DESPERTAR_OSO;
            msgsnd(msgid, &msgSnd, msgSize, 0);
        } else {
            // Si aún hay espacio, devuelve el mensaje VACIO
            msgsnd(msgid, &msgRcv, msgSize, 0);
        }
        msgsnd(msgid,&msgMutex,msgSize,0); // devuelvo el mutex
    }
}

void oso() {
    mensaje_t msgRcv, msgSnd;
    while (1) {
        // Espera a que el tarro esté lleno
        msgrcv(msgid, &msgRcv, msgSize, DESPERTAR_OSO, 0);

        printf("El oso se despierta\n");

        for (int i = 0;i< ABEJAS;i++){
                msgrcv(msgid, &msgRcv, msgSize, TURNO, 0);
        }
        for (int i = 0; i < PORCIONES; i++) {
            msgrcv(msgid, &msgRcv, msgSize, LLENO, 0);
            printf("Oso COME miel %d\n", i);

            // Devuelve un espacio vacío al tarro
            msgSnd.type = VACIO;
            msgsnd(msgid, &msgSnd, msgSize, 0);
        }
        printf("El oso se comio toda la miel, ahora se va a dormir\n");

        msgSnd.type = TURNO;
        for (int i = 0;i< ABEJAS;i++){
                msgsnd(msgid,&msgSnd,msgSize,0); // devuelvo los turnos
        }
        sleep(3);
    }
}

int main() {
    key = ftok("osoyabejas", 65);
    msgid = msgget(key, 0660 | IPC_CREAT);

    // Inicializa el tarro vacío
    for (int i = 0; i < PORCIONES; i++) {
        mensaje_t msg;
        msg.type = VACIO;
        msgsnd(msgid, &msg, msgSize, 0);
    }

    //Añado el mutex a la cola de mensajes
    mensaje_t msg;
    msg.type = MUTEX;
    msgsnd(msgid, &msg, msgSize, 0);

    //Añado los turnos a la cola de mensajes
    msg.type = TURNO;
    for (int i = 0;i< ABEJAS;i++){
            msgsnd(msgid,&msg,msgSize,0);
    }

    // Crea procesos para las abejas
    for (int i = 0; i < ABEJAS; i++) {
        if (fork() == 0) {
            abeja(i + 1);
            exit(0);
        }
    }

    // Crea proceso para el oso
    if (fork() == 0) {
        oso();
        exit(0);
    }

    // Espera a que todos los procesos hijos terminen
    for (int i = 0; i < ABEJAS + 1; i++) {
        wait(NULL);
    }

    // Elimina la cola de mensajes
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
