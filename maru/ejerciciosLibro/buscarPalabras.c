#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PALABRAS 3

/*fscanf(archivo, "%99s", palabra_leida)

fscanf lee del archivo tratando de matchear el formato que le diste. %s le dice "leeme una palabra"
es decir, todo lo que encuentre hasta el próximo espacio, tab o salto de línea, sin incluirlos. 
El 99 antes de la s es un límite de seguridad: "leé como máximo 99 caracteres y frená ahí", 
para que nunca te desborde el arreglo palabra_leida (que en el código de antes lo declaramos como 
char palabra_leida[100] — dejás 1 lugar para el \0 que fscanf te pone solo al final).
fscanf no te devuelve la palabra por el return (esa te la deja en palabra_leida, por referencia).
Lo que devuelve es cuántos ítems logró leer y matchear. Como el formato pide un solo %s, en un caso normal 
devuelve 1. Cuando ya no hay más nada para leer (llegaste al final del archivo), devuelve EOF (que es una 
constante, típicamente -1).
Por eso el while funciona así: "mientras fscanf haya logrado leer algo (no haya devuelto EOF), seguí". 
En criollo: es el mismo patrón que tenías con fgetc en tu apunte (while ((c = fgetc(archivo)) != EOF)), 
pero acá en vez de traerte un carácter a la vez, te trae una palabra a la vez.
*/

int main(int argc, char *argv[]) {
    char *palabras[NUM_PALABRAS] = {"hola", "como", "estan"};
    int pipefd[NUM_PALABRAS][2];
    pid_t pid;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s archivo.txt\n", argv[0]);
        exit(1);
    }

    for (int i = 0; i < NUM_PALABRAS; i++) {
        if (pipe(pipefd[i]) < 0) {
            perror("pipe");
            exit(1);
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // HIJO i: cuenta SU palabra y termina
            close(pipefd[i][0]); // no lee

            FILE *archivo = fopen(argv[1], "r");
            if (archivo == NULL) {
                perror("fopen");
                exit(1);
            }

            int contador = 0;
            char palabra_leida[100];

            while (fscanf(archivo, "%99s", palabra_leida) != EOF) {
                if (strcmp(palabra_leida, palabras[i]) == 0) {
                    contador++;
                }
            }

            fclose(archivo);
            write(pipefd[i][1], &contador, sizeof(int));
            close(pipefd[i][1]);
            exit(0);
        } else {
            // PADRE: cierra escritura de este pipe y sigue creando el próximo hijo
            close(pipefd[i][1]);
        }
    }

    for (int i = 0; i < NUM_PALABRAS; i++) {
        int resultado;
        read(pipefd[i][0], &resultado, sizeof(int));
        close(pipefd[i][0]);
        printf("\"%s\" aparece %d veces\n", palabras[i], resultado);
    }

    for (int i = 0; i < NUM_PALABRAS; i++) {
        wait(NULL);
    }

    return 0;
}