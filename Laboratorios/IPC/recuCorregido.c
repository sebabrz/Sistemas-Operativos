#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <string.h>

#define COMPRA 0
#define VENTA 1
#define KEY ((key_t) (9999)) // MODIFICAR: elegir una key que no se pise con otro ejercicio

struct pedido {
    long tipo;     // NO TOCAR: siempre va primero, lo exigen msgsnd/msgrcv
    int articulo;
};

void incinerar(struct pedido p) {
    char art[10];
    if (p.articulo == COMPRA) {
        strcpy(art, "de compra");
    } else {
        strcpy(art, "de venta");
    }
    // DEBUG: confirmar que el incinerador SOLO recibe tipo=6 (compras excedentes)
    printf("[INCINERADOR pid=%d] quemando pedido %s\n", getpid(), art);
}

void producirPedidos(int cantidad, struct pedido p, int msqid) {
    srandom(getpid()); // semilla distinta por proceso, sino los 2 generadores tiran los mismos numeros
    int longitudG = sizeof(struct pedido) - sizeof(long);
    int enviados = 0;

    // DEBUG: confirmar que cada generador arranca con la mitad que le corresponde
    printf("[GENERADOR pid=%d] voy a mandar %d pedidos\n", getpid(), cantidad);

    while (enviados != cantidad) {
        p.tipo = random() % 5 + 1;   // articulo 1 a 5
        p.articulo = random() % 2;   // 0=COMPRA, 1=VENTA

        // DEBUG: ver CADA pedido que se genera, para poder despues sumarlos a mano
        // y comparar contra lo que reportan los receptores (deberian coincidir)
        printf("[GENERADOR pid=%d] envio #%d -> articulo=%ld tipo=%s\n",
               getpid(), enviados + 1, p.tipo, p.articulo == COMPRA ? "COMPRA" : "VENTA");

        if (msgsnd(msqid, &p, longitudG, 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        enviados++;
    }

    // DEBUG: si esto no aparece en la salida, el exit() no se esta llamando
    // (era el bug de la fork bomb de la version anterior)
    printf("[GENERADOR pid=%d] termine, hago exit\n", getpid());
    exit(0);
}

int main() {
    int cantidadPedidos = 0;

    printf("Indique la cantidad de pedidos: \n");
    scanf("%d", &cantidadPedidos);

    int mitad1 = cantidadPedidos / 2;
    int mitad2 = cantidadPedidos - mitad1;

    // DEBUG: confirmar que el reparto suma el total original
    printf("[PADRE] cantidadPedidos=%d -> mitad1=%d mitad2=%d (suma=%d)\n",
           cantidadPedidos, mitad1, mitad2, mitad1 + mitad2);

    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }
    // DEBUG: con este id podes chequear "ipcs -q" desde otra terminal
    printf("[PADRE] cola creada con msqid=%d (key=%d)\n", msqid, (int) KEY);

    struct pedido ped;
    int longitud = sizeof(struct pedido) - sizeof(long);

    pid_t generador1 = fork();
    if (generador1 == 0) {
        producirPedidos(mitad1, ped, msqid);
    }
    printf("[PADRE] cree generador1 pid=%d\n", generador1);

    pid_t generador2 = fork();
    if (generador2 == 0) {
        producirPedidos(mitad2, ped, msqid);
    }
    printf("[PADRE] cree generador2 pid=%d\n", generador2);

    for (int j = 0; j < 5; j++) {
        pid_t recibidor = fork();
        if (recibidor == 0) {
            int stock = 0;
            int longitudR = sizeof(struct pedido) - sizeof(long);

            // DEBUG: confirmar que hay 5 receptores, uno por articulo (j+1)
            printf("[RECEPTOR articulo=%d pid=%d] arranco, escuchando tipo=%d\n", j + 1, getpid(), j + 1);

            while (1) {
                msgrcv(msqid, &ped, longitudR, j + 1, 0);

                // DEBUG: la linea mas importante para ver el ejercicio funcionando.
                // te deja ver, para CADA articulo, la secuencia completa de su stock
                printf("[RECEPTOR articulo=%d pid=%d] recibi %s | stock antes=%d\n", j + 1, getpid(), ped.articulo == COMPRA ? "COMPRA" : "VENTA", stock);

                if ((ped.articulo == COMPRA) && (stock + 1) <= 10) {
                    stock++;
                    printf("[RECEPTOR articulo=%d] stock ahora=%d\n", j + 1, stock);
                } else if ((ped.articulo == COMPRA) && (stock + 1) > 10) {
                    printf("[RECEPTOR articulo=%d] stock lleno (10), mando a incinerar\n", j + 1);
                    ped.tipo = 6;
                    msgsnd(msqid, &ped, longitudR, 0);
                } else if ((ped.articulo == VENTA) && (stock - 1) >= 0) {
                    stock--;
                    printf("[RECEPTOR articulo=%d] stock ahora=%d\n", j + 1, stock);
                } else if ((ped.articulo == VENTA) && (stock - 1) < 0) {
                    printf("[RECEPTOR articulo=%d] sin stock, rechazo venta\n", j + 1);
                    ped.tipo = 7;
                    msgsnd(msqid, &ped, longitudR, 0);
                }
            }
        }
    }

    pid_t incinerador = fork();
    if (incinerador == 0) {
        int longitudI = sizeof(struct pedido) - sizeof(long);
        printf("[INCINERADOR pid=%d] arranco, escuchando tipo=6\n", getpid());
        while (1) {
            msgrcv(msqid, &ped, longitudI, 6, 0);
            incinerar(ped);
        }
    } else {
        int rechazados = 0;
        printf("[RECHAZOS pid=%d] arranco, escuchando tipo=7\n", getpid());
        while (1) {
            msgrcv(msqid, &ped, longitud, 7, 0);
            rechazados++;
            printf("[RECHAZOS pid=%d] rechace pedido. Total rechazados=%d\n", getpid(), rechazados);
        }
    }

    // el padre espera igual a los hijos
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}