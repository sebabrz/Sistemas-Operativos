#include "def.h"


int main() {
    char id_str[10];  // Buffer to hold the string representation of id
    char *argsAbeja[] = {"./abejas", id_str, NULL};
    char *argsOso[] = {"./oso", NULL};

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

        // Inicializa el tarro vacío
        for (int i = 0; i < PORCIONES; i++) {
            struct mensaje_t msg;
            msg.type = VACIO;
            msgsnd(cola, &msg, msgSize, 0);
        }

        //Añado el mutex a la cola de mensajes
        struct mensaje_t msg;
        msg.type = MUTEX;
        msgsnd(cola, &msg, msgSize, 0);

        //Añado los turnos a la cola de mensajes
        msg.type = TURNO;
        for (int i = 0;i< ABEJAS;i++){
                msgsnd(cola,&msg,msgSize,0);
        }

        // Crear el proceso oso
        int pid_oso = fork();
        if (pid_oso == -1) {
            perror("Error al crear proceso oso");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        if (pid_oso == 0) {
            execv("./oso", argsOso);
            exit(0);
        }

        // Generar n abejas

        for(int i =0; i<ABEJAS; i++){
		int pidC = fork();
		if(pidC == 0){
            sprintf(id_str, "%d", i + 1);  // Convert id to string
            execv("./abejas", argsAbeja);
			printf("error");
			exit(4);
		}
	}



        for (int i = 0;i<ABEJAS+1;i++){
        wait(NULL);
        }

        msgctl(cola, IPC_RMID, NULL);

        return 0;
    }

