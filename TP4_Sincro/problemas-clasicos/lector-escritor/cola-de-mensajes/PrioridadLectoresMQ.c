#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define NUM_ESCRITORES 3
#define NUM_LECTORES 4

#define TIEMPO_ENTRADA 1

#define ESCRITOR_ENTRADA 1
#define LECTOR_ENTRADA 2
#define ESCRITOR_INGRESA 3
#define LECTOR_INGRESA 4
#define ESCRITOR_SALIDA 5
#define LECTOR_SALIDA 6

typedef struct {
    long mtype; 
    pid_t pid;  // ID del hilo

}mensaje;

int msgid;

void* escritor() {
        mensaje msg;
        msg.mtype = ESCRITOR_ENTRADA;
        msg.pid = pthread_self();
        // Envía mensaje para intentar entrar
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
        
        // Espera respuesta
        msgrcv(msgid, &msg, sizeof(msg.pid), ESCRITOR_INGRESA, 0);
        printf("Escritor %ld escribiendo\n", (long)pthread_self());
        sleep(TIEMPO_ENTRADA); // Simula el tiempo escribiendo
        printf("Escritor %ld terminó de escribir\n", (long)pthread_self());
        sleep(TIEMPO_ENTRADA); // Simula el tiempo escribiendo
        // Enviar mensaje de salida
        msg.mtype = ESCRITOR_SALIDA;
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
}

void* lector(){
    mensaje msg ;
        msg.mtype = LECTOR_ENTRADA;
        msg.pid = pthread_self();
        // Envía mensaje para intentar entrar
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
        
        // Espera respuesta
        msgrcv(msgid, &msg, sizeof(msg.pid), LECTOR_INGRESA, 0);
        printf("Lector %ld esta leyendo\n", (long)pthread_self());
        
        sleep(TIEMPO_ENTRADA); // Simula el tiempo escribiendo
        printf("Lector %ld termino de leer\n", (long)pthread_self());

        // Enviar mensaje de salida
        msg.mtype = LECTOR_SALIDA;
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
  

}

void* gestor_buffer() {
    int num_lectores = 0, num_escritores = 0;
    mensaje msg;

    while (1) {
        // Recibe mensajes de escritores y lectores
        msgrcv(msgid, &msg, sizeof(msg.pid), 0, 0);

        if (msg.mtype == LECTOR_ENTRADA) {
            // Permitir entrada solo si no hay escritores
            if (num_escritores == 0) {
                num_lectores++;
                msg.mtype = LECTOR_INGRESA; // Indica permiso para leer
                msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            } else {
                // Mantener el mensaje para procesarlo después
                msg.mtype = LECTOR_ENTRADA;
                msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            }
        } else if (msg.mtype == ESCRITOR_ENTRADA) {
            // Permitir entrada solo si no hay escritores o lectores activos
            if (num_lectores == 0 && num_escritores == 0) {
                //printf ("Permitiendo entrada a escritor con %d lectores y %d escritores\n", num_lectores, num_escritores);
                num_escritores++;
                msg.mtype = ESCRITOR_INGRESA; // Indica permiso para escribir
                msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            } else {
                // Mantener el mensaje para procesarlo después
                msg.mtype = ESCRITOR_ENTRADA;
                msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            }
        }else if (msg.mtype == ESCRITOR_SALIDA) {
            num_escritores--;
        }else if (msg.mtype == LECTOR_SALIDA) {
            num_lectores--;
        }else {
            msgsnd(msgid, &msg, sizeof(msg.pid), 0); // Devuelve el mensaje a la cola
        }

    }
}

int main () {



    pthread_t escritores[NUM_ESCRITORES];
    pthread_t lectores[NUM_LECTORES];
    pthread_t gestor;

    key_t key = ftok("sadasdasd", 10);
    mensaje msg;
    msgid = msgget(key, 0666 | IPC_CREAT );
    msgctl(msgid, IPC_RMID, NULL);
    msgid = msgget(key, 0666 | IPC_CREAT);

    pthread_create(&gestor, NULL, gestor_buffer, NULL);

    for (int i = 0; i < NUM_LECTORES; i++){
        pthread_create(&lectores[i], NULL, lector, NULL);
    }

    for (int i = 0; i < NUM_ESCRITORES; i++){
        pthread_create(&escritores[i], NULL, escritor, NULL);
    }


    pthread_join(gestor, NULL);

    for (int i = 0; i < NUM_LECTORES; i++){
        pthread_join(lectores[i], NULL);
    }

    for (int i = 0; i < NUM_ESCRITORES; i++){
        pthread_join(escritores[i], NULL);
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
} 