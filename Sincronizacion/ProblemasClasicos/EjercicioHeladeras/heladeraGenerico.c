#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define HUESPEDES 30
#define MAXIMA 5
#define TIPOS 10

/*
Heladera de hotel con TIPOS tipos de bebida, cada una con stock maximo MAXIMA.
Los huespedes usan la heladera de a uno. Si un huesped se lleva la ultima unidad
de una bebida, despierta al repositor y espera a que termine de reponer.
Mientras tanto nadie mas puede usar la heladera.
Para agregar bebidas alcanza con cambiar TIPOS y agregar el nombre en la lista.
*/

char *nombres[TIPOS] = {
    "Agua", "Cerveza", "Leche", "Gaseosa", "Jugo",
    "Vino", "Soda", "Te helado", "Fernet", "Champagne"
};

pthread_t huespedes[HUESPEDES];
pthread_t repositor;

sem_t stock[TIPOS];     // un semaforo contador por bebida
sem_t avisoAlRepo;      // el repositor duerme aca hasta que lo despierten
sem_t finRepo;          // el huesped espera aca mientras se repone
pthread_mutex_t heladera;

void agarrarBebida(int tipo) {
    pthread_mutex_lock(&heladera);

    // Primer trywait: consumir. Si no hay, me voy sin esperar. sino puedo usar un sem_wait
    if (sem_trywait(&stock[tipo]) != 0) {//aca ya consumi bebida
        printf("No hay %s, me voy\n", nombres[tipo]);
        pthread_mutex_unlock(&heladera);
        return;
    }
    printf("Agarre %s\n", nombres[tipo]);

    // Segundo trywait: ver si era la ultima. Aca ya consumi en el anterior trywait
    if (sem_trywait(&stock[tipo]) == 0) {
        sem_post(&stock[tipo]);     // todavia queda, devuelvo la que saque para mirar
    } else {
        printf("Era la ultima %s, aviso al repositor\n", nombres[tipo]);
        sem_post(&avisoAlRepo);
        sem_wait(&finRepo);
    }

    pthread_mutex_unlock(&heladera);
}

void *huesped(void *arg) {
    while (1) {
        int tipo = rand() % TIPOS;
        agarrarBebida(tipo);
        sleep(1);
    }
    return NULL;
}

void *reponer(void *arg) {
    while (1) {
        sem_wait(&avisoAlRepo);     // duerme hasta que lo despierten
        printf("El repositor se desperto\n");

        for (int t = 0; t < TIPOS; t++) {
            if (sem_trywait(&stock[t]) == 0) {
                sem_post(&stock[t]);        // hay, devuelvo la que saque
            } else {
                for (int i = 0; i < MAXIMA; i++) {
                    sem_post(&stock[t]);
                }
                printf("Repuse %s\n", nombres[t]);
            }
        }

        sem_post(&finRepo);
        printf("El repositor se durmio\n");
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    for (int t = 0; t < TIPOS; t++) {
        sem_init(&stock[t], 0, MAXIMA);
    }
    sem_init(&avisoAlRepo, 0, 0);
    sem_init(&finRepo, 0, 0);
    pthread_mutex_init(&heladera, NULL);

    pthread_create(&repositor, NULL, reponer, NULL);
    for (int i = 0; i < HUESPEDES; i++) {
        pthread_create(&huespedes[i], NULL, huesped, NULL);
    }

    // Los hilos son infinitos: el programa se corta con Ctrl+C
    pthread_join(repositor, NULL);

    return 0;
}