#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stddef.h>

#define bufferSize 5
#define VACIO 1
#define LLENO 2
#define FIN 3

key_t key;
int msgid;

typedef struct {
    long type;
    int product;
} mensaje_t;

size_t msgSize = (sizeof(mensaje_t)-sizeof(long));


void productor(int id){
    //READ VACIO
    mensaje_t msgFin;
    msgFin.type = LLENO;
    msgFin.product = -1;
    for(int i=0;i<70;i++){
        mensaje_t msgRcv, msgSnd;
        msgrcv(msgid,&msgRcv, msgSize, VACIO,0);

        msgSnd.type = LLENO;
        msgSnd.product = rand()%100;
        printf("El productor %i produce: %i\n",id,msgSnd.product);

        msgsnd(msgid,&msgSnd, msgSize,0);
    }
    //WRITE LLENO
    msgsnd(msgid,&msgFin, msgSize,0);
}

void consumidor(int id){
    //READ LLENO
    while(1){
        mensaje_t msgRcv, msgSnd;
        msgrcv(msgid,&msgRcv, msgSize, LLENO,0);
        if(msgRcv.product == -1) break;
        printf("El consumido %i recibe: %i\n",id,msgRcv.product);
        
        msgSnd.type = VACIO;
        msgSnd.product = 0;
        msgsnd(msgid,&msgSnd, msgSize,0);
    }
    //WRITE VACIO
}
//N mensajes VACIOS
int main(){
    key = ftok("msgRoman",64);
    //Creo o vinculo la cola de mensajes
    msgid = msgget(key, 0660 | IPC_CREAT);
    
    for(int i=0;i<bufferSize;i++){
        mensaje_t msg;
        msg.type = VACIO;
        msg.product = 0;
        msgsnd(msgid, &msg, msgSize, 0);
    }
    pid_t productorP = fork();
    if(productorP == 0){
        productor(1);
        exit(0);
    }    

    pid_t consumidorP = fork();
    if(consumidorP == 0){
        consumidor(2);
        exit(0);
    }    
    wait(NULL);
    wait(NULL);

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}