#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>



/*  Crear M hilos con pthread create().
    Cada hilo imprime su numero de identificacion y un mensaje.
    Esperar la finalizacion de todos los hilos con*/

/*
    Cuando me esta pidiendo el nro de identificacion, no me esta
    pidiendo el ID, sino que imprima     
*/

//rutina

void *identificacion (void *nroRecibido) {

    int miNumero = (int) nroRecibido;
    printf ("Hola soy el Hilo %d \n", miNumero);
    return 0; 
}


int main() {

    int variable;

    printf ("Ingrese la cantidad de Hilos a crear ");
    scanf("%d", &variable);

    pthread_t hilo[variable];   //Se declara un arreglo de Hilos con la variable
                                //que ingreso el usuario

    for (int i=1; i<variable+1; i++){
        pthread_create(&hilo[i], NULL, &identificacion, (void *)(int) i); //el cuarto dato es el que se le entrega a la funcion
    }

    for (int i=1; i<variable+1; i++){
        pthread_join(hilo[i], NULL);        // bloquea al hilo que lo llama, 
                                            // el hilo principal, dentro de main()) hasta que 
                                            // el hilo especificado (hilo[i]) termine su ejecución
    }

    return 0;
}