#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define DIMENSAO 4

void imprimir_matriz(double matriz[DIMENSAO][DIMENSAO]) {
    for (int i = 0; i < DIMENSAO; i++) {
        printf("[ ");
        for (int j = 0; j < DIMENSAO; j++) {
            printf("%6.2f ", matriz[i][j]);
        }
        printf("]\n");
    }
}

int main(int argc, char** argv) {
    int id_processo, total_processos;
    double matriz_A[DIMENSAO][DIMENSAO];
    double matriz_B[DIMENSAO][DIMENSAO];
    double matriz_Resultado[DIMENSAO][DIMENSAO];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_processo);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processos);

    if (DIMENSAO % total_processos != 0) {
        if (id_processo == 0) {
            fprintf(stderr, "erro: dimensao da matriz (%d) nao eh divisivel pelo numero de processos (%d)\n", DIMENSAO, total_processos);
        }
        MPI_Finalize();
        return 1;
    }

    if (id_processo == 0) {
        printf("iniciando calculo paralelo com %d processos\n", total_processos);
        srand(time(NULL));
        for (int i = 0; i < DIMENSAO; i++) {
            for (int j = 0; j < DIMENSAO; j++) {
                matriz_A[i][j] = (double)(rand() % 10);
                matriz_B[i][j] = (double)(rand() % 10);
            }
        }
        printf("\nmatriz a:\n");
        imprimir_matriz(matriz_A);
        printf("\nmatriz b:\n");
        imprimir_matriz(matriz_B);
    }

    int linhas_por_processo = DIMENSAO / total_processos;

    double *pedaco_local_A = (double*)malloc(linhas_por_processo * DIMENSAO * sizeof(double));
    double *pedaco_local_Resultado = (double*)malloc(linhas_por_processo * DIMENSAO * sizeof(double));

    if (pedaco_local_A == NULL || pedaco_local_Resultado == NULL) {
        fprintf(stderr, "processo %d: falha em alocar memoria\n", id_processo);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Bcast(matriz_B, DIMENSAO * DIMENSAO, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Scatter(matriz_A, linhas_por_processo * DIMENSAO, MPI_DOUBLE,
                pedaco_local_A, linhas_por_processo * DIMENSAO, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    printf("processo %d: calculando minha parte (%d linha(s))\n", id_processo, linhas_por_processo);
    for (int i = 0; i < linhas_por_processo; i++) {
        for (int j = 0; j < DIMENSAO; j++) {
            pedaco_local_Resultado[i * DIMENSAO + j] = 0.0;
            for (int k = 0; k < DIMENSAO; k++) {
                pedaco_local_Resultado[i * DIMENSAO + j] += pedaco_local_A[i * DIMENSAO + k] * matriz_B[k][j];
            }
        }
    }

    MPI_Gather(pedaco_local_Resultado, linhas_por_processo * DIMENSAO, MPI_DOUBLE,
               matriz_Resultado, linhas_por_processo * DIMENSAO, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (id_processo == 0) {
        printf("\nmatriz resultante (a * b):\n");
        imprimir_matriz(matriz_Resultado);
    }

    free(pedaco_local_A);
    free(pedaco_local_Resultado);

    MPI_Finalize();
    return 0;
}