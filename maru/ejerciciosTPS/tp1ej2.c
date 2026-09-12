#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
int main(){
    pid_t pid;
    pid_t waitpid(pid_t pid, int *status, int options);
    int N;
    int i=0,j=0;
     printf("Hola! estamos programando en C para SO n_n \n");
     printf("Ingrese cuantos procesos hijos quiere crear: \n");
     scanf("%d", &N);
    pid_t hijos[N];//arreglo para guardar los pid de los hijos del apdre para q espere hasta q terminen 
     while(i!=N){
        pid= fork();
        if(pid==0){
            
            printf("Hola! soy el proceso hijo con PID=%d, y el de mi padre es %d \n", getpid(), getppid());
            exit(0);
        } else{
            hijos[i]=pid;
        }
        sleep(2);
        i++;
     }
     while (j!=N){
        waitpid(hijos[j], NULL, 0);
        j++;
     }

    return 0;    
}