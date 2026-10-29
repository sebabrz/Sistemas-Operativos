#include "estructura.h"

int main(){
	
	char *argsHombres[] = {"./hombres", NULL};
	char *argsMujeres[] = {"./mujeres", NULL};

	srand(time(NULL));
	// Clave única para la cola de pedidos
	key_t key_q = generateKey();

	// Crear cola de mensajes
	int cola = msgget(key_q, 0666 | IPC_CREAT);

	if (cola == -1) {
		perror("Error al crear la cola de mensajes");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < MAX_BANO; i++) {
		struct mensaje msg;
		msg.tipo = MAX_H;
		msgsnd(cola, &msg, MSG_SIZE, 0);
		msg.tipo = MAX_M;
		msgsnd(cola, &msg, MSG_SIZE, 0);
	}

	struct mensaje msg;
	msg.tipo = MUTEX_H;
	msgsnd(cola, &msg, MSG_SIZE, 0);
	msg.tipo = MUTEX_M;
	msgsnd(cola, &msg, MSG_SIZE, 0);

	msg.tipo = TURNO;
	msgsnd(cola,&msg,MSG_SIZE,0);
	
	msg.tipo = BANO;
	msgsnd(cola,&msg,MSG_SIZE,0);

	// Crear hombres
	for(int i =0; i < CANT; i++){ 
		int pidH = fork();
		if(pidH == 0){
			execv("./hombres", argsHombres);
			printf("error");
		exit(4);
		}
	}
	// Crear hombres
	for(int i =0; i < CANT; i++){ 
		int pidM = fork();
		if(pidM == 0){
			execv("./mujeres", argsMujeres);
			printf("error");
		exit(4);
		}
	}


	for (int i = 0; i < 2*CANT; i++){
		wait(NULL);
	}

	msgctl(cola, IPC_RMID, NULL);

	return 0;
}	
