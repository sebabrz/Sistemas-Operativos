#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define ABEJAS 3
#define TARRO 10

#define TIME 300000

#define ABEJA_QUIERE_ENTRAR 1
#define ABEJA_ENTRA 2
#define ABEJA_SALE 3
#define OSO_ENTRA 4
#define OSO_SALE 5

#define YELLOW "\033[0;33m"
#define RESET "\033[0m"
#define BROWN "\033[0;31m" // Código ANSI estándar para rojo

typedef struct {
    long mtype;
    pid_t pid;
} mensaje;

int msgid;

void *abeja() {
    mensaje msg;
    while(1){
        msg.mtype = ABEJA_QUIERE_ENTRAR;
        //Envia el mensaje para intentar entrar
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
        //Espera respuesta
        msgrcv(msgid, &msg, sizeof(msg.pid), ABEJA_ENTRA, 0);
        printf(YELLOW "\nAbeja %ld entra al tarro y pone miel.\n" RESET, (long)pthread_self());
        usleep(TIME);
        //Enviar mensaje de salida
        msg.mtype = ABEJA_SALE;
        printf(YELLOW"Abeja %ld sale del tarro.\n \n" RESET, (long)pthread_self());
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
    }
}

void *oso() {
    mensaje msg;
    while(1){
        msgrcv(msgid, &msg, sizeof(msg.pid), OSO_ENTRA, 0);
        printf(BROWN "Oso entra al tarro y come miel.\n"RESET );
        usleep(TIME);
        //Enviar mensaje de salida
        msg.mtype = OSO_SALE;
        printf(BROWN"Oso sale del tarro.\n"RESET );
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
    }
}

void *gestor_tarro(){
    int lugaresTarro = 0;  
    mensaje msg; 
    while(1){
        while ( lugaresTarro < TARRO){
            msgrcv(msgid, &msg, sizeof(msg.pid), ABEJA_QUIERE_ENTRAR, 0);
            msg.mtype = ABEJA_ENTRA;
            msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            lugaresTarro++;
            msgrcv(msgid, &msg, sizeof(msg.pid), ABEJA_SALE, 0);
            printf("Lugares en el tarro %d / %d\n", lugaresTarro, TARRO);
        }
        printf("El tarro esta lleno, el oso puede entrar.\n \n");
        msg.mtype = OSO_ENTRA;
        msgsnd(msgid, &msg, sizeof(msg.pid), 0);
        msgrcv(msgid, &msg, sizeof(msg.pid), OSO_SALE, 0);
        lugaresTarro = 0;
   }
}

int main() {
    pthread_t abejas[ABEJAS];
    pthread_t t_oso;
    pthread_t gestor;
    key_t key = ftok("OsoYAbejas", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);
    mensaje msg;
    while (msgrcv(msgid, &msg, sizeof(mensaje) - sizeof(long), 0, IPC_NOWAIT) != -1); // Limpiar la cola de mensajes

    for (int i = 0; i < ABEJAS; i++) {
        pthread_create(&abejas[i], NULL, abeja, NULL);
    }
    pthread_create(&t_oso, NULL, oso, NULL);
    pthread_create(&gestor, NULL, gestor_tarro, NULL);

    for (int i = 0; i < ABEJAS; i++) {
        pthread_join(abejas[i], NULL);
    }
    pthread_join(t_oso, NULL);
    pthread_join(gestor, NULL);
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}