/*
 * Problema de los fumadores
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

//Declaracion de semaforos

sem_t sem_agente,sem_tabaco,sem_fosforos,sem_papel;
pthread_mutex_t mutex_mesa;

#define RESET   "\033[0m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

//Procesos

void* pAgente() {
    while (1) {
        sem_wait(&sem_agente); // Espera que lo llamen
        int number = rand() % 3;
        switch (number) {
            case 0:
                printf(BLUE "\nEl agente deja fósforos y papel.\n" RESET);
                usleep(1000000);
                sem_post(&sem_fosforos); // Deja fósforos y papel
                sem_post(&sem_papel);
                break;
            case 1:
                printf(BLUE "\nEl agente deja fósforos y tabaco.\n" RESET);
                usleep(1000000);
                sem_post(&sem_fosforos); // Deja fósforo y tabaco
                sem_post(&sem_tabaco);
                break;
            case 2:
                printf(BLUE "\nEl agente deja tabaco y papel.\n" RESET);
                usleep(1000000);
                sem_post(&sem_tabaco); // Deja tabaco y papel
                sem_post(&sem_papel);
                break;
        }
    }
}

void* pFumadorConTabaco() {
    while (1) {
		pthread_mutex_lock(&mutex_mesa);
        if (sem_trywait(&sem_fosforos) == 0){ // Agarra el fósforo de la mesa
			printf(GREEN "\nEl fumador con tabaco agarra el fósforo de la mesa." RESET);
			usleep(1000000);
			if (sem_trywait(&sem_papel) == 0) { // Agarra el papel de la mesa
				printf(GREEN "\nEl fumador con tabaco agarra el papel de la mesa." RESET);
				usleep(1000000);
				// Arma el cigarro y fuma
				sem_post(&sem_agente); // Le avisa al agente que fumó
				printf(GREEN "\nEl fumador con tabaco arma su cigarro y le avisa al agente.\n" RESET);
				usleep(1000000);
			} else {
				printf(GREEN "\nEl fumador con tabaco deja el fósforo en la mesa porque no había papel.\n" RESET);
				usleep(1000000);
				sem_post(&sem_fosforos); // Si no había papel, deja el fósforo de nuevo en la mesa.
        }
	}

        pthread_mutex_unlock(&mutex_mesa);
        usleep(1000000);
    }
}

void* pFumadorConPapel() {
    while (1) {
		pthread_mutex_lock(&mutex_mesa);
        if (sem_trywait(&sem_tabaco) == 0){// Agarra el tabaco de la mesa
			printf(YELLOW "\nEl fumador con papel agarra el tabaco de la mesa." RESET);
			usleep(1000000);
			if (sem_trywait(&sem_fosforos) == 0) { // Agarra el fósforo de la mesa
				printf(YELLOW "\nEl fumador con papel agarra el fosforo de la mesa." RESET);
				usleep(1000000);
				// Arma el cigarro y fuma
				sem_post(&sem_agente); // Le avisa al agente que fumó
				printf(YELLOW "\nEl fumador con papel arma su cigarro y le avisa al agente.\n" RESET);
				usleep(1000000);
			} else {
				printf(YELLOW "\nEl fumador con papel deja el tabaco y fosforos en la mesa porque no había fosforos.\n" RESET);
				usleep(1000000);
				sem_post(&sem_tabaco); // Si no había fósforo, deja el tabaco de nuevo en la mesa.
				}
		}
        pthread_mutex_unlock(&mutex_mesa);
        usleep(1000000);
    }
}

void* pFumadorConFosforos() {
    while (1) {
		pthread_mutex_lock(&mutex_mesa);
        if (sem_trywait(&sem_papel) == 0) { // Agarra el papel de la mesa
			printf(RED "\nEl fumador con fósforos agarra el papel de la mesa." RESET);
			usleep(1000000);
			if (sem_trywait(&sem_tabaco) == 0) { // Agarra el tabaco de la mesa
				printf(RED "\nEl fumador con fósforos agarra el tabaco de la mesa." RESET);
				usleep(1000000);
				// Arma el cigarro y fuma
				sem_post(&sem_agente); // Le avisa al agente que fumó
				printf(RED "\nEl fumador con fósforos arma su cigarro y le avisa al agente.\n" RESET);
				usleep(1000000);
			} else {
				printf(RED "\nEl fumador con fósforos deja el tabaco en la mesa porque no había papel.\n" RESET);
				usleep(1000000);
				sem_post(&sem_papel); // Si no había papel, deja el tabaco de nuevo en la mesa.
				}
		}
        pthread_mutex_unlock(&mutex_mesa);
        usleep(1000000);
    }
}

int main(int argc, char **argv)
{
	//Inicializacion de semaforos
	sem_init(&sem_agente, 0, 1);
	sem_init(&sem_tabaco, 0, 0);
	sem_init(&sem_papel, 0, 0);
	sem_init(&sem_fosforos, 0, 0);
	pthread_mutex_init(&mutex_mesa, NULL);
	
	//Inicializacion de hilos
	pthread_t thread[4];
	
	pthread_create(&thread[0], NULL, &pAgente, NULL);
	pthread_create(&thread[1], NULL, &pFumadorConFosforos, NULL);
	pthread_create(&thread[2], NULL, &pFumadorConPapel, NULL);
	pthread_create(&thread[3], NULL, &pFumadorConTabaco, NULL);
	
	
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
	pthread_join(thread[2], NULL);
	pthread_join(thread[3], NULL);
	
	sem_close(&sem_tabaco);
	sem_close(&sem_papel);
	sem_close(&sem_fosforos);
	sem_close(&sem_agente);
	
	sem_destroy(&sem_tabaco);
	sem_destroy(&sem_papel);
	sem_destroy(&sem_fosforos);
	sem_destroy(&sem_agente);
	pthread_mutex_destroy(&mutex_mesa);
	return 0;
}

