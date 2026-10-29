#include "estructura.h"

int main(){

    char nroAlumno[ALUMNOS];
    char *argsAsistente[] = {"./Asistente", NULL};
    char *argsAlumno[] = {"./Alumno", nroAlumno, NULL};
 
	key_t clave = generateKey();
	int id = shmget(clave, 1024, IPC_CREAT | 0666);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	

	//Asociar la memoria a la estructura aCompartir
	struct aCompartir* recursosCompartidos = (struct aCompartir*) shmat(id, 0, 0);
	if(recursosCompartidos <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
        

      
	
	sem_init(&(recursosCompartidos->mtx),1,1);
	sem_init(&(recursosCompartidos->sem_aula),1,1);
	sem_init(&(recursosCompartidos->sem_sillas),1,SILLAS);
	sem_init(&(recursosCompartidos->sem_alumno),1,0);
      
      


	int pidO = fork();
		if(pidO == 0){
			execv("./Asistente", argsAsistente);
			exit(3);
		}

		
	for(int i=0;i<ALUMNOS;i++){
	int pidH = fork();
		if(pidH == 0){
			sprintf(nroAlumno,"%d", i+1);
			execv("./Alumno", argsAlumno);
			exit(3);
		}
	}

     

   for(int i=0; i< ALUMNOS+1;i++){
		wait(NULL);	
	}
	
	
	return 0;
	
	
}
