#include "estructura.h"

int main(){
	char nroEscritor[3];
	char nroLector[3];
	char *argsLector[] = {"./lector", nroLector, NULL};
	char *argsEscritor[] = {"./escritor",nroEscritor, NULL};

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
	msg.tipo = MUTEX;
	msgsnd(cola, &msg, MSG_SIZE, 0);
	msg.tipo = RW;
	msgsnd(cola, &msg, MSG_SIZE, 0);

	// Crear escritor
	for(int i = 0; i < CANT; i++){
	int pidE = fork();
		if(pidE == 0){
			sprintf(nroEscritor, "%d", i + 1);
			execv("./escritor", argsEscritor);
			printf("error");
			exit(4);
		}
	}
	
	// Crear lectores
	for(int i = 0; i < CANT; i++){
		int pidL = fork();
		if(pidL == 0){
			sprintf(nroLector, "%d", i + 1);
			execv("./lector", argsLector);
			printf("error");
			exit(4);
		}
	}

	for(int i = 0; i < CANT*2; i++){
		wait(NULL);
	}
	wait(NULL);

	msgctl(cola, IPC_RMID, NULL);

	return 0;
}	
