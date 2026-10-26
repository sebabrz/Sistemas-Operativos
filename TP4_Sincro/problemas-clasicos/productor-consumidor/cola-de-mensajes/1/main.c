#include "estructura.h"

int main(){
	
	char nroProd[3];
	char nroCons[2];
	char *argsProd[] = {"./productor", nroProd, NULL};
	char *argsCons[] = {"./consumidor", nroCons, NULL};

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

	for (int i = 0; i < MAX_PRODS; i++) {
		struct mensaje msg;
		msg.tipo = VACIO;
		msgsnd(cola, &msg, MSG_SIZE, 0);
	}

	struct mensaje msg;
	msg.tipo = MUTEX;
	msgsnd(cola, &msg, MSG_SIZE, 0);

	// Crear productor
	for(int i = 0; i < 3; i++){
		int pidP = fork();
		if(pidP == 0){
			sprintf(nroProd, "%d", i + 1);
			execv("./productor", argsProd);
			printf("error");
			exit(4);
		}
	}
	
	// Crear consumidor
	for(int i = 0; i < 2; i++){
		int pidC = fork();
		if(pidC == 0){
			sprintf(nroCons, "%d", i + 1);
			execv("./consumidor", argsCons);
			printf("error");
			exit(4);
		}
	}

	wait(NULL);
	wait(NULL);

	msgctl(cola, IPC_RMID, NULL);

	return 0;
}	
