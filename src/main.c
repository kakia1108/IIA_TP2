// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pesquisa_local.h"
#include "algoritmo_evolutivo.h"
#include "utils.h"
#include "funcao.h"

#define DEFAULT_RUNS 10
#define DADOS_PATH "dados/"
#define LOG_FILE "resultados_otimizacao.csv"

int main(int argc, char *argv[])
{
    char nome_fich[100];
    char filepath[300];
    int C, m, k, runs;
    double *mat = NULL;
    int *sol = NULL, *best = NULL;
    double custo, best_custo = 0.0;
    double soma_custos = 0.0;

    // Variavel para escolher o algoritmo (1=RS, 2=AE)
    int alg_tipo = 1;

    // ************************************************************
    // PARAMETROS DA RECRISTALIZACAO SIMULADA (RS) - Valores Default
    // ************************************************************
    double tmax = 10.0;
    double tmin = 0.05;
    double farref = 0.99;
    int viz_tipo = 3;
    int aceita_igual = 0;

    // ************************************************************
    // PARAMETROS DO ALGORITMO EVOLUTIVO (AE) - Valores Default
    // ************************************************************
    int tam_pop = 50;
    int num_geracoes = 1000;
    double prob_mut = 0.05;
    double prob_cross = 0.8;
    int tam_torneio = 5;
    int selecao_tipo = 1;
    int cross_tipo = 1;
    int mut_tipo = 1;
    // ************************************************************


    // Processamento dos Argumentos (hibrido: tenta CLI, senao usa scanf)
    if(argc > 1)
    {
        // MODO CLI (Processamento em lote - preferencial para o estudo)
        // 1. Ficheiro (Obrigatorio)
        strcpy(nome_fich, argv[1]);

        // 2. Runs
        runs = (argc > 2) ? atoi(argv[2]) : DEFAULT_RUNS;

        // 3. Algoritmo
        alg_tipo = (argc > 3) ? atoi(argv[3]) : alg_tipo;

        // 4. Parametros especificos (AE/RS)
        if (alg_tipo == 1) // RS (4 argumentos adicionais apos alg_tipo)
        {
            tmax = (argc > 4) ? atof(argv[4]) : tmax;
            farref = (argc > 5) ? atof(argv[5]) : farref;
            viz_tipo = (argc > 6) ? atoi(argv[6]) : viz_tipo;
            aceita_igual = (argc > 7) ? atoi(argv[7]) : aceita_igual;
        }
        else if (alg_tipo == 2) // AE (8 argumentos adicionais apos alg_tipo)
        {
            tam_pop = (argc > 4) ? atoi(argv[4]) : tam_pop;
            num_geracoes = (argc > 5) ? atoi(argv[5]) : num_geracoes;
            prob_mut = (argc > 6) ? atof(argv[6]) : prob_mut;
            prob_cross = (argc > 7) ? atof(argv[7]) : prob_cross;
            tam_torneio = (argc > 8) ? atoi(argv[8]) : tam_torneio;
            selecao_tipo = (argc > 9) ? atoi(argv[9]) : selecao_tipo;
            cross_tipo = (argc > 10) ? atoi(argv[10]) : cross_tipo;
            mut_tipo = (argc > 11) ? atoi(argv[11]) : mut_tipo;
        }
    }
    else
    {
        // MODO INTERATIVO (Apenas o minimo essencial)
        runs = DEFAULT_RUNS;
        printf("Nome do Ficheiro (ex: tourism_5.txt): ");
        if (scanf("%s", nome_fich) != 1) {
            printf("Erro ao ler o nome do ficheiro.\n");
            return 1;
        }

        printf("Tipo de Algoritmo (1=RS, 2=AE) [Default: %d]: ", alg_tipo);
        if (scanf("%d", &alg_tipo) != 1) {
            alg_tipo = 1; // Default
        }

        // **IMPORTANTE**: No modo interativo simplificado, todos os outros parametros
        // (tmax, tam_pop, etc.) usam os valores DEFAULT definidos no topo do ficheiro.
    }

    if(runs <= 0)
        return 0;

    sprintf(filepath, "%s%s", DADOS_PATH, nome_fich);
    init_rand();

    // Ler dados (C e m sao preenchidos aqui)
    mat = init_dados(filepath, &C, &m);

    // NOVO PRINT: Imprimir C e m uma unica vez
    printf("Candidatos: %d, Locais a construir: %d\n", C, m);

    sol = malloc(sizeof(int) * C);
    best = malloc(sizeof(int) * C);
    if(sol == NULL || best == NULL)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    // --- Configuracao da Execucao e Log ---
    char alg_nome[30];

    if (alg_tipo == 1) {
        strcpy(alg_nome, "RS");
    } else if (alg_tipo == 2) {
        strcpy(alg_nome, "AE");
    } else {
        printf("ERRO: Tipo de algoritmo invalido.\n");
        return 1;
    }

    // Escrever o cabecalho de log (run_number=0)
    log_run_result(LOG_FILE, 0, 0.0, C, m, alg_nome,
                   tmax, tmin, farref, 0.0, // RS
                   tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, // AE
                   viz_tipo, mut_tipo, (selecao_tipo==2) ? 1.0 : 0.0, (cross_tipo==2) ? 1.0 : 0.0, aceita_igual, // Variacoes
                   nome_fich);


    printf("\n--- CONFIGURACAO DE TESTE ---\n");
    printf("Algoritmo: %s\n", alg_nome);
    printf("Instancia: %s, Total de runs: %d\n", nome_fich, runs);
    if (alg_tipo == 1) {
        printf("RS Params: TMax=%.2f, Farref=%.3f, Vizinhanca=%d, Aceita Iguais=%s\n",
            tmax, farref, viz_tipo, (aceita_igual==1 ? "Sim" : "Nao"));
    } else {
        printf("AE Params: Pop=%d, Gens=%d, P_Mut=%.2f, P_Cross=%.2f\n", tam_pop, num_geracoes, prob_mut, prob_cross);
        printf("AE Ops: Selecao=%s, Crossover=%s, Mutacao=%s, T_Torneio=%d\n",
            (selecao_tipo == 1 ? "Torneio" : "Roleta"),
            (cross_tipo == 1 ? "1 Ponto" : "2 Pontos"),
            (mut_tipo == 1 ? "Troca" : "Bit"),
            tam_torneio);
    }
    printf("Resultados guardados em: %s\n", LOG_FILE);
    printf("----------------------------\n");


    // Executar runs repeticoes
    for(k = 0; k < runs; k++)
    {
        // Gerar solucao inicial (so e usada por RS)
        gera_sol_inicial(sol, C, m);

        if (alg_tipo == 1) // Recristalizacao Simulada
        {
            custo = recristalizacao(sol, mat, C, m, tmax, tmin, farref, viz_tipo, aceita_igual);
            // Log RS
            log_run_result(LOG_FILE, k + 1, custo, C, m, alg_nome,
                           tmax, tmin, farref, 0.0,
                           0, 0, 0.0, 0.0, 0,
                           viz_tipo, 0, 0.0, 0.0, aceita_igual, // aceita_igual mapeado para o ultimo campo de variacao
                           nome_fich);

        }
        else if (alg_tipo == 2) // Algoritmo Evolutivo
        {
            custo = algoritmo_evolutivo(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo);
            // Log AE
            log_run_result(LOG_FILE, k + 1, custo, C, m, alg_nome,
                           0.0, 0.0, 0.0, 0.0,
                           tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio,
                           0, mut_tipo, (selecao_tipo==2) ? 1.0 : 0.0, (cross_tipo==2) ? 1.0 : 0.0, 0,
                           nome_fich);
        }

        // Escrever resultado da repeticao k
        printf("\n========== Repeticao %d ==========\n", k + 1);
        escreve_sol(sol, C);
        printf("Distancia Media: %.4f\n", custo);

        // Acumular custo para media
        soma_custos += custo;

        // Guardar melhor solucao encontrada
        if(k == 0 || custo > best_custo)
        {
            best_custo = custo;
            substitui(best, sol, C);
        }
    }

    // Escrever resultados globais
    printf("\n\n========== RESULTADOS GLOBAIS ==========\n");
    printf("Algoritmo: %s\n", alg_nome);
    printf("Distancia Media (media de %d runs): %.4f\n", runs, soma_custos / runs);
    printf("\nMelhor solucao encontrada:\n");
    escreve_sol(best, C);
    printf("Melhor Distancia Media: %.4f\n", best_custo);

    // Liberar memoria
    free(mat);
    free(sol);
    free(best);

    return 0;
}