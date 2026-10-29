#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int n;

    printf("Ingrese un entero positivo: \n");
    scanf("%d", &n);

    while (n <= 0) {
        printf("Debe ser un entero positivo. Ingrese de nuevo: \n");
        scanf("%d", &n);
    }

    pid_t pid;
    pid = fork();

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