#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TAG_FINALIZA 0
#define TAG_MEDIA 1
#define TAG_FILTRO 2
#define TAG_ORDENAR 3
#define TAG_DESVIO_PADRAO 4

#define MAX_VETOR_SIZE 100

void simular_processamento_pesado();
void calcular_media(int* vetor, int tamanho);
void aplicar_filtro_passa_baixa(int* vetor, int tamanho);
void ordenar_vetor_bubble(int* vetor, int tamanho);
void calcular_desvio_padrao(int* vetor, int tamanho);
void imprimir_vetor(int* vetor, int tamanho, const char* titulo);

void mestre(int world_size);
void escravo(int world_rank);

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2)
    {
        fprintf(stderr, "sao necessarios pelo menos 2 processos (1 mestre e 1+ escravos).\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (world_rank == 0)
    {
        mestre(world_size);
    }
    else
    {
        escravo(world_rank);
    }

    MPI_Finalize();
    return 0;
}

void mestre(int world_size)
{
    srand(time(NULL));

    const int TOTAL_TAREFAS = (rand() % 15) + 10;

    int num_escravos = world_size - 1;
    int tarefas_enviadas = 0;
    int tarefas_concluidas = 0;


    int vetor[MAX_VETOR_SIZE];

    printf("MESTRE: Iniciando com %d escravos. Total de tarefas a serem executadas: %d\n", num_escravos, TOTAL_TAREFAS);

    for (int i = 1; i <= num_escravos; i++)
    {
        if (tarefas_enviadas < TOTAL_TAREFAS)
        {
            int tamanho_vetor = (rand() % 20) + 5;
            int tag_tarefa = (rand() % 4) + 1;
            for (int j = 0; j < tamanho_vetor; j++)
            {
                vetor[j] = rand() % 100;
            }
            MPI_Send(vetor, tamanho_vetor, MPI_INT, i, tag_tarefa, MPI_COMM_WORLD);
            tarefas_enviadas++;
            printf("MESTRE: Tarefa %d (tag %d) enviada para o escravo %d.\n", tarefas_enviadas, tag_tarefa, i);
        }
    }

    MPI_Status status;
    int resultado_escravo;
    while (tarefas_concluidas < TOTAL_TAREFAS)
    {
        MPI_Recv(&resultado_escravo, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        int id_escravo_livre = status.MPI_SOURCE;
        tarefas_concluidas++;
        printf("MESTRE: Escravo %d terminou sua tarefa. concluidas: %d/%d.\n", id_escravo_livre, tarefas_concluidas, TOTAL_TAREFAS);

        if (tarefas_enviadas < TOTAL_TAREFAS)
        {
            int tamanho_vetor = (rand() % 20) + 5;
            int tag_tarefa = (rand() % 4) + 1;
            for (int j = 0; j < tamanho_vetor; j++)
            {
                vetor[j] = rand() % 100;
            }
            MPI_Send(vetor, tamanho_vetor, MPI_INT, id_escravo_livre, tag_tarefa, MPI_COMM_WORLD);
            tarefas_enviadas++;
            printf("MESTRE: Nova tarefa %d (tag %d) enviada para o escravo %d.\n", tarefas_enviadas, tag_tarefa, id_escravo_livre);
        }
    }

    printf("MESTRE: Todas tarefas terminadas. Enviando sinal de termino.\n");
    for (int i = 1; i <= num_escravos; i++)
    {
        MPI_Send(NULL, 0, MPI_INT, i, TAG_FINALIZA, MPI_COMM_WORLD);
    }
}

void escravo(int world_rank)
{

    int vetor[MAX_VETOR_SIZE];
    int tamanho_vetor;
    MPI_Status status;

    while (1)
    {
        MPI_Recv(vetor, MAX_VETOR_SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_FINALIZA)
        {
            printf("ESCRAVO %d: Sinal de finalizacao recebido. Encerrando.\n", world_rank);
            break;
        }

        MPI_Get_count(&status, MPI_INT, &tamanho_vetor);

        switch (status.MPI_TAG)
        {
        case TAG_MEDIA:
            printf("ESCRAVO %d: Recebeu tarefa de MEDIA com %d elementos.\n", world_rank, tamanho_vetor);
            calcular_media(vetor, tamanho_vetor);
            break;
        case TAG_FILTRO:
            printf("ESCRAVO %d: Recebeu tarefa de FILTRO com %d elementos.\n", world_rank, tamanho_vetor);
            aplicar_filtro_passa_baixa(vetor, tamanho_vetor);
            break;
        case TAG_ORDENAR:
            printf("ESCRAVO %d: Recebeu tarefa de ORDENACAO com %d elementos.\n", world_rank, tamanho_vetor);
            ordenar_vetor_bubble(vetor, tamanho_vetor);
            break;
        case TAG_DESVIO_PADRAO:
            printf("ESCRAVO %d: Recebeu tarefa de DESVIO PADRAO com %d elementos.\n", world_rank, tamanho_vetor);
            calcular_desvio_padrao(vetor, tamanho_vetor);
            break;
        default:
            printf("ESCRAVO %d: Recebeu tag desconhecido: %d.\n", world_rank, status.MPI_TAG);
        }

        int confirmacao = 1;
        MPI_Send(&confirmacao, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

void simular_processamento_pesado()
{
    volatile double a = 1.0;
    long iterations = 200000000;
    for (long i = 0; i < iterations; i++)
    {
        a = a + 1.00000001;
    }
}

void calcular_media(int* vetor, int tamanho)
{
    simular_processamento_pesado();
    long long soma = 0;
    for (int i = 0; i < tamanho; i++)
    {
        soma += vetor[i];
    }
    double media = (double)soma / tamanho;
    printf("Resultado Media: %.2f\n\n", media);
}

void aplicar_filtro_passa_baixa(int* vetor, int tamanho)
{
    simular_processamento_pesado();
    if (tamanho < 3)
    {
        printf("Filtro nao aplicavel (vetor muito pequeno).\n\n");
        imprimir_vetor(vetor, tamanho, "Vetor Original");
        return;
    }


    int* vetor_filtrado = (int*)malloc(tamanho * sizeof(int));
    if (vetor_filtrado == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria!\n");
        return; 
    }

    vetor_filtrado[0] = vetor[0];
    vetor_filtrado[tamanho - 1] = vetor[tamanho - 1];

    for (int i = 1; i < tamanho - 1; i++)
    {
        vetor_filtrado[i] = (vetor[i - 1] + vetor[i] + vetor[i + 1]) / 3;
    }
    imprimir_vetor(vetor, tamanho, "Vetor Original");
    imprimir_vetor(vetor_filtrado, tamanho, "Vetor Filtrado");
    printf("\n");

    free(vetor_filtrado);
}

void ordenar_vetor_bubble(int* vetor, int tamanho)
{
    simular_processamento_pesado();
    imprimir_vetor(vetor, tamanho, "Vetor Original");
    for (int i = 0; i < tamanho - 1; i++)
    {
        for (int j = 0; j < tamanho - i - 1; j++)
        {
            if (vetor[j] > vetor[j + 1])
            {
                int temp = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = temp;
            }
        }
    }
    imprimir_vetor(vetor, tamanho, "Vetor Ordenado");
    printf("\n");
}

void calcular_desvio_padrao(int* vetor, int tamanho)
{
    simular_processamento_pesado();
    long long soma = 0;
    for (int i = 0; i < tamanho; i++)
    {
        soma += vetor[i];
    }
    double media = (double)soma / tamanho;

    double soma_diferencas = 0;
    for (int i = 0; i < tamanho; i++)
    {
        soma_diferencas += pow(vetor[i] - media, 2);
    }
    double desvio_padrao = sqrt(soma_diferencas / tamanho);
    printf("Resultado Desvio Padrao: %.2f\n\n", desvio_padrao);
}

void imprimir_vetor(int* vetor, int tamanho, const char* titulo)
{
    printf("%s: [ ", titulo);
    for (int i = 0; i < tamanho; i++)
    {
        printf("%d ", vetor[i]);
    }
    printf("]\n");
}