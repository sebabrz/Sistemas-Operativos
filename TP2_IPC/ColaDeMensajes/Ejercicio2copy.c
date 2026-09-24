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

#define KEY ((key_t)(1243))

/*
    Extension del ejercicio anterior de colas de mensajes: se agrega
    un cuarto proceso que puede leer cualquiera de los dos tipos de
    mensaje. Al finalizar, cada proceso indica la cantidad de mensajes
    que leyo.
*/

struct mensaje {
    long tipo;
    int dato;
};

int main()
{

    srandom(time(NULL));

    //Creo la cola de mensajes
    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    pid_t pid;

    //Creo 3 hijos, que van a recibir de tipos:
    //tipo 0, recibi cualquier tipo
    //tipo 1, de tipo 1
    //tipo 2, recibe de tipo 2
    //lo bueno que al usar el i del for, ya contemplo todo
    for (int i=0; i < 3; i++){

        pid = fork ();  //apenas se ejecuta el FORK, deja de existir, el for y pasa a la instruccion del hijo

        if (pid == 0) {
            //soy uno de los hijos
            int cantidadMensajes = 0;
            while(1){
            
                struct mensaje msg;
                int longitud = sizeof(struct mensaje) - sizeof(long);

                if (msgrcv(msqid, &msg, longitud, i, 0) == -1) { 
                    perror("msgrcv"); exit(1); }
                
                printf("[%d] Soy el proceso %d Recibi el mensaje %d soy de tipo %ld \n",cantidadMensajes, i, msg.dato, msg.tipo );
                cantidadMensajes++;
            }
        }
    }

    int cantidadMensajes = 0;

    while (1) {
        int i=0;
        struct mensaje msg;
        int longitud = sizeof(struct mensaje) - sizeof(long); // NO TOCAR: el tipo no cuenta como dato

        msg.tipo = random() % 2 + 1;
        msg.dato = random() % 9;


        //Una vez armado el mensaje, lo mando, chequeando por las dudas si da error.
        if (msgsnd(msqid, &msg, longitud, 0) == -1) {
            printf("Error al mandar el mensaje\n");
        }

        printf ("[%d] Se envio mensajes a la cola de tipo %ld el mensaje es %d \n" , cantidadMensajes, msg.tipo, msg.dato );
        cantidadMensajes++;

        sleep(1); 
    }

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}