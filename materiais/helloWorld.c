/* hello_mpi.c */
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);               // inicializa o ambiente MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // id do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // número total de processos

    char name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(name, &len);   // nome do nó (opcional)

    printf("Olá do processo %d de %d (host: %s)\n", rank, size, name);

    MPI_Finalize();                       // finaliza o ambiente MPI
    return 0;
}