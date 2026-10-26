#include "estructura.h"

int main(int argc, char* argv[]){
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
	
	
	
    
    int idA = atoi(argv[1]);
    for (int i = 0; i < ITER; i++) {
        sleep(2);
		printf(YELLOW"Alumno %d llega \n"RESET, idA);
        
        sem_wait(&(memCompartida->mtx));
        if (sem_trywait(&(memCompartida->sem_sillas)) == 0) {
            // si hay sillas, se sienta
            sem_post(&(memCompartida->sem_alumno)) ;
            sem_post(&(memCompartida->mtx));

            printf(YELLOW"alumno %d espera assitente... \n"RESET, idA);
             sem_wait(&(memCompartida->sem_aula)); // espera assistente

            // assitente ayudando alumno
            
            sem_post(&(memCompartida->sem_sillas));
        } else {
            // si no hay sillas, se va
            sem_post(&(memCompartida->mtx));
             printf(YELLOW"alumno %d se va \n"RESET, idA);
			sleep(1);
        }
        
         sleep(4);
    }


    return 0;		
}	
