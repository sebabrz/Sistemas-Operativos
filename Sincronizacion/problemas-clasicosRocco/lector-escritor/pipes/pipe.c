    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <fcntl.h>


    #define PIPE_READ 0
    #define PIPE_WRITE 1
    
#define READERS 20
#define WRITERS 3

// Definición de colores
#define COLOR_WRITER "\033[1;34m"  // Azul
#define COLOR_READER "\033[1;32m"  // Verde
#define COLOR_RESET "\033[0m"      // Reset

// Requerimientos-Condiciones para este problema:
// * Varios lectores pueden leer en forma simultánea.
// * Escritor escribe en forma excluyente.

// Convención tomada: ningún lector debería esperar a leer a menos que un escritor 
// haya obtenido el permiso de editar el recurso (archivo en este ej) compartido.

    char m = 'x';

    int mutex[2], bsem_rw[2], sem_readers[2];

    // Función para configurar un pipe como no bloqueante
    void set_non_blocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }


void *writing()
{
	close(sem_readers[PIPE_WRITE]);
	close(sem_readers[PIPE_READ]);
	close(mutex[PIPE_WRITE]);
	close(mutex[PIPE_READ]);

    
    while (1){
        read(bsem_rw[PIPE_READ], &m, sizeof(char));

        printf(COLOR_WRITER "Escritor se encuentra escribiendo.\n" COLOR_RESET); 
        
        write(bsem_rw[PIPE_WRITE], &m, sizeof(char));
        printf(COLOR_WRITER "Escritor se va.\n" COLOR_RESET);
        sleep(3);
    }
}

void *reading() 
{
        
    while (1){   
        read(mutex[PIPE_READ], &m, sizeof(char));
            if(read(sem_readers[PIPE_READ], &m, sizeof(char)) > 0) {
                write(sem_readers[PIPE_WRITE], &m, sizeof(char));
            }
            else {
                read(bsem_rw[PIPE_READ], &m, sizeof(char));
            }
            write(sem_readers[PIPE_WRITE], &m, sizeof(char));
        write(mutex[PIPE_WRITE], &m, sizeof(char));

        printf(COLOR_READER "Lector se encuentra leyendo.\n" COLOR_RESET);        
        
        read(mutex[PIPE_READ], &m, sizeof(char));
            read(sem_readers[PIPE_READ], &m, sizeof(char)); // Se va un lector
            printf(COLOR_READER "Lector se va.\n" COLOR_RESET);   
            if(read(sem_readers[PIPE_READ], &m, sizeof(char)) > 0) {
                write(sem_readers[PIPE_WRITE], &m, sizeof(char));
            }
            else{
                write(bsem_rw[PIPE_WRITE], &m, sizeof(char));
            }
        write(mutex[PIPE_WRITE], &m, sizeof(char));
        sleep(3);
    }
}

int main()
{
	pid_t pidA;
    pid_t pidB;


    pipe(mutex);
    pipe(bsem_rw);
    pipe(sem_readers);
    
    set_non_blocking(sem_readers[PIPE_READ]);

    

    write(mutex[PIPE_WRITE], &m, sizeof(char));
    write(bsem_rw[PIPE_WRITE], &m, sizeof(char));

    
    for (int i = 0; i < READERS; i++) {
        pidA = fork();
        if(pidA == -1){
        printf("Error al crear pidA\n");
        }
        else if(pidA == 0){
            reading();}
    }
    
    for (int i = 0; i < WRITERS; i++) {
        pidB = fork();
        if(pidB == -1){
        printf("Error al crear pidA\n");
        }
        else if(pidB == 0){
            writing();}
    }
    
    close(sem_readers[PIPE_WRITE]);
	close(sem_readers[PIPE_READ]);
	close(mutex[PIPE_WRITE]);
	close(mutex[PIPE_READ]);
	close(bsem_rw[PIPE_WRITE]);
	close(bsem_rw[PIPE_READ]);
    
    for(int i = 0; i < READERS + WRITERS; i++){
        wait(NULL);
    }
   
    return 0;
}
