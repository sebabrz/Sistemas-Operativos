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
	
	while(1){
		// Espera a que el tarro esté lleno
        sem_wait(&(memCompartida->despertar_oso));

        printf("El oso se despierta\n");

        for (int i = 0;i< ABEJAS;i++){
                sem_wait(&(memCompartida->turno));
        }
        for (int i = 0; i < PORCIONES; i++) {
            sem_wait(&(memCompartida->lleno));
            printf("Oso COME miel %d\n", i);

            // Devuelve un espacio vacío al tarro
            sem_post(&(memCompartida->vacio));
        }
        printf("El oso se comio toda la miel, ahora se va a dormir\n");

        for (int i = 0;i< ABEJAS;i++){
                sem_post(&(memCompartida->turno)); // devuelvo los turnos
        }
        sleep(3);
    }

    return 0;		
}	
