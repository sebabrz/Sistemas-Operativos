#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h> //agrega el wait()
#include <unistd.h> 
#include <stdlib.h>
#include <math.h>

int main(){
    pid_t pid;
    int n;
    printf("Ingrese un entero positivo: \n");
    scanf("%d", &n);
    while(n<=0){
        printf("Ingrese un entero positivo: \n");
    scanf("%d", &n);
    }
    pid=fork();
    if (pid < 0) {
        printf("Error al crear el proceso hijo\n");
        exit(1);
    } else if (pid == 0) {
        // Proceso hijo: genera y muestra la secuencia de Collatz
        printf("%d", n);
        while (n != 1) {
            if (n % 2 == 0) {
                n = n / 2;
            } else {
                n = 3 * n + 1;
            }
            printf(", %d", n);
        }
        
        printf("\n");
    } else {
        // Proceso padre: espera a que el hijo termine
        wait(NULL);
        printf("El proceso hijo ha finalizado.\n");
    }

    return 0;
}