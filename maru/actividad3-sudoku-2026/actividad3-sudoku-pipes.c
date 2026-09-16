#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

#define cant 9

int sudoku[cant][cant];
void generarMatriz(){
    for(int i=0;i<cant;i++){
        for(int j=0;j<cant;j++){
            sudoku[i][j]=random()%9+1;
            printf("%d ", sudoku[i][j]);
        }
        printf("\n ");
    }
}
    
int validarFilas(){
    int valido=1;//creamos una variable local de la funcion asumiendo que es verdadera y el sudoku es valido
    for(int i=0; i<cant; i++){
        int aux[cant+1] = {0};
        for(int j=0; j<cant; j++){
            if(aux[sudoku[i][j]] == sudoku[i][j]){
                valido=0;
                return valido;
            }
            aux[sudoku[i][j]] = sudoku[i][j];
        }
    }
    return valido;
}

int validarColumnas(){
    int valido=1;
    for(int i=0; i<cant; i++){
        int aux[cant+1] = {0};
        for(int j=0; j<cant; j++){
            if(aux[sudoku[j][i]] == sudoku[j][i]){
               valido=0;
                return valido;
            }
            aux[sudoku[j][i]] = sudoku[j][i];
        }
    }
    return valido;
}

int validarBloques(){
 int valido=1;
for(int i=0; i<cant; i+=3){
        for(int j=0; j<cant; j+=3){
            int aux[cant+1] = {0};
            for(int k=i;k<i+3;k++){
                for(int l=j;l<j+3;l++){
                    if(aux[sudoku[k][l]] == sudoku[k][l]){
                        valido=0;
                        return valido;
                    }
                    aux[sudoku[k][l]] = sudoku[k][l];
                }
            }
        }
}
 return valido;
}


int main(){
    srandom(time(NULL));
    printf("sudoku \n");
   // generarMatriz();
   // printf("\n");
   // si descomentas esto y comentas lo de generar matriz, esta matriz es correcta y no genera errores, 
   // proba cambiando algun nro si queres probar
   //si falla por filas va a fallar por columnas asique para probar el de columnas hay q comentar validarFilas y cuando haces el pipe de filas y todo eso
   /*
  int matrizTest[cant][cant] = { //esta usala apra validar bloques
    {1,2,3,4,5,6,7,8,9},
    {2,3,4,5,6,7,8,9,1},
    {3,4,5,6,7,8,9,1,2},
    {4,5,6,7,8,9,1,2,3},
    {5,6,7,8,9,1,2,3,4},
    {6,7,8,9,1,2,3,4,5},
    {7,8,9,1,2,3,4,5,6},
    {8,9,1,2,3,4,5,6,7},
    {9,1,2,3,4,5,6,7,8}
};
int matrizTest[cant][cant] = { //esta usala para validar filas y columnas
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};
    for(int i=0;i<cant;i++)
        for(int j=0;j<cant;j++)
            sudoku[i][j] = matrizTest[i][j];

    // print de control: lo que ves acá es lo que se valida de verdad
    for(int i=0;i<cant;i++){
        for(int j=0;j<cant;j++) printf("%d ", sudoku[i][j]);
        printf("\n");
    }
    printf("\n");
   */

    int valido=1;
    pid_t procesoFilas;
    int pipeFilas[2]; //creo un pipe para comunicarme con el proceso fila
    pipe(pipeFilas);  // fd[0] = extremo de lectura, fd[1] = extremo de escritura (WRITE_END)

    procesoFilas=fork();
    if(procesoFilas==0){
        close(pipeFilas[0]);
        valido=validarFilas(); //el proceso en su variable valido guarda el valor que se le pasa de resultado
        write(pipeFilas[1], &valido, sizeof(valido));//le pasa el valor de valido al apdre
        close(pipeFilas[1]);
        exit(0);//termina
    }else if (procesoFilas>0){
        close(pipeFilas[1]);
        wait(NULL);//espera a que el hijo filas termine
        read(pipeFilas[0], &valido, sizeof(valido));//lee el valor q le apsa ylo guarda en su variable de valido
        close(pipeFilas[0]);
        if(valido==0){//si no es valido
            printf("El sudoku no es valido porque falla en una fila \n");
            return 0;//terminamos y no hacemos nada mas
        }
    }
    pid_t procesoColumnas;
    int pipeColumnas[2]; //creo un pipe
    pipe(pipeColumnas);  // fd[0] = extremo de lectura (READ_END), fd[1] = extremo de escritura (WRITE_END)

    procesoColumnas=fork();
    if(procesoColumnas==0){
        close(pipeColumnas[0]);
        valido=validarColumnas();
        write(pipeColumnas[1], &valido, sizeof(valido));
        close(pipeColumnas[1]);
        exit(0);
    }else if (procesoColumnas>0){
        close(pipeColumnas[1]);
        wait(NULL);
        read(pipeColumnas[0], &valido, sizeof(valido));
        close(pipeColumnas[0]);
        if(valido==0){
            printf("El sudoku no es valido porque falla en una columna \n");
            return 0;
        }
    }
    pid_t procesoBloques;
    int pipeBloques[2]; //creo un pipe
    pipe(pipeBloques);
    procesoBloques=fork();
    if(procesoBloques==0){
        close(pipeBloques[0]);
        valido=validarBloques();
        write(pipeBloques[1], &valido, sizeof(valido));
        close(pipeBloques[1]);
        exit(0);
    }else if (procesoBloques>0){
        close(pipeBloques[1]);
        wait(NULL);
        read(pipeBloques[0], &valido, sizeof(valido));
        close(pipeBloques[0]);
        if(valido==0){
            printf("El sudoku no es valido porque falla en un bloque \n");
            return 0;
        }
    }

    printf("el sudoku es VALIDO\n");

    return 0;
}
