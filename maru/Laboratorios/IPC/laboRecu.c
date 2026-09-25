#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <string.h>
#define COMPRA 0
#define VENTA 1
#define FIN 2
#define KEY ((key_t) (9999)) // MODIFICAR: elegir una key que no se pise con otro ejercicio


// MODIFICAR SEGÚN EL EJERCICIO: contenido del mensaje
struct pedido {
    long tipo; // NO TOCAR: siempre va primero, lo exigen msgsnd/msgrcv

    int articulo;
};

void *incinerar (struct pedido p){
    char art[7];
    if(p.articulo==0){
        strcpy("de compra", art);
    }else{
        strcpy("de venta", art);
    }
    printf("Incinerando articulo %s\n", art); 
}

int main()
{   
    
    int cantidadPedidos=0;
    printf("Indique la cantidad de pedidos: \n");
    scanf("%d", &cantidadPedidos);
    // NO TOCAR: creación/asociación de la cola
    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    struct pedido ped;
    int longitud = sizeof(struct pedido) - sizeof(long); // NO TOCAR: el tipo no cuenta como dato

    for (int i=0;i<2;i++){
        pid_t generador=fork();
        if(generador==0){
            srandom(getpid());
            int longitudG = sizeof(struct pedido) - sizeof(long);
            int enviados=0;
            while(enviados!=(cantidadPedidos/2)){
            ped.tipo=random()%5+1;
            ped.articulo=random()%2;
            if (msgsnd(msqid, &ped, longitudG, IPC_NOWAIT) == -1) { perror("msgsnd"); exit(1); }
            enviados++;
            }
            exit(0);
        }
    }

   

    for(int j=0; j<5; j++){
        pid_t recibidor=fork();
        if(recibidor==0){
        int stock=0;
        int longitudR = sizeof(struct pedido) - sizeof(long);
        while(msgrcv(msqid, &ped, longitudR, j+1, IPC_NOWAIT)!=-1){
            if((ped.articulo==COMPRA) && (stock+1)<=10){
                stock++;//aumenta el stock
            }else if ((ped.articulo==COMPRA) && (stock+1)>10){
                ped.tipo=6; //tipo incinerar
                msgsnd(msqid, &ped, longitudR, IPC_NOWAIT);//mandamos a incinerar
            } else if ((ped.articulo==VENTA) && (stock-1)>=0){
                stock--;//disminuye el stock
            }else if((ped.articulo==VENTA) && (stock-1)<0){
                //aviso rechazo al padre
                ped.tipo=7; //tipo rechazar
                msgsnd(msqid, &ped, longitudR, IPC_NOWAIT);
            }
        }
        exit(0);
        }
    }
    for(int j=0; j<5; j++){
        wait(NULL);
    }

    pid_t incinerador=fork();
    if(incinerador==0){
        int longitudI = sizeof(struct pedido) - sizeof(long);
        while(msgrcv(msqid, &ped, longitudI, 6, IPC_NOWAIT)!=-1){
            incinerar(ped);
        }
        exit(0);
    }else{
        int rechazados=0;
        while(msgrcv(msqid, &ped, longitud, 7, IPC_NOWAIT)!=-1){
            rechazados++;
        }
        printf("Pedidos rechazados: %d\n", rechazados);
    }
    
    wait(NULL);
    msgctl(msqid, IPC_RMID, NULL);
    
    return 0;
}