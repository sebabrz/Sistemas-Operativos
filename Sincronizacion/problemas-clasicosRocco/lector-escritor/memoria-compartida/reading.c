#include "estructura.h"

int main(){
	key_t clave = generateKey();
	int id = shmget(clave, 1024, 0);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	
	struct aCompartir* memCompartida = (struct aCompartir*) shmat(id,0,0);
	if(memCompartida <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
		
    while (1){   
        sem_wait(&(memCompartida->mutex)); // tomo el mutex para realizar una operacion que debe estar protegida
            if(sem_trywait(&(memCompartida->sem_readers)) == 0) {
                sem_post(&(memCompartida->sem_readers));
            }
            else {
                sem_wait(&(memCompartida->bsem_rw)); 
            }
        sem_post(&(memCompartida->sem_readers)); // Llega un lector
        sem_post(&(memCompartida->mutex));

        printf(COLOR_READER "Lector se encuentra leyendo.\n" COLOR_RESET);        
        
        sem_wait(&(memCompartida->mutex)); // tomo el mutex para realizar una operacion que debe estar protegida
            sem_wait(&(memCompartida->sem_readers)); // Se va un lector
            printf(COLOR_READER "Lector se va.\n" COLOR_RESET);   
            if(sem_trywait(&(memCompartida->sem_readers)) == 0) {
                sem_post(&(memCompartida->sem_readers));
            }
            else{
                sem_post(&(memCompartida->bsem_rw));  
            }
        sem_post(&(memCompartida->mutex));
        sleep(3);
	}
    return 0;
}
