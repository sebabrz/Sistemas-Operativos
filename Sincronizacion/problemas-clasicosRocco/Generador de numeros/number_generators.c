#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define READ 0
#define WRITE 1

// Función para generar números aleatorios
void generator(int pipe_fd[]) {
    close(pipe_fd[READ]);
    srand(getpid());  // Inicializar la semilla para números aleatorios
    while (1) {
        int num = rand() % 100;  // Genera número aleatorio
        if (write(pipe_fd[WRITE], &num, sizeof(int)) == -1) {
            perror("Error al escribir en el pipe del generador");
            exit(1);
        }
        sleep(1);  // Espera 1 segundo entre generaciones
    }
}

// Función para escribir números en archivo
void writer(const char* filename, int pipe_fd[]) {
    close(pipe_fd[WRITE]);
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }
    
    int num;
    while (1) {
        ssize_t bytes_read = read(pipe_fd[READ], &num, sizeof(int));
        if (bytes_read == -1) {
            perror("Error al leer del pipe del escritor");
            break;
        } else if (bytes_read == 0) {
            // El pipe se ha cerrado
            break;
        }
        fprintf(file, "%d\n", num);
        fflush(file);
    }
    fclose(file);
}

// Función para el proceso de control
void control(int pipe_fd[]) {
    close(pipe_fd[READ]);
    int signal = 1;
    while (1) {
        sleep(5);  // Envía señal de control cada 5 segundos
        if (write(pipe_fd[WRITE], &signal, sizeof(int)) == -1) {
            perror("Error al escribir en el pipe de control");
            exit(1);
        }
    }
}

// Función para el proceso de sincronización
void sync_process(int gen1_fd[], int gen2_fd[], int writer1_fd[], int writer2_fd[], int control_fd[]) {
    close(gen1_fd[WRITE]);
    close(gen2_fd[WRITE]);
    close(writer1_fd[READ]);
    close(writer2_fd[READ]);
    close(control_fd[WRITE]);
    
    int current_writer = 1;  // Inicialmente, se envían los datos a Salida1
    int num, control_signal;
    
    while (1) {
        // Verificar señal de control (no bloqueante)
        if (read(control_fd[READ], &control_signal, sizeof(int)) > 0) {
            current_writer = (current_writer == 1) ? 2 : 1;
            printf("Cambiando al escritor %d\n", current_writer);
        }
        
        // Leer del generador 1 (bloqueante)
        if (read(gen1_fd[READ], &num, sizeof(int)) > 0) {
            if (write(current_writer == 1 ? writer1_fd[WRITE] : writer2_fd[WRITE], &num, sizeof(int)) == -1) {
                perror("Error al escribir en el pipe del escritor");
            }
        }
        
        // Verificar señal de control nuevamente (no bloqueante)
        if (read(control_fd[READ], &control_signal, sizeof(int)) > 0) {
            current_writer = (current_writer == 1) ? 2 : 1;
            printf("Cambiando al escritor %d\n", current_writer);
        }
        
        // Leer del generador 2 (bloqueante)
        if (read(gen2_fd[READ], &num, sizeof(int)) > 0) {
            if (write(current_writer == 1 ? writer1_fd[WRITE] : writer2_fd[WRITE], &num, sizeof(int)) == -1) {
                perror("Error al escribir en el pipe del escritor");
            }
        }
    }
}

int main() {
    int gen1_pipe[2], gen2_pipe[2], writer1_pipe[2], writer2_pipe[2], control_pipe[2];
    
    // Crear pipes
    if (pipe(gen1_pipe) == -1 || pipe(gen2_pipe) == -1 || 
        pipe(writer1_pipe) == -1 || pipe(writer2_pipe) == -1 || 
        pipe(control_pipe) == -1) {
        perror("Error al crear pipes");
        exit(1);
    }
    
    // Configurar solo el pipe de control como no bloqueante
    fcntl(control_pipe[READ], F_SETFL, O_NONBLOCK);
    
    // Crear procesos generadores
    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("Error al crear proceso generador 1");
        exit(1);
    } else if (pid == 0) {
        generator(gen1_pipe);
        exit(0);
    }
    
    if ((pid = fork()) == -1) {
        perror("Error al crear proceso generador 2");
        exit(1);
    } else if (pid == 0) {
        generator(gen2_pipe);
        exit(0);
    }
    
    // Crear procesos escritores
    if ((pid = fork()) == -1) {
        perror("Error al crear proceso escritor 1");
        exit(1);
    } else if (pid == 0) {
        writer("Salida1.txt", writer1_pipe);
        exit(0);
    }
    
    if ((pid = fork()) == -1) {
        perror("Error al crear proceso escritor 2");
        exit(1);
    } else if (pid == 0) {
        writer("Salida2.txt", writer2_pipe);
        exit(0);
    }
    
    // Crear proceso de control
    if ((pid = fork()) == -1) {
        perror("Error al crear proceso de control");
        exit(1);
    } else if (pid == 0) {
        control(control_pipe);
        exit(0);
    }
    
    // Proceso de sincronización
    sync_process(gen1_pipe, gen2_pipe, writer1_pipe, writer2_pipe, control_pipe);
    
    // Esperar a que todos los procesos hijos terminen
    while (wait(NULL) > 0);
    
    return 0;
}