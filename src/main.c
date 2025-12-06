// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pesquisa_local.h" // Novo include
#include "algoritmo_evolutivo.h" // Novo include
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

    // Variável para escolher o algoritmo (1=RS, 2=AE)
    int alg_tipo = 1; // Default: Recristalização Simulada (RS)

    // ************************************************************
    // PARÂMETROS DA RECRISTALIZAÇÃO SIMULADA (RS)
    // ************************************************************
    double tmax = 10.0;
    double tmin = 0.05;
    double farref = 0.99;
    int viz_tipo = 3; // Default: 3 (Alternar V1 e V2)

    // ************************************************************
    // PARÂMETROS DO ALGORITMO EVOLUTIVO (AE)
    // ************************************************************
    int tam_pop = 50;
    int num_geracoes = 1000;
    double prob_mut = 0.05;
    double prob_cross = 0.8;
    int tam_torneio = 5;
    int selecao_tipo = 1; // 1: Torneio, 2: Roleta
    int cross_tipo = 1; // 1: Ponto Único, 2: Dois Pontos
    int mut_tipo = 1; // 1: Troca, 2: Por Bit
    // ************************************************************


    // Processamento dos Argumentos (híbrido: tenta CLI, senão usa scanf)
    if(argc > 1)
    {
        // 1. Ficheiro (Obrigatório)
        strcpy(nome_fich, argv[1]);

        // 2. Runs
        runs = (argc > 2) ? atoi(argv[2]) : DEFAULT_RUNS;

        // 3. Algoritmo (Novo Argumento)
        alg_tipo = (argc > 3) ? atoi(argv[3]) : alg_tipo;

        // 4. Parâmetros específicos (ajusta-se à quantidade de argumentos para RS ou AE)
        if (alg_tipo == 1) // RS (3 argumentos adicionais após alg_tipo)
        {
            tmax = (argc > 4) ? atof(argv[4]) : tmax;
            farref = (argc > 5) ? atof(argv[5]) : farref;
            viz_tipo = (argc > 6) ? atoi(argv[6]) : viz_tipo;
        }
        else if (alg_tipo == 2) // AE (8 argumentos adicionais após alg_tipo)
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
        // MODO INTERATIVO
        runs = DEFAULT_RUNS;
        printf("Nome do Ficheiro (ex: tourism_5.txt): ");
        if (scanf("%s", nome_fich) != 1) {
            printf("Erro ao ler o nome do ficheiro.\n");
            return 1;
        }

        printf("Tipo de Algoritmo (1=RS, 2=AE): ");
        if (scanf("%d", &alg_tipo) != 1) {
            alg_tipo = 1; // Default
        }

        // BLOCO: PEDIR CONFIGURAÇÃO DO AE SE FOR ESCOLHIDO O AE NO MODO INTERATIVO
        if (alg_tipo == 2) {
            printf("\n--- CONFIGURAÇÃO AE ---\n");
            printf("Tamanho da População [%d]: ", tam_pop);
            scanf("%d", &tam_pop);

            printf("Número de Gerações [%d]: ", num_geracoes);
            scanf("%d", &num_geracoes);

            printf("Prob. Mutação (0.0 a 1.0) [%.2f]: ", prob_mut);
            scanf("%lf", &prob_mut);

            printf("Prob. Crossover (0.0 a 1.0) [%.2f]: ", prob_cross);
            scanf("%lf", &prob_cross);

            printf("Tipo de Seleção (1=Torneio, 2=Roleta) [%d]: ", selecao_tipo);
            scanf("%d", &selecao_tipo);

            printf("Tipo de Crossover (1=1 Ponto, 2=2 Pontos) [%d]: ", cross_tipo);
            scanf("%d", &cross_tipo);

            printf("Tipo de Mutação (1=Troca, 2=Bit) [%d]: ", mut_tipo);
            scanf("%d", &mut_tipo);

            printf("Tamanho Torneio (k) [%d]: ", tam_torneio);
            scanf("%d", &tam_torneio);
        }
        // FIM DO BLOCO INTERATIVO
    }

    if(runs <= 0)
        return 0;

    sprintf(filepath, "%s%s", DADOS_PATH, nome_fich);
    init_rand();

    mat = init_dados(filepath, &C, &m);

    sol = malloc(sizeof(int) * C);
    best = malloc(sizeof(int) * C);
    if(sol == NULL || best == NULL)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    // --- Configuração da Execução e Log ---
    char alg_nome[30];

    if (alg_tipo == 1) {
        strcpy(alg_nome, "RS");
    } else if (alg_tipo == 2) {
        strcpy(alg_nome, "AE");
    } else {
        printf("ERRO: Tipo de algoritmo invalido.\n");
        return 1;
    }

    // Escrever o cabeçalho de log (run_number=0)
    log_run_result(LOG_FILE, 0, 0.0, C, m, alg_nome,
                   tmax, tmin, farref, 0.0, // RS
                   tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, // AE
                   viz_tipo, mut_tipo, (selecao_tipo==2) ? 1.0 : 0.0, (cross_tipo==2) ? 1.0 : 0.0, 0, // Variações
                   nome_fich);


    printf("\n--- CONFIGURAÇÃO DE TESTE ---\n");
    printf("Algoritmo: %s\n", alg_nome);
    printf("Instancia: %s, Total de runs: %d\n", nome_fich, runs);
    if (alg_tipo == 1) {
        printf("RS Params: TMax=%.2f, Farref=%.3f, Vizinhança=%d\n", tmax, farref, viz_tipo);
    } else {
        printf("AE Params: Pop=%d, Gens=%d, P_Mut=%.2f, P_Cross=%.2f\n", tam_pop, num_geracoes, prob_mut, prob_cross);
        printf("AE Ops: Seleção=%s, Crossover=%s, Mutação=%s, T_Torneio=%d\n",
            (selecao_tipo == 1 ? "Torneio" : "Roleta"),
            (cross_tipo == 1 ? "1 Ponto" : "2 Pontos"),
            (mut_tipo == 1 ? "Troca" : "Bit"),
            tam_torneio);
    }
    printf("Resultados guardados em: %s\n", LOG_FILE);
    printf("----------------------------\n");


    // Executar runs repetições
    for(k = 0; k < runs; k++)
    {
        // Gerar solução inicial (só é usada por RS)
        gera_sol_inicial(sol, C, m);

        if (alg_tipo == 1) // Recristalização Simulada
        {
            custo = recristalizacao(sol, mat, C, m, tmax, tmin, farref, viz_tipo);
            // Log RS
            log_run_result(LOG_FILE, k + 1, custo, C, m, alg_nome,
                           tmax, tmin, farref, 0.0,
                           0, 0, 0.0, 0.0, 0,
                           viz_tipo, 0, 0.0, 0.0, 0,
                           nome_fich);

        }
        else if (alg_tipo == 2) // Algoritmo Evolutivo
        {
            // O AE recebe 'sol' como ponteiro para guardar a melhor solução final
            custo = algoritmo_evolutivo(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo);
            // Log AE
            log_run_result(LOG_FILE, k + 1, custo, C, m, alg_nome,
                           0.0, 0.0, 0.0, 0.0,
                           tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio,
                           0, mut_tipo, (selecao_tipo==2) ? 1.0 : 0.0, (cross_tipo==2) ? 1.0 : 0.0, 0,
                           nome_fich);
        }

        // Escrever resultado da repetição k
        printf("\n========== Repeticao %d ==========\n", k + 1);
        escreve_sol(sol, C);
        printf("Distancia Media: %.4f\n", custo);

        // Acumular custo para média
        soma_custos += custo;

        // Guardar melhor solução encontrada
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

    // Liberar memória
    free(mat);
    free(sol);
    free(best);

    return 0;
}