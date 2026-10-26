#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MUJERES 3
#define MAX_HOMBRES 3

#define CANT_MUJERES 5
#define CANT_HOMBRES 5

#define TIME 900000

#define MUJER_ENTRADA 1
#define HOMBRE_ENTRADA 2
#define MUJER_INGRESA 3
#define HOMBRE_INGRESA 4
#define MUJER_SALIDA 5
#define HOMBRE_SALIDA 6


typedef struct {
    long mtype;
    pid_t pid;
} mensaje;

int msgid;

void *mujer(){

    mensaje msg;
    msg.mtype = MUJER_ENTRADA;
    //Envia el mensaje para intentar entrar
    msgsnd(msgid, &msg, sizeof(msg.pid), 0);
    //Espera respuesta  
    msgrcv(msgid, &msg, sizeof(msg.pid), MUJER_INGRESA, 0);
    printf("Mujer %ld entrando al baño\n", (long)pthread_self());
    usleep(TIME);
    //Enviar mensaje de salida
    msg.mtype = MUJER_SALIDA;
    printf("Mujer %ld saliendo del baño\n", (long)pthread_self());
    msgsnd(msgid, &msg, sizeof(msg.pid), 0);


}

void *hombre(){
    mensaje msg;
    msg.mtype = HOMBRE_ENTRADA;
    //Envia el mensaje para intentar entrar
    msgsnd(msgid, &msg, sizeof(msg.pid), 0);
    //Espera respuesta
    msgrcv(msgid, &msg, sizeof(msg.pid), HOMBRE_INGRESA, 0);
    printf("Hombre %ld entrando al baño\n", (long)pthread_self());
    usleep(TIME);
    //Enviar mensaje de salida
    msg.mtype = HOMBRE_SALIDA;
    printf("Hombre %ld saliendo del baño\n", (long)pthread_self());   
    msgsnd(msgid, &msg, sizeof(msg.pid), 0);

}

void *gestor_bano(){
int cant_mujeres = 0, cant_hombres = 0;
mensaje msg;
    while(1){
        msgrcv(msgid, &msg, sizeof(msg.pid), 0, 0);   
        if (msg.mtype == MUJER_ENTRADA){
            if (cant_hombres == 0 && cant_mujeres <  MAX_MUJERES){
                cant_mujeres++;
                msg.mtype = MUJER_INGRESA;
                msgsnd(msgid, &msg, sizeof(msg.pid), 0); //Indica permiso para entrar a una mujer
            } else {
                msg.mtype = MUJER_ENTRADA;
                msgsnd(msgid, &msg, sizeof(msg.pid), 0); //Devuelve el mensaje de entrada a la cola
            }
        } else if (msg.mtype == HOMBRE_ENTRADA){
            if (cant_mujeres == 0 && cant_hombres < MAX_HOMBRES){ //Indica permiso para entrar a un hombre
                cant_hombres++;
                msg.mtype = HOMBRE_INGRESA;
                msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            } else {                                    //Devuelve el mensaje de entrada a la cola
                msg.mtype = HOMBRE_ENTRADA;
                msgsnd(msgid, &msg, sizeof(msg.pid), 0);
            }
        } else if (msg.mtype == MUJER_SALIDA){ //Sale una mujer
            cant_mujeres--; 
        } else if (msg.mtype == HOMBRE_SALIDA){ //Sale un hombre
            cant_hombres--; 
        }else if (msg.mtype == HOMBRE_ENTRADA){
            msg.mtype = HOMBRE_ENTRADA; //Devuelve el mensaje ya que no es para el gestor
            msgsnd(msgid, &msg, sizeof(msg.pid), 0);
        } else if (msg.mtype == MUJER_ENTRADA){
            msg.mtype = MUJER_ENTRADA; //Devuelve el mensaje ya que no es para el gestor
            msgsnd(msgid, &msg, sizeof(msg.pid), 0);
        }
    }
} 

int main() {

pthread_t mujeres[CANT_MUJERES];
pthread_t hombres[CANT_HOMBRES];
pthread_t gestor;
key_t key = ftok("/bin/ls", 1);
mensaje msg;
msgid = msgget(key, 0666 | IPC_CREAT);
msgctl(msgid, IPC_RMID, NULL);
msgid = msgget(key, 0666 | IPC_CREAT);

pthread_create(&gestor, NULL, gestor_bano, NULL);

for (int i = 0; i < CANT_MUJERES; i++){
    pthread_create(&mujeres[i], NULL, mujer, NULL);
}

for (int i = 0; i < CANT_HOMBRES; i++){
    pthread_create(&hombres[i], NULL, hombre, NULL);
}

for (int i = 0; i < CANT_MUJERES; i++){
    pthread_join(mujeres[i], NULL);
}

for (int i = 0; i < CANT_HOMBRES; i++){
    pthread_join(hombres[i], NULL);
}

pthread_join(gestor, NULL);

msgctl(msgid, IPC_RMID, NULL);
return 0;



}