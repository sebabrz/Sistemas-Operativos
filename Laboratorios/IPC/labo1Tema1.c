#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <time.h>
#define cant 15
#define KEY ((key_t) (9999)) // MODIFICAR: elegir una key que no se pise con otro ejercicio

#define FIN 4


int matriz[cant][cant];

void generarMatriz() {
	for (int i = 0; i < cant; i++) {
		for (int j = 0; j < cant; j++) {
			matriz[i][j] = random() % 10;
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
	}
}

void imprimirMatriz() {
	for (int i = 0; i < cant; i++) {
		for (int j = 0; j < cant; j++) {
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
	}
}

// MODIFICAR SEGÚN EL EJERCICIO: contenido del mensaje
struct mensaje {
	long tipo; // NO TOCAR: siempre va primero, lo exigen msgsnd/msgrcv
	int numero;
	int x;
	int y;
};

int esPrimo(int numero) {
	if (numero == 2 || numero == 3 || numero == 5 || numero == 7) {
		return 1;
	} else
		return 0;
};

int main() {

	srandom(time(NULL));
	generarMatriz();
	imprimirMatriz();


	int msqid = msgget(KEY, IPC_CREAT | 0666);

	pid_t proceso1;
	proceso1 = fork();

	if (proceso1 == 0) {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 15; j++) {
				if (esPrimo(matriz[i][j]) == 1) {   //Si es primo , envio a la cola de mensaje
					struct mensaje num;
					int longitud = sizeof(struct mensaje) - sizeof(long);
					num.tipo = 1; num.numero = matriz[i][j]; num.x = i; num.y = j;  //arme el mensaje, lo mando
					printf("Soy el proceso 1, envie el mensaje, el numero es %d y su posicion es [%d][%d] \n", matriz[i][j], i, j);
					msgsnd(msqid, &num, longitud, 0);
				}
			}
		}

		struct mensaje num;
		int longitud = sizeof(struct mensaje) - sizeof(long);
		num.tipo = FIN;

        msgsnd(msqid, &num, longitud, 0);

		printf("finalize proceso 1 \n");
		exit(0);
	}

	pid_t proceso2;
	proceso2 = fork();

	if (proceso2 == 0) {
		for (int i = 5; i < 10; i++) {
			for (int j = 0; j < 15; j++) {
				if (esPrimo(matriz[i][j]) == 1) {   //Si es primo , envio a la cola de mensaje
					struct mensaje num;
					int longitud = sizeof(struct mensaje) - sizeof(long);
					num.tipo = 2; num.numero = matriz[i][j]; num.x = i; num.y = j; //arme el mensaje, lo mando
					printf("Soy el proceso 2, envie el mensaje, el numero es %d y su posicion es [%d][%d] \n", matriz[i][j], i, j);
					if (msgsnd(msqid, &num, longitud, 0) == -1) {
						perror("msgrcv");
						exit(1);
					}
				}
			}
		}
        //MANDO TIPO 4 , CUANDO LLEGA TIPO 4 AL PADRE, SIGNIFICA QUE TERMINE
		struct mensaje num;
		int longitud = sizeof(struct mensaje) - sizeof(long);
		num.tipo = FIN;

		msgsnd(msqid, &num, longitud, 0);

		printf("finalize proceso 2 \n");
		exit(0);
	}

	pid_t proceso3;
	proceso3 = fork();

	if (proceso3 == 0) {
		for (int i = 10; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				if (esPrimo(matriz[i][j]) == 1) {   //Si es primo , envio a la cola de mensaje
					struct mensaje num;
					int longitud = sizeof(struct mensaje) - sizeof(long);
					num.tipo = 3; num.numero = matriz[i][j]; num.x = i; num.y = j; //arme el mensaje, lo mando
					printf("Soy el proceso 3, envie el mensaje, el numero es %d y su posicion es [%d][%d] \n", matriz[i][j], i, j);
					msgsnd(msqid, &num, longitud, 0);
				}
			}
		}
		struct mensaje num;
		int longitud = sizeof(struct mensaje) - sizeof(long);
		num.tipo = FIN;

		msgsnd(msqid, &num, longitud, 0);

		printf("finalize proceso 3 \n");
		exit(0);
	}

    //----------PROCESO PADRE---------

	int cantidadNumerosPrimos = 0;
	int procesosTerminados = 0;
	struct mensaje msg;
	int longitud = sizeof(struct mensaje) - sizeof(long);

	while (procesosTerminados < 3) {
		msgrcv(msqid, &msg, longitud, 0, 0);

		if (msg.tipo == FIN) {
			procesosTerminados++;
		} else
			cantidadNumerosPrimos++;
	}

	wait(NULL);
	wait(NULL);
	wait(NULL);

	printf("Se encontraron %d numeros primos\n", cantidadNumerosPrimos);

	msgctl(msqid, IPC_RMID, NULL);
	return 0;
}