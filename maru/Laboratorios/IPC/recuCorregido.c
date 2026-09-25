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

void producirPedidos(int m, struct pedido p, int msqid){
    srandom(getpid());
    int cantidad=m;
    int longitudG = sizeof(struct pedido) - sizeof(long);
    int enviados=0;
    while(enviados!=cantidad){
    p.tipo=random()%5+1;
    p.articulo=random()%2;
    if (msgsnd(msqid, &p, longitudG, 0) == -1) { perror("msgsnd"); exit(1); }
    enviados++;
    }
    exit(0);
}

int main()
{   
    
    int cantidadPedidos=0;
    int mitad1=0;
    int mitad2=0;
    printf("Indique la cantidad de pedidos: \n");
    mitad1=cantidadPedidos/2;
    mitad2=cantidadPedidos-mitad1;
    scanf("%d", &cantidadPedidos);
    // NO TOCAR: creación/asociación de la cola
    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    struct pedido ped;
    int longitud = sizeof(struct pedido) - sizeof(long); // NO TOCAR: el tipo no cuenta como dato
    pid_t generador1;
    pid_t generador2;

    generador1=fork();
    if(generador1==0){
        producirPedidos(mitad1, ped, msqid);
    }

    generador2=fork();
    if(generador2==0){
        producirPedidos(mitad2, ped, msqid);
    }

    for(int j=0; j<5; j++){
        pid_t recibidor=fork();
        if(recibidor==0){
        int stock=0;
        int longitudR = sizeof(struct pedido) - sizeof(long);
        while(msgrcv(msqid, &ped, longitudR, j+1, 0)!=-1){
            if((ped.articulo==COMPRA) && (stock+1)<=10){
                stock++;//aumenta el stock
            }else if ((ped.articulo==COMPRA) && (stock+1)>10){
                ped.tipo=6; //tipo incinerar
                msgsnd(msqid, &ped, longitudR, 0);//mandamos a incinerar
            } else if ((ped.articulo==VENTA) && (stock-1)>=0){
                stock--;//disminuye el stock
            }else if((ped.articulo==VENTA) && (stock-1)<0){
                //aviso rechazo al padre
                ped.tipo=7; //tipo rechazar
                msgsnd(msqid, &ped, longitudR, 0);
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
        while(msgrcv(msqid, &ped, longitudI, 6, 0)!=-1){
            incinerar(ped);
        }
        exit(0);
    }else{
        int rechazados=0;
        while(msgrcv(msqid, &ped, longitud, 7, 0)!=-1){
            rechazados++;
        }
        printf("Pedidos rechazados: %d\n", rechazados);
    }
    wait(NULL);

    msgctl(msqid, IPC_RMID, NULL);
    
    return 0;
}