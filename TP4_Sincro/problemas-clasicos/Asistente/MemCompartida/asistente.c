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
	
	
	for (int i = 0; i < ITER; i++) {
        // barbero espera clientes
        sem_wait(&(memCompartida->mtx));
        if (sem_trywait(&(memCompartida->sem_alumno)) != 0) {
            sem_post(&(memCompartida->mtx));
	    printf(PURPLE"Asistente se duerme"RESET);
            sem_wait(&(memCompartida->sem_alumno)) ;
            
            printf(PURPLE"Asistente se despierta \n"RESET);
            printf(PURPLE"Asistente ayudando a alumno\n"RESET);
	    sleep(1);

            sem_post(&(memCompartida->sem_aula));
        
	}else{
            sem_post(&(memCompartida->mtx));
            printf(PURPLE"Asistente ayudando a alumno\n"RESET);
	    sleep(1);
            sem_post(&(memCompartida->sem_aula));
	}
    
    }	


    return 0;		
}	
