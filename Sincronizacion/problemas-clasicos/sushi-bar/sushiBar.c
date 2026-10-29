/* This problem was inspired by a problem proposed by Kenneth Reek [9]. Imagine
a sushi bar with 5 seats. If you arrive while there is an empty seat, you can take
a seat immediately. But if you arrive when all 5 seats are full, that means that
all of them are dining together, and you will have to wait for the entire party
to leave before you sit down.*/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define NUM_SEATS 5

// Semáforos
sem_t asientos; // Controla los asientos disponibles
sem_t clientesEnBar; // Número de clientes actuales
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* customer(void* id) {
    while(1){
        int customer_id = (int)id;
        pthread_mutex_lock(&mutex);
        // Intentar tomar asiento
        if(sem_trywait(&asientos)==0){
            printf(GREEN "Cliente %d se sienta.\n" RESET , customer_id);  
            sem_post(&clientesEnBar);
            pthread_mutex_unlock(&mutex);  
                   
            printf(MAGENTA "Cliente %d comiendo sushi.\n" RESET , customer_id);  
            sleep(3); // Comen sushi
            
            pthread_mutex_lock(&mutex);
            sem_wait(&clientesEnBar);
            printf(YELLOW "Cliente %d termina y se va.\n" RESET, customer_id);
            // Si todos los clientes actuales han terminado, permitir a los nuevos clientes tomar asiento
            if (sem_trywait(&clientesEnBar) != 0) {
                printf(GREEN "Todos los clientes se fueron. Nuevos clientes pueden entrar.\n" RESET);
                for (int i = 0; i < NUM_SEATS; i++) {
                    sem_post(&asientos); // Liberar los asientos para los nuevos clientes
                }
            }
            else{
                sem_post(&clientesEnBar);
                
            }
            pthread_mutex_unlock(&mutex);
            pthread_exit(0); // me voy del bar
        }
        else{        
            pthread_mutex_unlock(&mutex);
           
        }
        
    }
    
}
int main() {
    pthread_t customers[15];

    // Inicialización de semáforos
    sem_init(&asientos, 0, NUM_SEATS); // Inicialmente hay 5 asientos disponibles
    sem_init(&clientesEnBar, 0, 0); 

    for (int i = 0; i < 15; i++) {
        pthread_create(&customers[i], NULL, customer, (void *)i);
        sleep(1);
      
    }

    // Esperar a que todos los clientes terminen
    for (int i = 0; i < 15; i++) {
        pthread_join(customers[i], NULL);
    }

    // Destruir los semáforos
    sem_destroy(&asientos);
    sem_destroy(&clientesEnBar);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
