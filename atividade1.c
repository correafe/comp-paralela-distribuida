#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_size < 2) {
    fprintf(stderr, "numero de processos deve ser no minimo 2 para %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int counter;

  int next_rank = (world_rank + 1) % world_size;
  int prev_rank = (world_rank - 1 + world_size) % world_size;

  if (world_rank == 0) {

    counter = 1;
    printf("Processo %d: iniciando, enviando contador %d para o processo %d\n", world_rank, counter, next_rank);
    MPI_Send(&counter, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);

    MPI_Recv(&counter, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Processo %d: recebeu contador final %d do processo %d\n", world_rank, counter, prev_rank);
    printf("--- anel completo. Valor final do contador: %d ---\n", counter);

  } else {
    MPI_Recv(&counter, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Processo %d: recebeu contador %d do processo %d\n", world_rank, counter, prev_rank);
    counter++;

    printf("Processo %d: incrementou e esta enviando o contador %d para o processo %d\n", world_rank, counter, next_rank);
    MPI_Send(&counter, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
