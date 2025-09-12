#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mpi.h>

#define LIMIAR_BRILHO_ESTRELA 200
#define LINHAS_GRID 4
#define COLUNAS_GRID 4

#define TAG_TAREFA_ENVIADA 1
#define TAG_DADOS_ENVIADOS 2
#define TAG_RESULTADO_ENVIADO 3
#define TAG_TERMINAR 4

typedef struct {
    int altura_bloco;
    int largura_bloco;
} InfoTarefa;

void ignorar_comentarios_pgm(FILE *arquivo) {
    int caractere;
    char linha[100];
    while ((caractere = fgetc(arquivo)) != EOF && (caractere == ' ' || caractere == '\t' || caractere == '\n'));
    if (caractere == '#') {
        fgets(linha, sizeof(linha), arquivo);
        ignorar_comentarios_pgm(arquivo);
    } else if (caractere != EOF) {
        fseek(arquivo, -1, SEEK_CUR);
    }
}

void executar_mestre(int total_processos, char* nome_arquivo) {
    FILE *arquivo_imagem = fopen(nome_arquivo, "rb");
    if (!arquivo_imagem) {
        fprintf(stderr, "erro: nao foi possivel abrir o arquivo '%s'.\n", nome_arquivo);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char tipo_imagem[3];
    int largura, altura, valor_max;
    fscanf(arquivo_imagem, "%2s", tipo_imagem);
    ignorar_comentarios_pgm(arquivo_imagem);
    fscanf(arquivo_imagem, "%d %d", &largura, &altura);
    ignorar_comentarios_pgm(arquivo_imagem);
    fscanf(arquivo_imagem, "%d", &valor_max);
    fgetc(arquivo_imagem);

    unsigned char *dados_imagem = (unsigned char*)malloc(altura * largura * sizeof(unsigned char));
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int valor_pixel;
            fscanf(arquivo_imagem, "%d", &valor_pixel);
            dados_imagem[i * largura + j] = (unsigned char)valor_pixel;
        }
    }
    fclose(arquivo_imagem);

    int total_tarefas = LINHAS_GRID * COLUNAS_GRID;
    int altura_base_bloco = altura / LINHAS_GRID;
    int largura_base_bloco = largura / COLUNAS_GRID;
    int tarefas_enviadas = 0;
    int contagem_total_estrelas = 0;
    int num_escravos = total_processos - 1;
    MPI_Status status;

    printf("[Mestre] analisando imagem '%s' (%dx%d).\n", nome_arquivo, largura, altura);
    printf("[Mestre] distribuindo %d tarefas para %d escravos...\n\n", total_tarefas, num_escravos);

    for (int i = 1; i <= num_escravos && tarefas_enviadas < total_tarefas; i++) {
        int idx_linha = tarefas_enviadas / COLUNAS_GRID;
        int idx_coluna = tarefas_enviadas % COLUNAS_GRID;
        InfoTarefa info_tarefa;
        info_tarefa.altura_bloco = (idx_linha == LINHAS_GRID - 1) ? altura - (idx_linha * altura_base_bloco) : altura_base_bloco;
        info_tarefa.largura_bloco = (idx_coluna == COLUNAS_GRID - 1) ? largura - (idx_coluna * largura_base_bloco) : largura_base_bloco;
        int linha_inicial = idx_linha * altura_base_bloco;
        int coluna_inicial = idx_coluna * largura_base_bloco;

        MPI_Send(&info_tarefa, sizeof(InfoTarefa), MPI_BYTE, i, TAG_TAREFA_ENVIADA, MPI_COMM_WORLD);
        
        unsigned char* buffer_bloco = (unsigned char*)malloc(info_tarefa.altura_bloco * info_tarefa.largura_bloco * sizeof(unsigned char));
        for(int r = 0; r < info_tarefa.altura_bloco; r++) {
            for(int c = 0; c < info_tarefa.largura_bloco; c++) {
                buffer_bloco[r * info_tarefa.largura_bloco + c] = dados_imagem[(linha_inicial + r) * largura + (coluna_inicial + c)];
            }
        }
        MPI_Send(buffer_bloco, info_tarefa.altura_bloco * info_tarefa.largura_bloco, MPI_UNSIGNED_CHAR, i, TAG_DADOS_ENVIADOS, MPI_COMM_WORLD);
        free(buffer_bloco);
        tarefas_enviadas++;
    }

    int resultados_recebidos = 0;
    while (resultados_recebidos < total_tarefas) {
        int contagem_parcial;
        MPI_Recv(&contagem_parcial, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULTADO_ENVIADO, MPI_COMM_WORLD, &status);
        contagem_total_estrelas += contagem_parcial;
        resultados_recebidos++;
        int id_escravo = status.MPI_SOURCE;

        if (tarefas_enviadas < total_tarefas) {
            int idx_linha = tarefas_enviadas / COLUNAS_GRID;
            int idx_coluna = tarefas_enviadas % COLUNAS_GRID;
            InfoTarefa info_tarefa;
            info_tarefa.altura_bloco = (idx_linha == LINHAS_GRID - 1) ? altura - (idx_linha * altura_base_bloco) : altura_base_bloco;
            info_tarefa.largura_bloco = (idx_coluna == COLUNAS_GRID - 1) ? largura - (idx_coluna * largura_base_bloco) : largura_base_bloco;
            int linha_inicial = idx_linha * altura_base_bloco;
            int coluna_inicial = idx_coluna * largura_base_bloco;
            
            MPI_Send(&info_tarefa, sizeof(InfoTarefa), MPI_BYTE, id_escravo, TAG_TAREFA_ENVIADA, MPI_COMM_WORLD);
            
            unsigned char* buffer_bloco = (unsigned char*)malloc(info_tarefa.altura_bloco * info_tarefa.largura_bloco * sizeof(unsigned char));
            for(int r = 0; r < info_tarefa.altura_bloco; r++) {
                for(int c = 0; c < info_tarefa.largura_bloco; c++) {
                    buffer_bloco[r * info_tarefa.largura_bloco + c] = dados_imagem[(linha_inicial + r) * largura + (coluna_inicial + c)];
                }
            }
            MPI_Send(buffer_bloco, info_tarefa.altura_bloco * info_tarefa.largura_bloco, MPI_UNSIGNED_CHAR, id_escravo, TAG_DADOS_ENVIADOS, MPI_COMM_WORLD);
            free(buffer_bloco);
            tarefas_enviadas++;
        }
    }

    for (int i = 1; i <= num_escravos; i++) {
        MPI_Send(NULL, 0, MPI_BYTE, i, TAG_TERMINAR, MPI_COMM_WORLD);
    }

    printf("  arquivo processado: %s\n", nome_arquivo);
    printf("  numero total de estrelas encontradas: %d\n", contagem_total_estrelas);
    free(dados_imagem);
}

void executar_escravo() {
    while (true) {
        InfoTarefa info_tarefa;
        MPI_Status status;
        MPI_Recv(&info_tarefa, sizeof(InfoTarefa), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_TERMINAR) {
            break; 
        }

        int tamanho_bloco = info_tarefa.altura_bloco * info_tarefa.largura_bloco;
        unsigned char* dados_bloco = (unsigned char*)malloc(tamanho_bloco * sizeof(unsigned char));
        MPI_Recv(dados_bloco, tamanho_bloco, MPI_UNSIGNED_CHAR, 0, TAG_DADOS_ENVIADOS, MPI_COMM_WORLD, &status);

        int contagem_local_estrelas = 0;
        int largura = info_tarefa.largura_bloco;

        for (int r = 1; r < info_tarefa.altura_bloco - 1; r++) {
            for (int c = 1; c < info_tarefa.largura_bloco - 1; c++) {
                int idx = r * largura + c;
                unsigned char valor_pixel = dados_bloco[idx];
                if (valor_pixel > LIMIAR_BRILHO_ESTRELA) {
                    bool eh_maximo_local = 
                        valor_pixel > dados_bloco[idx - largura - 1] && valor_pixel > dados_bloco[idx - largura] && valor_pixel > dados_bloco[idx - largura + 1] &&
                        valor_pixel > dados_bloco[idx - 1]           &&                                          valor_pixel > dados_bloco[idx + 1]           &&
                        valor_pixel > dados_bloco[idx + largura - 1] && valor_pixel > dados_bloco[idx + largura] && valor_pixel > dados_bloco[idx + largura + 1];
                    if (eh_maximo_local) {
                        contagem_local_estrelas++;
                    }
                }
            }
        }
        free(dados_bloco);
        MPI_Send(&contagem_local_estrelas, 1, MPI_INT, 0, TAG_RESULTADO_ENVIADO, MPI_COMM_WORLD);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int total_processos, rank_processo;
    MPI_Comm_size(MPI_COMM_WORLD, &total_processos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_processo);

    if (rank_processo == 0) {
        if (argc < 2) {
            fprintf(stderr, "Uso: mpiexec -n <numero_processos> %s <arquivo_imagem.pgm>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        executar_mestre(total_processos, argv[1]);
    } else {
        executar_escravo();
    }

    MPI_Finalize();
    return 0;
}