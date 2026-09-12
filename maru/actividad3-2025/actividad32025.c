#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
void crearArbol(int profundidad, int ancho, int nivelActual);
int main() {

    int profundidad;
    int ancho;

    printf("Ingrese una profundidad: \n");
    scanf("%d", &profundidad);

    printf("Ingrese un ancho: \n");
    scanf("%d", &ancho);
    crearArbol(profundidad, ancho, 0);

    return 0;
}

void crearArbol(int profundidad, int ancho, int nivelActual){
pid_t pid=getpid();
pid_t hijo;
printf("Soy el procesos PID %d y el PID del padre es %d\n [nivelActual]", getpid(), getppid());

  if (nivelActual < profundidad - 1) {
    pid= fork();
    if(pid>0)
        for (int i = 1; i < ancho; i++) {
            hijo= fork();
            if(hijo==0){
            printf("Soy el procesos PID %d y el PID del padre es %d\n [nivelActual]", getpid(), getppid());
                exit(0);
        }
        }
    }else if (pid==0){
        crearArbol(profundidad,ancho,nivelActual+1);

    }


for(int i=0;i<ancho;i++){
wait(NULL);    
}
}