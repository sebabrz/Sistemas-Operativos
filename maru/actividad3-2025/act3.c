
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>


void crearArbol(int profundidad, int ancho, int nivelActual){
    int p=profundidad;
    int a=ancho;
    int n=nivelActual+1;
    pid_t pid;

   if(p-1>n){
    pid=fork();
    if(pid==0){
        printf("Soy el procesos PID=%d, mi padre es PID=%d, nivel=%d \n", getpid(), getppid(), n);
        crearArbol(p,a,n);
    }
    else if(pid>0){
        for(int i=0;i<a-1;i++){
            pid_t hijo2=fork();
            if(hijo2==0){
                printf("Soy el procesos PID=%d, mi padre es PID=%d, nivel=%d \n", getpid(), getppid(), n);
                exit(0);
            }else if(hijo2>0){
                wait(NULL);
            }
        }
    }
   }
}
int main() {

    int profundidad;
    int ancho;

    printf("Ingrese una profundidad: \n");
    scanf("%d", &profundidad);

    printf("Ingrese un ancho: \n");
    scanf("%d", &ancho);
    printf("Soy el procesos PID=%d, mi padre es PID=%d, nivel=0 \n", getpid(), getppid());
    crearArbol(profundidad, ancho, 0);

    return 0;
}
