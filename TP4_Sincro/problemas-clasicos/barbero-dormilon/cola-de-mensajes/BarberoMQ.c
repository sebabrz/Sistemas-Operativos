#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define SOLICITAR_SILLA 11
#define SILLA_OCUPADA 12
#define INICIAR_CORTE 14
#define FINALIZAR_CORTE 15

#define MAX_CLIENTES 10
#define SILLAS 3

#define ROJO   "\033[31m"  // Rojo
#define AZUL   "\033[34m"  // Azul
#define VERDE  "\033[32m"  // Verde
#define RESET  "\033[0m"   // Resetear color

typedef struct {
    long tipo; // Tipo del mensaje
    int id;    // ID del cliente
} mensaje;

//Tendria q haber hecho 2 tipos de mensajes uno para cada cola, pero me dio paja, por eso esta medio raro el uso de tipos e id.

int msgid_clientes, msgid_barbero;

void gestorSillas() {
    int sillas = SILLAS; // Contador de sillas disponibles
    mensaje msg_barbero;
    mensaje msg_clientes;
    while (1) {

        // Esperar notificación de corte finalizado
        if (msgrcv(msgid_barbero, &msg_barbero, sizeof(mensaje) - sizeof(long), FINALIZAR_CORTE, IPC_NOWAIT) != -1) {
             // Si msgrcv devolvió un valor distinto de -1, significa que se recibió un mensaje correctamente
             sillas++;
            printf(ROJO "Gestor libera una silla (sillas libres: %d)\n" RESET, sillas);
            msg_clientes.tipo = msg_barbero.id;
            msg_clientes.id = FINALIZAR_CORTE;
            msgsnd(msgid_clientes, &msg_clientes, sizeof(mensaje) - sizeof(long), 0);

        }

        // Esperar solicitud de un cliente
        if (msgrcv(msgid_clientes, &msg_clientes, sizeof(mensaje) - sizeof(long), SOLICITAR_SILLA, IPC_NOWAIT)!= -1){
            
            if (sillas > 0) {
                sillas--;
                printf(ROJO "Gestor asigna una silla al cliente %d (sillas libres: %d)\n" RESET, msg_clientes.id, sillas);
                // Avisar al barbero que inicie el corte
                msg_barbero.tipo = INICIAR_CORTE;
                msg_barbero.id = msg_clientes.id;
                msgsnd(msgid_barbero, &msg_barbero, sizeof(mensaje) - sizeof(long), 0);
            } else {
                printf(ROJO "Gestor informa que no hay sillas para el cliente %d\n" RESET, msg_clientes.id);
                // Avisar al cliente que no hay sillas disponibles
                msg_clientes.tipo = msg_clientes.id;
                msg_clientes.id = SILLA_OCUPADA;
                msgsnd(msgid_clientes, &msg_clientes, sizeof(mensaje) - sizeof(long), 0);
            }
        }
        
    }
}

void *cliente(void *arg) {
    int id = *((int *)arg);
    mensaje msg;
    msg.id = id;
    sleep(rand() % MAX_CLIENTES + 1); // Esperar una cantidad aleatoria de tiempo entre 1 y MAX_CLIENTES
    printf(AZUL "Cliente %d solicita una silla\n" RESET, id);
        
    // Configurar el tipo de solicitud
    msg.tipo = SOLICITAR_SILLA;
    msgsnd(msgid_clientes, &msg, sizeof(mensaje) - sizeof(long), 0);

    msgrcv(msgid_clientes, &msg, sizeof(mensaje) - sizeof(long), id, 0);
    if (msg.id == FINALIZAR_CORTE) {
    printf(AZUL "Cliente %d se va luego de recibir el corte.\n" RESET, id);}
    if (msg.id == SILLA_OCUPADA ) {
    printf(AZUL "Cliente %d se va porque no hay sillas disponibles\n" RESET, id);}
    
    
}

void *barbero(void *arg) {
    mensaje msg;
    while (1) {
        printf(VERDE "El barbero está durmiendo\n" RESET);
        sleep(2);
        // Esperar cliente
        msgrcv(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), INICIAR_CORTE, 0);
        printf(VERDE "El barbero se despierta\n" RESET);

        //El barbero realiza 3 cortes antes de volverse a dormir

        //CORTE 1
        printf(VERDE "El barbero atiende al cliente %d\n" RESET, msg.id);
        printf(VERDE "El barbero finalizó el corte del cliente %d\n"RESET , msg.id);
        msg.tipo = FINALIZAR_CORTE;
        msgsnd(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), 0); // Avisar al gestor que el corte ha terminado

        //CORTE 2
        msgrcv(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), INICIAR_CORTE, 0);
        printf(VERDE "El barbero atiende al cliente %d\n"RESET, msg.id);
        printf(VERDE"El barbero finalizó el corte del cliente %d\n"RESET, msg.id);
        msg.tipo = FINALIZAR_CORTE;
        msgsnd(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), 0); // Avisar al gestor que el corte ha terminado

        //CORTE 3
        msgrcv(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), INICIAR_CORTE, 0);
        printf(VERDE"El barbero atiende al cliente %d\n"RESET, msg.id);
        printf(VERDE"El barbero finalizó el corte del cliente %d\n"RESET, msg.id);
        msg.tipo = FINALIZAR_CORTE;
        msgsnd(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), 0); // Avisar al gestor que el corte ha terminado

        printf(VERDE"El barbero se va a dormir\n"RESET);

    }
}

int main() {
    key_t key_clientes = ftok("barberia_clientes", 65); // Clave para la cola de mensajes de clientes
    key_t key_barbero = ftok("barberia_barbero", 66); // Clave para la cola de mensajes del barbero
    msgid_clientes = msgget(key_clientes, 0666 | IPC_CREAT); // Crear cola de mensajes para clientes
    msgid_barbero = msgget(key_barbero, 0666 | IPC_CREAT); // Crear cola de mensajes para barbero
    mensaje msg;

    while (msgrcv(msgid_barbero, &msg, sizeof(mensaje) - sizeof(long), 0, IPC_NOWAIT) != -1);
    while (msgrcv(msgid_clientes, &msg, sizeof(mensaje) - sizeof(long), 0, IPC_NOWAIT) != -1);

    pthread_t th_gestor, th_barbero, th_clientes[MAX_CLIENTES];
    int ids[MAX_CLIENTES];

    // Crear hilos
    pthread_create(&th_gestor, NULL, (void *)gestorSillas, NULL);
    pthread_create(&th_barbero, NULL, barbero, NULL);

    for (int i = 0; i < MAX_CLIENTES; i++) {
        ids[i] = i + 1;
        pthread_create(&th_clientes[i], NULL, cliente, &ids[i]);
    }

    // Esperar hilos de clientes
    for (int i = 0; i < MAX_CLIENTES; i++) {
        pthread_join(th_clientes[i], NULL);
    }
    //Esperar hilo barbero
    pthread_join(th_barbero, NULL);

    //Esperarr hilo gestor
    pthread_join(th_gestor, NULL);

    // Limpiar recursos
    msgctl(msgid_clientes, IPC_RMID, NULL);
    msgctl(msgid_barbero, IPC_RMID, NULL);

    return 0;
}