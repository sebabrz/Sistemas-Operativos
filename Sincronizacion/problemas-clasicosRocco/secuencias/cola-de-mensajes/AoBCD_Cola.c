#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stddef.h>
#define AoB 1
#define C 2
#define D 3

key_t key;
int msgid;

typedef struct{
	long tipo;	
} msg;

int msg_size = sizeof(msg) - sizeof(long);

void a(){
	while(1){
		msg msgRecibido,msgEnviado;
		msgrcv(msgid,&msgRecibido,msg_size,AoB,0);
		printf("A");
		fflush(stdout);
		msgEnviado.tipo = C;
		msgsnd(msgid,&msgEnviado,msg_size,0);
	}
		
}

void b(){
	while(1){
		msg msgRecibido,msgEnviado;
		msgrcv(msgid,&msgRecibido,msg_size,AoB,0);
		printf("B");
		fflush(stdout);
		msgEnviado.tipo = C;
		msgsnd(msgid,&msgEnviado,msg_size,0);
	}
}

void c(){
	while(1){
		msg msgRecibido,msgEnviado;
		msgrcv(msgid,&msgRecibido,msg_size,C,0);
		printf("C");
		fflush(stdout);
		msgEnviado.tipo = D;
		msgsnd(msgid,&msgEnviado,msg_size,0);
	}
}

void d(){
	while(1){
		msg msgRecibido,msgEnviado;
		msgrcv(msgid,&msgRecibido,msg_size,D,0);
		printf("D\n");
		fflush(stdout);
		sleep(1);
		msgEnviado.tipo = AoB;
		msgsnd(msgid,&msgEnviado,msg_size,0);
	}
}
int main(){
	key = ftok("Msggiulia",64);
    //Creo o vinculo la cola de mensajes
    msgid = msgget(key, 0660 | IPC_CREAT);


	if (msgid<0){
		printf("error");
		}

    //Creo procesos

    pid_t proc_a = fork();

    if (proc_a == 0){
        a();
    }

    pid_t proc_b = fork();

    if (proc_b == 0){
        b();
    }
    
    pid_t proc_c = fork();

    if (proc_c == 0){
        c();
    }
    
    pid_t proc_d = fork();

    if (proc_d == 0){
        d();
    }
    
		// meto el primer msg aob
      msg msgEnviado;
       msgEnviado.tipo = AoB;
       msgsnd(msgid,&msgEnviado,msg_size,0);   

	// Espero procesos a q terminen
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    
    //cierro la cola xd
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}

