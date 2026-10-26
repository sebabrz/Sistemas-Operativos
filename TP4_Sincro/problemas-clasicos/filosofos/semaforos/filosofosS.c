#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define ITERATIONS 100

#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

sem_t sem_stick[NUM_PHILOSOPHERS];

// semaforo para prevencion de deadlock
// siempre que un palillo quede libre, no se genera deadlock

pthread_mutex_t  mutex;

void *philosophers(void *args) {
    int id = (int) args;

    int left_stick = id % NUM_PHILOSOPHERS;
    int right_stick = (id + 4) % NUM_PHILOSOPHERS;

    while (1) {
        // filosofo pensando
        printf(BLUE "Filosofo %d pensando... \n" RESET, id);

        
        pthread_mutex_lock(&mutex);
        
        sem_wait(&sem_stick[left_stick]);
        if(sem_trywait(&sem_stick[right_stick]) == 0){
             printf(RED "Filosofo %d come (s:%d s:%d)\n"RESET, id, left_stick, right_stick);
             pthread_mutex_unlock(&mutex);
             sleep(1);
             sem_post(&sem_stick[left_stick]);
             sem_post(&sem_stick[right_stick]);
         }else{
                 printf( "Filosofo %d suelta palito porque no pudo agarrar los dos\n", id);
                sem_post(&sem_stick[left_stick]);
              pthread_mutex_unlock(&mutex);
              
              
         }
         
         sleep(2);
                
    }
    pthread_exit(NULL);
}

void main() {
    pthread_t philosophers_t[NUM_PHILOSOPHERS];
    pthread_mutex_init(&mutex, NULL);

   

    // ---- palillos y filosofos ----

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&sem_stick[i], 0, 1);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        int id = i + 1;
        pthread_create(&philosophers_t[i], NULL, philosophers, (void *) id);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers_t[i], NULL);
    }

    // ---- destruye los palillos ----

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&sem_stick[i]);
    }
   pthread_mutex_destroy(&mutex);
}
