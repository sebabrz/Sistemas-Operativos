#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

int main(){
pid_t hijo;
pid_t hijos[2];
int i=0, j=0;
while(i<2){
hijo= fork();
if(hijo==0){
    printf("Soy un hijo, PID=%d\n", getpid());
    if(i==0){//directorio
        DIR *dir = opendir("/etc");//declaramos puntero tipo DIR que apunta al directorio abierto etc
        if (dir == NULL) {
            printf("Error al abrir el directorio\n");
            exit(1);
        }
        struct dirent *entrada; //creamos un puntero a una estructura dirent, cada vez que lea una entrada del directorio etc, guardo
                    //la info en esta struct
        while ((entrada = readdir(dir)) != NULL) {//mientras tenga entradas que leer, 
            printf("%s\n", entrada->d_name);//d_name es el campo de la estructura que tiene el nombre de la entrada del directorio
        }
        closedir(dir);
    }else{//contador
        for (int i = 1; i <= 15; i++) {
        printf("%d\n", i);
        }
    }
exit(0);
}else{

    hijos[i]=hijo;
}
i++;
}
while(j<2){
    waitpid(hijos[j],NULL,0);
    j++;
}
printf("Padre: ambos hijos terminaron\n");
return 0;
}