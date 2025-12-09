#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "utils.h"
#include "funcao.h"


// Leitura do ficheiro de input (formato turismo)
double* init_dados(char *nome, int *C, int *m)
{
    FILE *f;
    double *p;
    char label1[5], label2[5];
    double dist;

    f = fopen(nome, "r");
    if(!f)
    {
        printf("Erro no acesso ao ficheiro dos dados\n");
        exit(1);
    }

    // 1. Ler C (número de candidatos) e m (número de locais a construir)
    fscanf(f, "%d %d", C, m);

    // REMOVIDO: printf("Candidatos: %d, Locais a construir: %d\n", *C, *m);

    // 2. Alocação dinâmica da matriz de distâncias e inicialização a ZERO
    p = calloc((*C) * (*C), sizeof(double));
    if(!p)
    {
        printf("Erro na alocacao da memoria\n");
        exit(1);
    }

    // 3. Ler o resto do ficheiro
    while(fscanf(f, "%4s %4s %lf", label1, label2, &dist) == 3)
    {
        int idx1 = atoi(label1 + 1) - 1;
        int idx2 = atoi(label2 + 1) - 1;

        if (idx1 >= 0 && idx1 < *C && idx2 >= 0 && idx2 < *C)
        {
            p[idx1 * (*C) + idx2] = dist;
            p[idx2 * (*C) + idx1] = dist;
        }
    }

    fclose(f);
    return p;
}

// Gera a solução inicial com exatamente m locais selecionados
// Parametros: solução, número de candidatos, número de locais a construir
void gera_sol_inicial(int *sol, int C, int m)
{
    int i, x;

    // Inicializar todos com 0 (não selecionados)
    for(i = 0; i < C; i++)
        sol[i] = 0;

    // Selecionar m locais aleatoriamente
    for(i = 0; i < m; i++)
    {
        do
            x = random_l_h(0, C - 1);
        while(sol[x] != 0);  // Se já foi selecionado, tenta outro
        sol[x] = 1;
    }
}

// Escreve solução
// Parametros: solução, número de candidatos
void escreve_sol(int *sol, int C)
{
    int i, count = 0;

    printf("\nLocais selecionados: ");
    for(i = 0; i < C; i++)
    {
        if(sol[i] == 1)
        {
            printf("%d  ", i + 1);  // Mostrar de 1 a C (mais intuitivo)
            count++;
        }
    }
    printf("\n(Total: %d locais)\n", count); // 'locais' mantem-se
}

// ...
// Implementação COMPLETA da função de log (21 parâmetros)
void log_run_result(char *filepath, int run_number, double custo, int C, int m, char *alg_nome,
                    double tmax, double tmin, double farref, double temperatura_final,
                    int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio,
                    int viz_tipo, int num_mutacoes, double prob_sel_roleta, double prob_cross_uniforme, int num_cross_dois_pontos,
                    char *nome_fich)
{
    // Usar 'a' (append) para adicionar ao ficheiro
    FILE *f = fopen(filepath, "a");

    if (!f)
    {
        printf("ERRO: Nao foi possivel abrir o ficheiro de log: %s\n", filepath);
        return;
    }

    // Se run_number for 0, escreve o cabecalho (apenas 1 vez)
    if (run_number == 0)
    {
        // CABECALHO LIMPO DE ACENTOS para o CSV (compatibilidade com Excel/sistemas)
        fprintf(f, "Instancia;Candidatos;Locais;Algoritmo;Run;Custo;");
        fprintf(f, "RS_TMax;RS_TMin;RS_Farref;RS_Vizinhanca;RS_TFinal;");
        fprintf(f, "AE_TamPop;AE_Geracoes;AE_ProbMut;AE_ProbCross;AE_TamTorneio;");
        fprintf(f, "AE_NumMut;AE_ProbRoleta;AE_ProbUniforme;AE_Cross2Pontos\n");
    }
    else
    {
        // Escreve os dados da run, usando 0/0.0 para parametros nao aplicaveis
        fprintf(f, "%s;%d;%d;%s;%d;%.4f;",
                nome_fich, C, m, alg_nome, run_number, custo);

        // Parametros RS
        fprintf(f, "%.2f;%.2f;%.3f;%d;%.2f;",
                tmax, tmin, farref, viz_tipo, temperatura_final);

        // Parametros AE
        fprintf(f, "%d;%d;%.2f;%.2f;%d;",
                tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio);

        // Parametros de Variacao (para o estudo experimental futuro)
        fprintf(f, "%d;%.2f;%.2f;%d\n",
                num_mutacoes, prob_sel_roleta, prob_cross_uniforme, num_cross_dois_pontos);
    }

    fclose(f);
}

// Copia vector b para a (tamanho n)
void substitui(int a[], int b[], int n)
{
    int i;
    for(i = 0; i < n; i++)
        a[i] = b[i];
}

// Inicializa o gerador de números aleatórios
void init_rand()
{
    srand((unsigned)time(NULL));
}

// Devolve valor inteiro aleatório entre min e max (inclusive)
int random_l_h(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Devolve um valor real aleatório do intervalo [0, 1]
float rand_01()
{
    return ((float)rand()) / RAND_MAX;
}