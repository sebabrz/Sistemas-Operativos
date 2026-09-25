#define READ 0
#define WRITE 1
#define BUFF 100 // MODIFICAR: tamaño del mensaje si hace falta

int main() {
    // NO TOCAR: creación de los pipes
    // Si el ejercicio es unidireccional, borrar pipeFD2 y todo lo que lo usa.
    int pipeFD1[2]; // padre -> hijo
    int pipeFD2[2]; // hijo -> padre
    char mensaje[BUFF];

    pid_t pid;

    if (pipe(pipeFD1) == -1 || pipe(pipeFD2) == -1 ) {
        perror("Error al crear pipes");
        exit(1);
    }

    pid = fork();

    if (pid > 0) {
        //---- Proceso padre ----
        // NO TOCAR: cierre de los extremos que no usa el padre
        close(pipeFD1[READ]);
        close(pipeFD2[WRITE]);

        // ejemplo escribir: write(pipeFD1[WRITE], mensaje, strlen(mensaje) + 1);
        // ejemplo leer:     read(pipeFD2[READ], mensaje, sizeof(mensaje));
        // Si el mensaje no es texto sino un struct (enteros, tipo+contenido, etc.),
        // reemplazar char mensaje[BUFF] por el struct y usar sizeof(struct) en vez de strlen()+1.

        close(pipeFD1[WRITE]);
        close(pipeFD2[READ]);
        wait(NULL);

    } else if (pid == 0) {
        //---- Proceso hijo ----
        // NO TOCAR: cierre de los extremos que no usa el hijo
        close(pipeFD1[WRITE]);
        close(pipeFD2[READ]);

        // TODO: implementar lo que pide el ejercicio
        // ejemplo leer:     read(pipeFD1[READ], mensaje, sizeof(mensaje));
        // ejemplo escribir: write(pipeFD2[WRITE], mensaje, strlen(mensaje) + 1);

        close(pipeFD1[READ]);
        close(pipeFD2[WRITE]);
        exit(0);
    }

    return 0;
}