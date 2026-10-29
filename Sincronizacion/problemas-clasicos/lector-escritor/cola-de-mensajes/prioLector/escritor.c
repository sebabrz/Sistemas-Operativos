#include "estructura.h"

int main(int argsc, char * argv[]){
	int id = atoi(argv[1]);
	key_t clave = generateKey();

	int cola = msgget(clave, 0666);

	if (cola == -1) {
		perror("Error al obtener la cola de mensajes");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));
	
	struct mensaje msgRcv, msgSnd, msgMutex;
	
	
	while (1){
        
        
		msgrcv(cola, &msgRcv, MSG_SIZE, RW, 0);
        printf(COLOR_WRITER "Escritor %d se encuentra escribiendo.\n" COLOR_RESET, id); 
        msgSnd.tipo=RW;
        msgsnd(cola, &msgSnd, MSG_SIZE,0);
        printf(COLOR_WRITER "Escritor %d se va.\n" COLOR_RESET, id);
        sleep(1);
    }

	return 0;
}

