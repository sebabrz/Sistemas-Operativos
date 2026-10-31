#include "def.h"

int main()
{
    char id_str[10]; // Buffer to hold the string representation of id
    char *argsAlumno[] = {"./alumno", id_str, NULL};
    char *argsAsistente[] = {"./asistente", NULL};

    srand(time(NULL));
    // Clave �nica para la cola de pedidos
    key_t key_q = generateKey();

    // Crear cola de mensajes
    int cola = msgget(key_q, 0666 | IPC_CREAT);

    if (cola == -1)
    {
        perror("Error al crear la cola de mensajes");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // Inicializa la cola con sillas
    for (int i = 0; i < SILLAS; i++)
    {
        struct mensaje_t msg;
        msg.type = semSilla;
        msgsnd(cola, &msg, msgSize, 0);
    }

    struct mensaje_t msg;

    // A�ado el mutex a la cola de mensajes

    msg.type = MUTEX;
    msgsnd(cola, &msg, msgSize, 0);

    // A�ado los el aula a la cola de mensajes
    msg.type = semAula;
    msgsnd(cola, &msg, msgSize, 0);

    // Crear el proceso asistente
    int pid_asist = fork();
    if (pid_asist == -1)
    {
        perror("Error al crear proceso pid_asist");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    if (pid_asist == 0)
    {
        execv("./asistente", argsAsistente);
        exit(0);
    }

    // Generar n Alumnos

    for (int i = 0; i < ALUMNOS; i++)
    {
        int pidC = fork();

        if (pidC == 0)
        {
            sprintf(id_str, "%d", i + 1); // Convert id to string
            execv("./alumno", argsAlumno);
            printf("error");
            exit(4);
        }
    }

    for (int i = 0; i < ALUMNOS + 1; i++)
    {
        wait(NULL);
    }

    msgctl(cola, IPC_RMID, NULL);

    return 0;
}
