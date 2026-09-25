#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, exit, random
#include <unistd.h> // fork, exec, getpid, sleep
#include <sys/wait.h> // wait, waitpid
#include <pthread.h> // hilos POSIX
#include <sys/types.h> // pid_t y otros tipos
#include <time.h> // time, medicion de tiempo
#include <stdint.h> // enteros de tamaño fijo
#include <sys/mman.h> // memoria compartida (mmap)
#include <fcntl.h> // flags de apertura (O_CREAT, etc.)
#include <sys/stat.h> // permisos de archivos
#include <sys/shm.h> // memoria compartida System V
#include <string.h> // manejo de strings
#include <sys/time.h> // gettimeofday
#include <math.h> // funciones matematicas
#include <dirent.h> // lectura de directorios
#include <sys/msg.h>

#define KEY ((key_t)(13))
#define ALTA 1
#define MEDIA 2
#define BAJA 3
#define FIN 4

struct pedido {
    long prioridad;
    char comida[30];
};

int main()
{   
    pid_t hamburguesa;
    pid_t papas;
    pid_t pizza;
    srandom(time(NULL));
    int msqid = msgget(KEY, IPC_CREAT | 0666);
    int ham = 0;
    int pa = 0;
    int pi = 0;
    printf("Ingrese la cantidad de hamburguesas: \n");
    scanf(" %d", &ham);
    printf("Ingrese la cantidad de papas: \n");
    scanf(" %d", &pa);
    printf("Ingrese la cantidad de pizzas: \n");
    scanf(" %d", &pi);
    int pedidos = ham + pa + pi;

    if (msqid == -1) {
        printf("La cola no pudo crearse");
        exit(1);
    }
    printf("Soy el proceso padre mi pid es: %d \n", getpid());

    for(int i=0;i<2;i++){
        struct pedido hamburguesas[ham];
        int longitudH;
        hamburguesa = fork();
        if(hamburguesa==0){
            for (int i = 0; i < ham; i++) {
            sleep(1);
            longitudH = sizeof(struct pedido) - sizeof(long);
            hamburguesas[i].prioridad = BAJA;
            strcpy(hamburguesas[i].comida, "hamburguesa");
            printf("Se envio un pedido de hamburguesas \n");
            msgsnd(msqid, &hamburguesas[i], longitudH, 0);
            
        }
        exit(0);
        }  
    }
    
    
    papas = fork();
        if (papas == 0) {
            struct pedido papas[pa];
            int longitudPa;
            for (int i = 0; i < pa; i++) {
                sleep(5);
                longitudPa = sizeof(struct pedido) - sizeof(long);
                papas[i].prioridad = MEDIA;
                strcpy(papas[i].comida, "papas");
                printf("Se envio un pedido de papas \n");
                msgsnd(msqid, &papas[i], longitudPa, 0);
               
            }
            exit(0);
        } 
    
    pizza = fork();
        if (pizza == 0) {
            struct pedido pizzas[pi];
            int longitudPi;
            for (int i = 0; i < pi; i++) {
                    sleep(10);
                    longitudPi = sizeof(struct pedido) - sizeof(long);
                    pizzas[i].prioridad = ALTA;
                    strcpy(pizzas[i].comida, "pizza");
                    printf("Se envio un pedido de pizza \n");
                    msgsnd(msqid, &pizzas[i], longitudPi, 0);
                    
                }
                exit(0);
            }
            
    struct pedido receptor;
    int longitudR = sizeof(struct pedido) - sizeof(long);;
    int mostrados = 0;
    int cantidadFIN = 0;

    while (mostrados < pedidos && cantidadFIN!=3) {
                /*
                if (msgrcv(msqid, &receptor, longitudR, ALTA, IPC_NOWAIT) != -1) {
                    printf("Pedido N°: %d, Pedido: %s, Prioridad: %ld\n", mostrados + 1, receptor.comida, receptor.prioridad);
                    mostrados++;
                    sleep(random() % 3 + 3);
                } else if (msgrcv(msqid, &receptor, longitudR, MEDIA, IPC_NOWAIT) != -1) {
                    printf("Pedido N°: %d, Pedido: %s, Prioridad: %ld\n", mostrados + 1, receptor.comida, receptor.prioridad);
                    mostrados++;
                    sleep(random() % 3 + 3);
                } else if (msgrcv(msqid, &receptor, longitudR, BAJA, IPC_NOWAIT) != -1) {
                    printf("Pedido N°: %d, Pedido: %s, Prioridad: %ld\n", mostrados + 1, receptor.comida, receptor.prioridad);
                    mostrados++;
                    sleep(random() % 3 + 3);
                }
                    */
        msgrcv(msqid, &receptor, longitudR, -4, 0);
        if(receptor.prioridad==4){
            cantidadFIN++;
        }else{
            printf("Pedido N°: %d, Pedido: %s, Prioridad: %ld\n", mostrados + 1, receptor.comida, receptor.prioridad);
            mostrados++;
            sleep(random() % 3 + 3);
        }
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);
    msgctl(msqid, IPC_RMID, NULL); //libera la cola bye bye

    return 0;
}