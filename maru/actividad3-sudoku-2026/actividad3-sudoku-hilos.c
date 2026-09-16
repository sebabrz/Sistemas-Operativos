#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

#define cant 9
int filaInvalida = -1;
int columnaInvalida= -1;
int bloqueInvalido= -1;
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
    
void *validarFilas(void *data){
    for(int i=0; i<cant; i++){
        int aux[cant+1] = {0};
        for(int j=0; j<cant; j++){
            if(aux[sudoku[i][j]] == sudoku[i][j]){
                filaInvalida = i;
                return NULL;
            }
            aux[sudoku[i][j]] = sudoku[i][j];
        }
    }
    return NULL;
}
void *validarColumnas(void *data){
    for(int i=0; i<cant; i++){
        int aux[cant+1] = {0};
        for(int j=0; j<cant; j++){
            if(aux[sudoku[j][i]] == sudoku[j][i]){
                columnaInvalida = i;
                return NULL;
            }
            aux[sudoku[j][i]] = sudoku[j][i];
        }
    }
    return NULL;
}
 
void* validarBloques(void *data){
for(int i=0; i<cant; i+=3){
        for(int j=0; j<cant; j+=3){
            int aux[cant+1] = {0};
            for(int k=i;k<i+3;k++){
                for(int l=j;l<j+3;l++){
                    if(aux[sudoku[k][l]] == sudoku[k][l]){
                        bloqueInvalido = (i/3)*3 + (j/3);
                        return NULL;
                    }
                    aux[sudoku[k][l]] = sudoku[k][l];
                }
            }
        }
}
 return NULL;
}

int main(){
    srandom(time(NULL));
    printf("sudoku \n");
    generarMatriz();
    printf("\n");
    
    pthread_t hiloFilas[1];
    pthread_t hiloColumnas[1];
    pthread_t hiloBloques[cant];

    pthread_create(&hiloFilas[1], NULL, validarFilas, NULL);
    pthread_join(hiloFilas[1], NULL);
    
    if(filaInvalida == -1){
        printf("sudoku valido (filas)\n");
    } else {
        printf("no valida fila %d\n", filaInvalida+1);
        return 0;
    }

    pthread_create(&hiloColumnas[1], NULL, validarColumnas, NULL);
    pthread_join(hiloColumnas[1], NULL);
    if(columnaInvalida == -1){
        printf("sudoku valido (columnas)\n");
    } else {
        printf("no valida columna %d\n", columnaInvalida+1);
        return 0;
    }

    pthread_create(&hiloBloques[1], NULL, validarBloques, NULL);
    pthread_join(hiloBloques[1], NULL);
    if(bloqueInvalido == -1){
        printf("sudoku valido (bloques)\n");
    } else {
        printf("no valido bloque %d\n", bloqueInvalido+1);
        return 0;
    }

    printf("el sudoku es VALIDO");

    return 0;
}
