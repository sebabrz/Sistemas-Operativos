#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>

#define N 10

char m[N][N];
int contador[N];

//Contar cada vocal de cada fila e ir sumando el resultado a una variable global

void *sumarVocalesPorFila (void *args){

    int fila = (int) args;
    contador[fila] = 0;  // arranca en 0 para esa fila
    
    for (int j=0 ; j<N; j++){
        if (m[fila][j] == 'a' || m[fila][j] == 'e' || m[fila][j] == 'i' || m[fila][j] == 'o' || m[fila][j] == 'u') {
            contador[fila]++;
        }
    }
}

int main() {
    
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

    pthread_t hilo[N];

    for (int i=0; i<N; i++){
        pthread_create(&hilo[i], NULL, &sumarVocalesPorFila, (void *) i); //el cuarto dato es el que se le entrega a la funcion
        
    }
    
    for (int i=0; i<N; i++){
        pthread_join(hilo[i], NULL);
    }

    int cant = 0;
    for (int i = 0; i < N; i++) {
        cant = cant + contador[i];
    }

    printf ("La cantidad de vocales es: %d" , cant);

    return 0;
}