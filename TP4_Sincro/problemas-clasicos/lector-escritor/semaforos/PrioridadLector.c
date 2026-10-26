#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define READERS 20
#define WRITERS 3

// Requerimientos-Condiciones para este problema:
// * Varios lectores pueden leer en forma simultánea.
// * Escritor escribe en forma excluyente.

// Convención tomada: ningún lector debería esperar a leer a menos que un escritor 
// haya obtenido el permiso de editar el recurso (archivo en este ej) compartido.

sem_t sem_readers, bsem_rw;
pthread_mutex_t read_mutex;

// Definición de colores
#define COLOR_WRITER "\033[1;34m"  // Azul
#define COLOR_READER "\033[1;32m"  // Verde
#define COLOR_RESET "\033[0m"      // Reset

void *writing(void *arg)
{
    while (1){
        int* id = (int*) arg;
        sem_wait(&bsem_rw);   

        printf(COLOR_WRITER "Escritor %i se encuentra escribiendo.\n" COLOR_RESET, *id); 
        
        sem_post(&bsem_rw); 
        printf(COLOR_WRITER "Escritor %i se va.\n" COLOR_RESET, *id);
        sleep(3);
    }
}

void *reading(void *arg) 
{
    int* id = (int*) arg;
    while (1){   
        pthread_mutex_lock(&read_mutex);
            if(sem_trywait(&sem_readers) == 0) {
                sem_post(&sem_readers);
            }
            else {
                sem_wait(&bsem_rw); 
            }
            sem_post(&sem_readers); // Llega un lector
        pthread_mutex_unlock(&read_mutex);

        printf(COLOR_READER "Lector %i se encuentra leyendo.\n" COLOR_RESET, *id);        
        
        pthread_mutex_lock(&read_mutex);
            sem_wait(&sem_readers); // Se va un lector
            printf(COLOR_READER "Lector %i se va.\n" COLOR_RESET, *id);   
            if(sem_trywait(&sem_readers) == 0) {
                sem_post(&sem_readers);
            }
            else{
                sem_post(&bsem_rw);  
            }
        pthread_mutex_unlock(&read_mutex);
        sleep(3);
    }
}

int main()
{
    pthread_t read_threads[READERS], write_threads[WRITERS];

    sem_init(&sem_readers, 0, 0); 
    sem_init(&bsem_rw, 0, 1);
   
    pthread_mutex_init(&read_mutex, NULL);
    
    // Para mejorar los printf (también puede usarse pthread self)
    int idsReaders[READERS];
    for (int i = 0; i < READERS; i++) {
        idsReaders[i] = i;
    }
    
    int idsWriters[WRITERS];
    for (int i = 0; i < WRITERS; i++) {
        idsWriters[i] = i;
    }
   
    for (int i = 0; i < READERS; i++) {
        pthread_create(&read_threads[i], NULL, &reading, &idsReaders[i]);   
    }
    
    for (int i = 0; i < WRITERS; i++) {
        pthread_create(&write_threads[i], NULL, &writing, &idsWriters[i]);   
    }

    for (int i = 0; i < READERS; i++) {
       pthread_join(read_threads[i], NULL);   
    }
    
    for (int i = 0; i < WRITERS; i++) {
        pthread_join(write_threads[i], NULL); 
    }

    sem_destroy(&sem_readers);
    sem_destroy(&bsem_rw);

    pthread_mutex_destroy(&read_mutex);

    return 0;
}
