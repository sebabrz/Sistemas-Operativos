#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <stdint.h>     // enteros de tamaño fijo
#include <string.h>     // manejo de strings
#include <semaphore.h>
#include <time.h>

sem_t vacio;
sem_t lleno;
sem_t abejasRestantes;

void *rutina_abeja(){
    while(1){
        printf("Una abeja se fija si hay porciones para poner\n");
        sem_wait(&vacio);//si hay porciones para poner, pone una, sino es porque el oso esta comiendo
        printf("Una abeja pone una porcion\n");
        if(sem_trywait(&abejasRestantes)!=0){//si nadie mas tiene que poner es porque soy la ultima, asique
            printf("La ultima abeja avisa al oso\n");
            sem_post(&lleno);//aviso al oso
        }
        sleep(2);
    }
}

void *rutina_oso(void *arg){
    int M=*(int *)arg;
    while(1){
        printf("El oso duerme \n");
        sem_wait(&lleno);//esta dormido inicialmente y se despierta cuando las abejas le dicen que esta lleno con el signal(lleno)
        printf("El oso se despierta\n");

        //el oso libera todas las vacantes antes que las M porciones, para que una abeja no llene con la porcion que recien libero
        //y se confunda con que es la ultima
        for(int i=0;i<M-1;i++){
            sem_post(&abejasRestantes);
        }
        //ahora si va liberando las porciones-->duda: aca no puede pasar que el mientra el oso liebra las abejas van llenando?
        for(int i=0;i<M;i++){
            sem_post(&vacio);
        }
        printf("El oso se duerme y ya vacio el tarro\n");
        sleep(5);
    }
}


int main() {
    int M=0;
    int cantidadAbejas=0;
    while(M<=0){
        printf("Indique cantidad de porciones\n");
        scanf("%d", &M);
    }
    while(cantidadAbejas<=0){
        printf("Indique cantidad de abejas\n");
        scanf("%d", &cantidadAbejas);
    }
    sem_init(&vacio,0,M);//cantidad de porciones que faltan poner en el tarro
    sem_init(&lleno,0,0);//binario, si es 0=no esta lleno, si es 1=esta lleno y lo consume entero el oso, sino debe llegar hasta M y consumir M porciones
    sem_init(&abejasRestantes,0,M-1); //para saber si es la ultima porcion que se puso, la abeja M quiere descontar pero como M-1 ya pusieron debe avisarle al oso
    srand(time(NULL));
    pthread_t oso;
    pthread_t abejas[cantidadAbejas];
  
    pthread_create(&oso, NULL, &rutina_oso, &M);
    for(int i=0; i<cantidadAbejas;i++){
        pthread_create(&abejas[i], NULL, &rutina_abeja, NULL);
    }

    pthread_join(oso,NULL);
    for(int i=0; i<cantidadAbejas;i++){
        pthread_join(abejas[i], NULL);
    }
    
    sem_close(&vacio);
	sem_close(&lleno);
	sem_close(&abejasRestantes);
	return 0;
}