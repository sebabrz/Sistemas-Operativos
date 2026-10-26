    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <fcntl.h>


    #define PIPE_READ 0
    #define PIPE_WRITE 1

    #define YELLOW "\033[1;33m"
    #define PURPLE "\033[1;35m"
    #define RESET "\033[0m"


    #define VIP 1
    #define NORMAL 2
    #define USERS_C 3
    #define USERS_V 5
    #define ITER 1

    typedef struct {
        int type;
    } Mensaje;

    int pipeImpresora1[2], pipeImpresora2[2], pipeRequerir[2], pipeLiberar[2], pipePrio[2], pipeEspera[2];

    // Función para configurar un pipe como no bloqueante
    void set_non_blocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    int requerir(){
        int printer = 0;
        Mensaje m;

        int bytes_leidos = read(pipeImpresora1[PIPE_READ], &m, sizeof(Mensaje));
        if (!(bytes_leidos > 0)) { //si la impresora 1 NO estaba disponible (no se pudo leer de su pipe)
            read(pipeImpresora2[PIPE_READ], &m, sizeof(Mensaje));
        }

        return m.type;
    }

    void liberar(int id){
        Mensaje m;
        m.type = id;
            switch (id) {
        case 1:
            write(pipeImpresora1[PIPE_WRITE],&m, sizeof(Mensaje));
            break;
        case 2:
            write(pipeImpresora2[PIPE_WRITE],&m, sizeof(Mensaje));
            break;
            }
    }

    void *user_v() {
        const char *color = PURPLE;
        Mensaje m;
        m.type = 0;

        close(pipeRequerir[PIPE_READ]);
        close(pipeLiberar[PIPE_READ]);
        close(pipePrio[PIPE_READ]);
        close(pipeEspera[PIPE_READ]);

        for (int i = 0; i < ITER; i++) {
            //sleep(1);

            // hay un usuario con prioridad esperando
            printf("%susuarioVIP quiere usar impresora%s\n", color, RESET);
            write(pipePrio[PIPE_WRITE],&m, sizeof(Mensaje));

            // usuario espera por impresora
            int printer = requerir();
            printf("%susuario V reserva impresora[%d] %s\n", color, printer, RESET);

            sleep(1);

            // usuario libera impresora
            liberar(printer);
            printf("%susuario V libera impresora[%d] %s\n", color, printer, RESET);
            //sleep(1);
            write(pipeEspera[PIPE_WRITE],&m, sizeof(Mensaje));
        }

        close(pipeImpresora1[PIPE_WRITE]);
        close(pipeImpresora2[PIPE_WRITE]);
        close(pipeImpresora1[PIPE_READ]);
        close(pipeImpresora2[PIPE_READ]);
        close(pipePrio[PIPE_WRITE]);
        close(pipeEspera[PIPE_WRITE]);

        exit(EXIT_SUCCESS);
    }

    void *user_c() {
        const char *color = YELLOW;
        Mensaje m;
        m.type = 0;

        close(pipeRequerir[PIPE_READ]);
        close(pipeLiberar[PIPE_READ]);
        close(pipePrio[PIPE_WRITE]);
        close(pipeEspera[PIPE_WRITE]);

        for (int i = 0; i < ITER; i++) {
            //sleep(1);

            // revisa si hay un usuario con prioridad esperando
            if (read(pipePrio[PIPE_READ], &m, sizeof(Mensaje)) > 0) {
                printf("%susuario comun espera a usuarioVIP%s\n", color, RESET);
                read(pipeEspera[PIPE_READ], &m, sizeof(Mensaje));
            }

            // usuario espera por impresora
            int printer = requerir();
            printf("%susuario C reserva impresora[%d] %s\n", color, printer, RESET);

            sleep(1);

            // usuario libera impresora
            liberar(printer);
            printf("%susuario C libera impresora[%d] %s\n", color, printer, RESET);
        }

        close(pipeImpresora1[PIPE_WRITE]);
        close(pipeImpresora2[PIPE_WRITE]);
        close(pipeImpresora1[PIPE_READ]);
        close(pipeImpresora2[PIPE_READ]);
        close(pipePrio[PIPE_READ]);
        close(pipeEspera[PIPE_READ]);

        exit(EXIT_SUCCESS);
    }

    int main(){
        if (pipe(pipeImpresora1) == -1) {
            perror("Error creando pipe Impresora 1");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipeImpresora2) == -1) {
            perror("Error creando pipe Impresora 2");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipeRequerir) == -1) {
            perror("Error creando pipe Requerir");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipeLiberar) == -1) {
            perror("Error creando pipe Liberar");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipePrio) == -1) {
            perror("Error creando pipe Prio");
            exit(EXIT_FAILURE);
        }

            if (pipe(pipeEspera) == -1) {
            perror("Error creando pipe Espera");
            exit(EXIT_FAILURE);
        }

        set_non_blocking(pipeImpresora1[PIPE_READ]);
        set_non_blocking(pipePrio[PIPE_READ]);

        // Initialize printer pipes
        Mensaje m1;
        m1.type = 1;
        Mensaje m2;
        m2.type = 2;

        if (write(pipeImpresora1[PIPE_WRITE], &m1, sizeof(Mensaje)) == -1) {
            perror("Error initializing Impresora 1");
            exit(EXIT_FAILURE);
        }

        if (write(pipeImpresora2[PIPE_WRITE], &m2, sizeof(Mensaje)) == -1) {
            perror("Error initializing Impresora 2");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < USERS_V; i++) {

            pid_t pid = fork();
            if (pid == -1) {
                perror("Error al crear proceso USER V");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {  // Proceso hijo
                user_v();
                exit(EXIT_FAILURE);

            }
            //sleep(1);
        }

        for (int i = 0; i < USERS_C; i++) {

            pid_t pid = fork();
            if (pid == -1) {
                perror("Error al crear proceso USER C");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {  // Proceso hijo
                user_c();
                exit(EXIT_FAILURE);
            }
            //sleep(1);
        }

        close(pipeImpresora1[PIPE_READ]);
        close(pipeImpresora1[PIPE_WRITE]);
        close(pipeImpresora2[PIPE_READ]);
        close(pipeImpresora2[PIPE_WRITE]);
        close(pipeRequerir[PIPE_READ]);
        close(pipeRequerir[PIPE_WRITE]);
        close(pipeLiberar[PIPE_READ]);
        close(pipeLiberar[PIPE_WRITE]);
        close(pipePrio[PIPE_READ]);
        close(pipePrio[PIPE_WRITE]);
        close(pipeEspera[PIPE_READ]);
        close(pipeEspera[PIPE_WRITE]);

        for (int i = 0;i<USERS_V+USERS_C;i++){
            wait(NULL);
        }

        return 0;
    }
