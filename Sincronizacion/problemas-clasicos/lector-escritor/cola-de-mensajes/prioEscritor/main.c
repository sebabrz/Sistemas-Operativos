#include "estructura.h"

int main(){
	
	char nroLector[3];
	char *argsLector[] = {"./lector", nroLector, NULL};
	char *argsEscritor[] = {"./escritor", NULL};

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

	struct mensaje msg;
	msg.tipo = MUTEXLECTORES;
	msgsnd(cola, &msg, MSG_SIZE, 0);
	msg.tipo = MUTEXESCRITORES;
	msgsnd(cola, &msg, MSG_SIZE, 0);
	msg.tipo = NOLECTORES;
	msgsnd(cola, &msg, MSG_SIZE, 0);
	msg.tipo = NOESCRITORES;
	msgsnd(cola, &msg, MSG_SIZE, 0);

	// Crear escritor
	int pidE = fork();
		if(pidE == 0){
			execv("./escritor", argsEscritor);
			printf("error");
			exit(4);
		}
	
	// Crear lectores
	for(int i = 0; i < 3; i++){
		int pidL = fork();
		if(pidL == 0){
			sprintf(nroLector, "%d", i + 1);
			execv("./lector", argsLector);
			printf("error");
			exit(4);
		}
	}

	for(int i = 0; i < 3; i++){
		wait(NULL);
	}
	wait(NULL);

	msgctl(cola, IPC_RMID, NULL);

	return 0;
}	
