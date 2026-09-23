#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <time.h>
#define cant 15
#define KEY ((key_t) (9999)) // MODIFICAR: elegir una key que no se pise con otro ejercicio

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

int esPrimo(int numero){
    if(numero==2 || numero==3 || numero==5 || numero==7){
        return 1;
    } else
    return 0;
};

int main()
{
    // NO TOCAR: creación/asociación de la cola
    srandom(time(NULL));
    generarMatriz();
    imprimirMatriz();

    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    struct mensaje msg;
    int longitud = sizeof(struct mensaje) - sizeof(long); // NO TOCAR: el tipo no cuenta como dato

    pid_t pid1;
    pid_t pid2;
    pid_t pid3;
    
    pid1 = fork();

    if (pid1 < 0) {
        printf("Error");
        exit(1);
    }

    if (pid1 == 0) {
        // ===== LÓGICA DEL EMISOR (padre) =====
        // TODO: armar el/los mensajes y mandarlos con msgsnd
        // msg.tipo = 1;                      // MODIFICAR: tipo de mensaje
        // msg.dato = /* ... */;               // MODIFICAR: contenido
        msg.tipo = 1;
        msg.numero = 0;
        msg.x=0;
        msg.y=0;
        int longitud1 = sizeof(struct mensaje) - sizeof(long);
        for(int i=0; i<5; i++){
            for(int j=0; j<15; j++){
                if(esPrimo(matriz[i][j])==1){
                    msg.numero=matriz[i][j];
                    msg.x=i;
                    msg.y=j;                   
                    msgsnd(msqid, &msg, longitud1, 0);
                    printf("envie el nro %d\n",matriz[i][j]);
                }
            }
        }
        exit(0);

    } else if (pid1 > 0) {
        pid2=fork();
        if (pid2 < 0) {
            printf("Error");
            exit(1);
        }
        if(pid2==0){
            msg.tipo = 1;
            msg.numero = 0;
            msg.x=0;
            msg.y=0;
            int longitud2 = sizeof(struct mensaje) - sizeof(long);
            for(int i=5; i<10; i++){
                for(int j=0; j<15; j++){
                    if(esPrimo(matriz[i][j])==1){
                        msg.numero=matriz[i][j];
                        msg.x=i;
                        msg.y=j;                  
                        msgsnd(msqid, &msg, longitud2, 0);
                        printf("envie el nro %d\n",matriz[i][j]);
                    }
                }
            }
            exit(0);
        }else if(pid2>0){
            pid3=fork();
            if (pid3 < 0) {
                printf("Error");
                exit(1);
            }
            if(pid3==0){
                msg.tipo = 1;
                msg.numero = 0;
                msg.x=0;
                msg.y=0;
                int longitud3 = sizeof(struct mensaje) - sizeof(long);
                for(int i=10; i<15; i++){
                    for(int j=0; j<15; j++){
                        if(esPrimo(matriz[i][j])==1){
                            msg.numero=matriz[i][j];
                            msg.x=i;
                            msg.y=j;                  
                            msgsnd(msqid, &msg, longitud3, 0);
                            printf("envie el nro %d\n",matriz[i][j]);
                        }
                    }
                }   
                exit(0);
            }
        }
    
    wait(NULL);
    wait(NULL);
    wait(NULL);
    int cantidadPrimos=0;
    //mientras tenga mensajes, la cola no esta vacia, no espero si no tengo mensaje spor eso el IPC_NOWAIT
    while(msgrcv(msqid, &msg, longitud, 1, IPC_NOWAIT) != -1){
        cantidadPrimos++;
    } 
    printf("Se encontraron %d numeros primeros \n", cantidadPrimos);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;

}
}