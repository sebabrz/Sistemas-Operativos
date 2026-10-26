#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_FILOSOFOS 5
#define SOLICITAR_TENEDORES 1
#define LIBERAR_TENEDORES 2

#define NUM_COLORS 5
#define RESET "\033[0m"

// Arreglo de colores ANSI
const char* colores[NUM_COLORS] = {
    "\033[0;31m",  // Rojo
    "\033[0;32m",  // Verde
    "\033[0;33m",  // Amarillo
    "\033[0;34m",  // Azul
    "\033[0;35m"   // Magenta
};

typedef struct {
    int id;    // ID del filósofo
    long tipo; // Tipo de msg_data

} msg_data;

int msgid;


// Gestor de tenedores
void gestor_tenedores() {
    int tenedores[NUM_FILOSOFOS];
    msg_data msg;
    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        tenedores[i] = 1;
    }

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg), 0, 0);
        if (msg.tipo == SOLICITAR_TENEDORES) {
            if (tenedores[msg.id] == 1 && tenedores[(msg.id + 1) % NUM_FILOSOFOS] == 1) {
                tenedores[msg.id] = 0;
                tenedores[(msg.id + 1) % NUM_FILOSOFOS] = 0;
                msgsnd(msgid, &msg, sizeof(msg), 0);
            }
        } else if (msg.tipo == LIBERAR_TENEDORES) {
            tenedores[msg.id] = 1;
            tenedores[(msg.id + 1) % NUM_FILOSOFOS] = 1;
        }
    }

}

// Proceso de filósofo
void filosofo(int id) {
    msg_data msg;
    msg.id = id;

    while (1) {
        printf("\n %sEl filosofo %d esta pensando %s \n" , colores[id % NUM_COLORS], id, RESET);
        sleep(1);

        // Solicita los tenedores
        printf("\n%sEl filosofo %d quiere comer %s \n", colores[id % NUM_COLORS], id,RESET);
        msg.tipo = SOLICITAR_TENEDORES;
        msgsnd(msgid, &msg, sizeof(msg), 0);

        // Espera respuesta
        msgrcv(msgid, &msg, sizeof(msg), 0, 0);
        printf("\n %sEl filosofo %d esta comiendo %s \n", colores[id % NUM_COLORS], id, RESET);

        // Libera tenedores
        msg.tipo = LIBERAR_TENEDORES;
        msgsnd(msgid, &msg, sizeof(msg), 0);
        printf("\n %sEl filosofo %d termino de comer %s \n", colores[id % NUM_COLORS], id,RESET);
    }
}



int main() {
    pthread_t gestor;
    pthread_t filosofos[NUM_FILOSOFOS];

    // Crear cola de mensajes
    key_t key = ftok("/tmp", 'A');
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Crear hilo gestor
    pthread_create(&gestor, NULL, gestor_tenedores, NULL);

    //Crear hilos para filósofos
    for( int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_create(&filosofos[i], NULL, filosofo, (void*)i);
    }

    //Espero a que los hilos terminen
    pthread_join(gestor, NULL);

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_join(filosofos[i], NULL);
    }

    //Libero la cola de mensajes

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}