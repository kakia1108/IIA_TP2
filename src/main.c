#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pesquisa_local.h"
#include "algoritmo_evolutivo.h"
#include "algoritmo_hibrido.h"
#include "utils.h"
#include "funcao.h"

#define DEFAULT_RUNS 10
#define DADOS_PATH "dados/"
#define LOG_FILE "resultados_otimizacao.csv"

int main(int argc, char *argv[])
{
    // Variáveis que serão alocadas/re-iniciadas a cada execução (necessário no modo interativo)
    char nome_fich[100];
    char filepath[300];
    int C = 0, m = 0, k, runs;
    double *mat = NULL;
    int *sol = NULL, *best = NULL;
    double custo = 0.0, best_custo = 0.0;
    double soma_custos = 0.0;

    // Variáveis de PARÂMETROS (mantidas no escopo principal, pois podem ser re-lidas)
    int alg_tipo = 1;
    double tmax = 10.0;
    double tmin = 0.05;
    double farref = 0.99;
    int viz_tipo = 3;
    int aceita_igual = 0;
    int tam_pop = 50;
    int num_geracoes = 1000;
    double prob_mut = 0.05;
    double prob_cross = 0.8;
    int tam_torneio = 5;
    int selecao_tipo = 1;
    int cross_tipo = 1;
    int mut_tipo = 1;
    int hibrido_tipo = 1;
    int num_iter_hc = 1000;


    // Processamento dos Argumentos (hibrido: tenta CLI, senao usa scanf)
    if(argc > 1)
    {
        // ==========================================================
        // MODO CLI (Processamento em lote - Apenas uma execução)
        // ==========================================================

        // ... [NÃO ALTERADO] O MODO CLI executa apenas uma vez e usa os argumentos da linha de comandos ...

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
        else if (alg_tipo == 3) // Hibrido (AE + RS/HC)
        {
            hibrido_tipo = (argc > 4) ? atoi(argv[4]) : hibrido_tipo;
            tam_pop = (argc > 5) ? atoi(argv[5]) : tam_pop;
            num_geracoes = (argc > 6) ? atoi(argv[6]) : num_geracoes;
            prob_mut = (argc > 7) ? atof(argv[7]) : prob_mut;
            prob_cross = (argc > 8) ? atof(argv[8]) : prob_cross;
            tam_torneio = (argc > 9) ? atoi(argv[9]) : tam_torneio;
            selecao_tipo = (argc > 10) ? atoi(argv[10]) : selecao_tipo;
            cross_tipo = (argc > 11) ? atoi(argv[11]) : cross_tipo;
            mut_tipo = (argc > 12) ? atoi(argv[12]) : mut_tipo;

            if (hibrido_tipo == 1) { // AE + RS
                tmax = (argc > 13) ? atof(argv[13]) : tmax;
                farref = (argc > 14) ? atof(argv[14]) : farref;
                viz_tipo = (argc > 15) ? atoi(argv[15]) : viz_tipo;
                aceita_igual = (argc > 16) ? atoi(argv[16]) : aceita_igual;
            } else { // AE + HC
                num_iter_hc = (argc > 13) ? atoi(argv[13]) : num_iter_hc;
            }
        }

        // CONTINUAÇÃO DO MODO CLI (EXECUÇÃO)
        if(runs <= 0) return 0;

        sprintf(filepath, "%s%s", DADOS_PATH, nome_fich);
        init_rand();
        mat = init_dados(filepath, &C, &m);
        printf("Candidatos: %d, Locais a construir: %d\n", C, m);
        sol = malloc(sizeof(int) * C);
        best = malloc(sizeof(int) * C);
        if(sol == NULL || best == NULL) { printf("Erro na alocacao de memoria\n"); exit(1); }

        char alg_nome[30];
        if (alg_tipo == 1) { strcpy(alg_nome, "RS"); }
        else if (alg_tipo == 2) { strcpy(alg_nome, "AE"); }
        else if (alg_tipo == 3) { (hibrido_tipo == 1) ? strcpy(alg_nome, "Hibrido_AE_RS") : strcpy(alg_nome, "Hibrido_AE_HC"); }
        else { printf("ERRO: Tipo de algoritmo invalido.\n"); return 1; }

        log_run_result(LOG_FILE, 0, 0.0, C, m, alg_nome, tmax, tmin, farref, 0.0, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, viz_tipo, mut_tipo, (selecao_tipo==2) ? 1.0 : 0.0, (cross_tipo==2) ? 1.0 : 0.0, aceita_igual, nome_fich);

        printf("\n--- CONFIGURACAO DE TESTE ---\n");
        printf("Algoritmo: %s\n", alg_nome);
        printf("Instancia: %s, Total de runs: %d\n", nome_fich, runs);
        if (alg_tipo == 1) { printf("RS Params: TMax=%.2f, Farref=%.3f, Vizinhanca=%d, Aceita Iguais=%s\n", tmax, farref, viz_tipo, (aceita_igual==1 ? "Sim" : "Nao")); }
        else if (alg_tipo == 2) { printf("AE Params: Pop=%d, Gens=%d, P_Mut=%.2f, P_Cross=%.2f\n", tam_pop, num_geracoes, prob_mut, prob_cross); printf("AE Ops: Selecao=%s, Crossover=%s, Mutacao=%s, T_Torneio=%d\n", (selecao_tipo == 1 ? "Torneio" : "Roleta"), (cross_tipo == 1 ? "1 Ponto" : "2 Pontos"), (mut_tipo == 1 ? "Troca" : "Bit"), tam_torneio); }
        else { printf("AE Params (Base): Pop=%d, Gens=%d, P_Mut=%.2f, P_Cross=%.2f\n", tam_pop, num_geracoes, prob_mut, prob_cross); if (hibrido_tipo == 1) { printf("RS Params (Refinamento): TMax=%.2f, Farref=%.3f, Vizinhanca=%d, Aceita Iguais=%s\n", tmax, farref, viz_tipo, (aceita_igual==1 ? "Sim" : "Nao")); } else { printf("HC Params (Refinamento): Iteracoes=%d\n", num_iter_hc); } }
        printf("Resultados guardados em: %s\n", LOG_FILE);
        printf("----------------------------\n");

        for(k = 0; k < runs; k++)
        {
            gera_sol_inicial(sol, C, m);
            custo = 0.0;

            if (alg_tipo == 1) { custo = recristalizacao(sol, mat, C, m, tmax, tmin, farref, viz_tipo, aceita_igual); }
            else if (alg_tipo == 2) { custo = algoritmo_evolutivo(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo); }
            else if (alg_tipo == 3) {
                if (hibrido_tipo == 1) { custo = hibrido_AE_RS(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo, tmax, tmin, farref, viz_tipo, aceita_igual); }
                else { custo = hibrido_AE_HC(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo, num_iter_hc); }
            }

            // Log da run (para o ficheiro CSV)
            log_run_result(LOG_FILE, k + 1, custo, C, m, alg_nome,
                       (alg_tipo == 1 || hibrido_tipo == 1) ? tmax : 0.0,
                       (alg_tipo == 1 || hibrido_tipo == 1) ? tmin : 0.0,
                       (alg_tipo == 1 || hibrido_tipo == 1) ? farref : 0.0,
                       (alg_tipo == 3 && hibrido_tipo == 2) ? (double)num_iter_hc : 0.0,
                       (alg_tipo >= 2) ? tam_pop : 0, (alg_tipo >= 2) ? num_geracoes : 0,
                       (alg_tipo >= 2) ? prob_mut : 0.0, (alg_tipo >= 2) ? prob_cross : 0.0,
                       (alg_tipo >= 2) ? tam_torneio : 0,
                       (alg_tipo == 1 || hibrido_tipo == 1) ? viz_tipo : 0,
                       (alg_tipo >= 2) ? mut_tipo : 0, (alg_tipo >= 2 && selecao_tipo==2) ? 1.0 : 0.0,
                       (alg_tipo >= 2 && cross_tipo==2) ? 1.0 : 0.0,
                       (alg_tipo == 1 || hibrido_tipo == 1) ? aceita_igual : 0,
                       nome_fich);

            printf("\n========== Repeticao %d ==========\n", k + 1);
            escreve_sol(sol, C);
            printf("Distancia Media: %.4f\n", custo);
            soma_custos += custo;

            if(k == 0 || custo > best_custo)
            {
                best_custo = custo;
                substitui(best, sol, C);
            }
        }

        printf("\n\n========== RESULTADOS GLOBAIS ==========\n");
        printf("Algoritmo: %s\n", alg_nome);
        printf("Distancia Media (media de %d runs): %.4f\n", runs, soma_custos / runs);
        printf("\nMelhor solucao encontrada:\n");
        escreve_sol(best, C);
        printf("Melhor Distancia Media: %.4f\n", best_custo);

        // Liberar memória (Modo CLI)
        free(mat);
        free(sol);
        free(best);

        return 0; // Fim do MODO CLI
    }
    else
    {
        // ==========================================================
        // NOVO MODO INTERATIVO EM LOOP (Multiplas Execuções no mesmo ficheiro)
        // ==========================================================
        int continuar = 1;

        // --- 1. LEITURA E CARREGAMENTO INICIAL DO FICHEIRO (FORA DO LOOP) ---
        printf("\n-------------------------------------------------\n");
        printf("CONFIGURACAO DE FICHEIRO\n");
        printf("Nome do Ficheiro (ex: tourism_5.txt): ");
        // Usar scanf para ler o nome do ficheiro (assumindo que o utilizador só insere o nome do ficheiro)
        scanf(" %99s", nome_fich);

        sprintf(filepath, "%s%s", DADOS_PATH, nome_fich);
        init_rand();
        mat = init_dados(filepath, &C, &m);
        if(mat == NULL || C == 0 || m == 0) {
            printf("Erro ao ler dados ou ficheiro de dados invalido. A sair.\n");
            return 1;
        }
        printf("Candidatos: %d, Locais a construir: %d\n", C, m);

        // --- 2. ALOCAÇÃO ÚNICA DE MEMÓRIA PARA SOLUÇÕES ---
        sol = malloc(sizeof(int) * C);
        best = malloc(sizeof(int) * C);
        if(sol == NULL || best == NULL)
        {
            printf("Erro na alocacao de memoria\n");
            free(mat);
            exit(1);
        }

        do
        {
            // --- Reset de variáveis de execução para cada novo algoritmo ---
            runs = DEFAULT_RUNS;
            custo = 0.0; best_custo = 0.0; soma_custos = 0.0;

            // --- 3. LEITURA DE PARÂMETROS DO ALGORITMO (Dentro do Loop) ---
            printf("CONFIGURACAO DE ALGORITMO (Ficheiro: %s)\n", nome_fich);

            printf("Tipo de Algoritmo (1=RS, 2=AE, 3=Hibrido) [Default: %d]: ", alg_tipo);
            if (scanf("%d", &alg_tipo) != 1) {
                alg_tipo = 1;
            }

            if (alg_tipo == 3) {
                printf("Tipo de Hibrido (1=AE+RS, 2=AE+HC) [Default: %d]: ", hibrido_tipo);
                if (scanf("%d", &hibrido_tipo) != 1) {
                    hibrido_tipo = 1;
                }
            }

            // --- Configuracao da Execucao e Log ---
            char alg_nome[30];

            if (alg_tipo == 1) { strcpy(alg_nome, "RS"); }
            else if (alg_tipo == 2) { strcpy(alg_nome, "AE"); }
            else if (alg_tipo == 3) { (hibrido_tipo == 1) ? strcpy(alg_nome, "Hibrido_AE_RS") : strcpy(alg_nome, "Hibrido_AE_HC"); }
            else { printf("Tipo de algoritmo invalido. A usar RS.\n"); strcpy(alg_nome, "RS"); alg_tipo = 1; }

            log_run_result(LOG_FILE, 0, 0.0, C, m, alg_nome, tmax, tmin, farref, 0.0, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, viz_tipo, mut_tipo, (selecao_tipo==2) ? 1.0 : 0.0, (cross_tipo==2) ? 1.0 : 0.0, aceita_igual, nome_fich);

            printf("\n--- CONFIGURACAO DE TESTE ---\n");
            printf("Algoritmo: %s\n", alg_nome);
            printf("Instancia: %s, Total de runs: %d\n", nome_fich, runs);
            if (alg_tipo == 1) { printf("RS Params: TMax=%.2f, Farref=%.3f, Vizinhanca=%d, Aceita Iguais=%s\n", tmax, farref, viz_tipo, (aceita_igual==1 ? "Sim" : "Nao")); }
            else if (alg_tipo == 2) { printf("AE Params: Pop=%d, Gens=%d, P_Mut=%.2f, P_Cross=%.2f\n", tam_pop, num_geracoes, prob_mut, prob_cross); printf("AE Ops: Selecao=%s, Crossover=%s, Mutacao=%s, T_Torneio=%d\n", (selecao_tipo == 1 ? "Torneio" : "Roleta"), (cross_tipo == 1 ? "1 Ponto" : "2 Pontos"), (mut_tipo == 1 ? "Troca" : "Bit"), tam_torneio); }
            else { printf("AE Params (Base): Pop=%d, Gens=%d, P_Mut=%.2f, P_Cross=%.2f\n", tam_pop, num_geracoes, prob_mut, prob_cross); if (hibrido_tipo == 1) { printf("RS Params (Refinamento): TMax=%.2f, Farref=%.3f, Vizinhanca=%d, Aceita Iguais=%s\n", tmax, farref, viz_tipo, (aceita_igual==1 ? "Sim" : "Nao")); } else { printf("HC Params (Refinamento): Iteracoes=%d\n", num_iter_hc); } }
            printf("Resultados guardados em: %s\n", LOG_FILE);
            printf("----------------------------\n");

            // 4. EXECUÇÃO
            for(k = 0; k < runs; k++)
            {
                gera_sol_inicial(sol, C, m);
                custo = 0.0;

                if (alg_tipo == 1) { custo = recristalizacao(sol, mat, C, m, tmax, tmin, farref, viz_tipo, aceita_igual); }
                else if (alg_tipo == 2) { custo = algoritmo_evolutivo(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo); }
                else if (alg_tipo == 3) {
                    if (hibrido_tipo == 1) { custo = hibrido_AE_RS(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo, tmax, tmin, farref, viz_tipo, aceita_igual); }
                    else { custo = hibrido_AE_HC(sol, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo, num_iter_hc); }
                }

                // Log da run (para o ficheiro CSV)
                log_run_result(LOG_FILE, k + 1, custo, C, m, alg_nome,
                           (alg_tipo == 1 || hibrido_tipo == 1) ? tmax : 0.0,
                           (alg_tipo == 1 || hibrido_tipo == 1) ? tmin : 0.0,
                           (alg_tipo == 1 || hibrido_tipo == 1) ? farref : 0.0,
                           (alg_tipo == 3 && hibrido_tipo == 2) ? (double)num_iter_hc : 0.0,
                           (alg_tipo >= 2) ? tam_pop : 0, (alg_tipo >= 2) ? num_geracoes : 0,
                           (alg_tipo >= 2) ? prob_mut : 0.0, (alg_tipo >= 2) ? prob_cross : 0.0,
                           (alg_tipo >= 2) ? tam_torneio : 0,
                           (alg_tipo == 1 || hibrido_tipo == 1) ? viz_tipo : 0,
                           (alg_tipo >= 2) ? mut_tipo : 0, (alg_tipo >= 2 && selecao_tipo==2) ? 1.0 : 0.0,
                           (alg_tipo >= 2 && cross_tipo==2) ? 1.0 : 0.0,
                           (alg_tipo == 1 || hibrido_tipo == 1) ? aceita_igual : 0,
                           nome_fich);

                printf("\n========== Repeticao %d ==========\n", k + 1);
                escreve_sol(sol, C);
                printf("Distancia Media: %.4f\n", custo);
                soma_custos += custo;

                if(k == 0 || custo > best_custo)
                {
                    best_custo = custo;
                    substitui(best, sol, C);
                }
            }

            // 5. RESULTADOS GLOBAIS
            printf("\n\n========== RESULTADOS GLOBAIS ==========\n");
            printf("Algoritmo: %s\n", alg_nome);
            printf("Distancia Media (media de %d runs): %.4f\n", runs, soma_custos / runs);
            printf("\nMelhor solucao encontrada:\n");
            escreve_sol(best, C);
            printf("Melhor Distancia Media: %.4f\n", best_custo);


            // 6. PERGUNTA DE CONTINUAÇÃO (Mantém o mesmo ficheiro)
            printf("\n\n-------------------------------------------------\n");
            printf("Deseja executar OUTRO ALGORITMO no ficheiro '%s'? (1=Sim, 0=Nao): ", nome_fich);
            if (scanf("%d", &continuar) != 1) {
                continuar = 0;
            }

        } while (continuar == 1);

        // --- 7. LIBERAR MEMÓRIA (No fim do MODO INTERATIVO) ---
        free(mat);
        free(sol);
        free(best);

        return 0; // Fim do MODO INTERATIVO
    }
}