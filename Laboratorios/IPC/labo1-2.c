#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#define READ 0
#define WRITE 1

struct mensaje{
    int conteo;
    char vocal;
};

int main(int argc, char *argv[])
{
    // NO TOCAR: creación de los pipes
    // Si el ejercicio es unidireccional, borrar pipeFD2 y todo lo que lo usa.
    int pipeFD[2]; // padre -> hijo // hijo -> padre
    char vocales[5]={'a', 'e', 'i', 'o', 'u'};
    struct mensaje msj;
    FILE *archivo;
    if (pipe(pipeFD) == -1) {
        perror("Error al crear pipeFD");
        exit(1);
        }
    for(int i=0;i<5;i++){
        pid_t pid;
        
        int cantidad=0;
        pid=fork();
        if (pid < 0) {
            printf("Error al crear proceso");
            exit(1);
        }
        if(pid==0){
            sleep(5);
            int c=0;
            close(pipeFD[READ]);
            archivo=fopen("vocales.txt", "r");
            while((c=fgetc(archivo))!=EOF){

                if(vocales[i]== (char) tolower(c)){
                    cantidad++;
                }
            }
            msj.conteo=cantidad;
            msj.vocal=vocales[i];
            write(pipeFD[WRITE], &msj, sizeof(msj));
            close(pipeFD[WRITE]);
            exit(0);
        }
    }

    close(pipeFD[WRITE]);
    for(int i=0; i<5; i++){
        read(pipeFD[READ], &msj, sizeof(msj));
        printf("Se encontraron %d vocales %c\n", msj.conteo, msj.vocal);
    }
    close(pipeFD[READ]);
   
    return 0;
}