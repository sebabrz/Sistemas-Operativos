#include "def.h"


int main() {
    char *argsPersona[] = {"./Persona", NULL};

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

        for (int i = 0; i < 5; i++) {
            struct mensaje_t msg;
            msg.type = CANT_PERSONAS_MAX;
            msgsnd(cola, &msg, msgSize, 0);
        }

        //Añado el mutex a la cola de mensajes
        struct mensaje_t msg;
        msg.type = MUTEX;
        msgsnd(cola, &msg, msgSize, 0);
        
        msg.type = MAQUINA;
        msgsnd(cola, &msg, msgSize, 0);


        for(int i =0; i<CANT; i++){
		int pidC = fork();
		if(pidC == 0){
            execv("./Persona", argsPersona);
			printf("error");
			exit(4);
			}
		}

        for (int i = 0;i<CANT;i++){
        wait(NULL);
        }

        msgctl(cola, IPC_RMID, NULL);

        return 0;
    }

