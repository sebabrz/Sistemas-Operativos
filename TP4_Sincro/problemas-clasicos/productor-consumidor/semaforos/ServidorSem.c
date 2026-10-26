
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define REPETITIONS 100  // Número de repeticiones 
#define N 10  
#define CONEXIONES 10  

// Declaración de semáforos
sem_t vacio;

void* conexion(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
        sem_wait(&vacio);
        printf("me conecte!!\n"); 
		sleep(1);               
        sem_post(&vacio);
        printf("me desconecte!!\n"); 
    }
    return NULL;
}

int main() {
    pthread_t thread_c;

    // Inicialización de los semáforos
    sem_init(&vacio, 0, N);  

	for (int i = 0; i < CONEXIONES; i++){
		pthread_create(&thread_c, NULL, conexion, NULL);
	}
	
	for (int i = 0; i < CONEXIONES; i++){
		pthread_join(thread_c, NULL);
	}
   
    // Destrucción de los semáforos
    sem_destroy(&vacio);

    printf("\n");  // Salto de línea final
    return 0;
}
