#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10

int main() {
    char m[N][N];
    int i, j;

    // Inicializar la semilla para números aleatorios
    srand(time(NULL));

    // Cargar matriz con letras random entre 'a' y 'z'
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            m[i][j] = 'a' + (rand() % 26); 
        }
    }

    // Mostrar matriz
    printf("Matriz de caracteres:\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf(" %c ", m[i][j]);
        }
        printf("\n");
    }

    return 0;
}
