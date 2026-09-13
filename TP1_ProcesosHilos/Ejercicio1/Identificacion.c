//Crear un proceeso hijo
//Ambos procesos deben mostrar su PID, el PID del padre, un mensaje indicando si es hijo o padree

//1 es true
//0 es false

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int pid;

    pid = fork();  //llamada al sistema que duplica el proceso en ejecución y retorna un entero

    if (pid < 0) {
        printf ("Hubo un error en la creacion del proceso hijo");
        return 1;
    } 

    if (pid > 0) {
        //sleep(1);   //retira por un momento al programa de la cola de listos
        wait(NULL); //se queda bloqueado el proceso en espera, mientras espera que termine el hijo
        printf ("Proceso padre, mi ID es %d y mi padre es %d\n", getpid(), getppid());

        
    }

    if (pid == 0) {
        printf ("Proceso hijo, mi ID es %d  y el pid de mi padre es %d \n", getpid(), getppid());

        char teclaPresionada;
        printf ("Desea continuar? Presiona s , para continuar \n");
        
        scanf("%c", &teclaPresionada);

        exit(1);
    }

    return 0;
}