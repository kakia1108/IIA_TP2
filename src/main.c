#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"

#define DEFAULT_RUNS 10
#define DADOS_PATH "dados/"
#define LOG_FILE "resultados_rs.csv"

int main(int argc, char *argv[])
{
    char nome_fich[100];
    char filepath[300];
    int C, m, k, runs;
    double *mat = NULL;
    int *sol = NULL, *best = NULL;
    double custo, best_custo = 0.0;
    double soma_custos = 0.0;

    // ************************************************************
    // PARÂMETROS DA RECRISTALIZAÇÃO SIMULADA (Valores default para o estudo)
    // ************************************************************
    double tmax = 10.0;
    double tmin = 0.05; // Mínimo fixo
    double farref = 0.99;
    int viz_tipo = 3; // Default: 3 (Alternar V1 e V2)
    // ************************************************************

    // Processamento dos Argumentos (híbrido: tenta CLI, senão usa scanf)
    if(argc > 1)
    {
        // 1. Ficheiro (Obrigatório)
        strcpy(nome_fich, argv[1]);

        // 2. Runs
        runs = (argc > 2) ? atoi(argv[2]) : DEFAULT_RUNS;

        // 3. Tmax
        tmax = (argc > 3) ? atof(argv[3]) : tmax;

        // 4. Fator de Arrefecimento
        farref = (argc > 4) ? atof(argv[4]) : farref;

        // 5. Tipo de Vizinhança
        viz_tipo = (argc > 5) ? atoi(argv[5]) : viz_tipo;
    }
    else
    {
        // MODO INTERATIVO (Restaurado)
        runs = DEFAULT_RUNS; // Valor default para runs
        printf("Nome do Ficheiro (ex: tourism_5.txt): ");
        if (scanf("%s", nome_fich) != 1) {
            printf("Erro ao ler o nome do ficheiro.\n");
            return 1;
        }
        // NOTA: Se o modo interativo for usado, os parâmetros da RS permanecem nos valores default.
    }

    if(runs <= 0)
        return 0;

    // Construir caminho completo: dados/nome_ficheiro.txt
    sprintf(filepath, "%s%s", DADOS_PATH, nome_fich);

    init_rand();

    // Ler dados do ficheiro (C candidatos, m locais a construir)
    mat = init_dados(filepath, &C, &m);

    // Alocação de memória para soluções
    sol = malloc(sizeof(int) * C);
    best = malloc(sizeof(int) * C);
    if(sol == NULL || best == NULL)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    // LOG: Limpar o ficheiro de log e escrever o cabeçalho (apenas na primeira run)
    log_run_result(LOG_FILE, 0, 0.0, C, m, tmax, farref, viz_tipo, 0, nome_fich);

    printf("\n--- CONFIGURAÇÃO DE TESTE ---\n");
    printf("Algoritmo: Recristalização Simulada\n");
    printf("TMax: %.2f, TMin: %.2f, Farref: %.3f, Vizinhança: %d\n", tmax, tmin, farref, viz_tipo);
    printf("Instancia: %s, Total de runs: %d\n", nome_fich, runs);
    printf("Resultados guardados em: %s\n", LOG_FILE);
    printf("----------------------------\n");


    // Executar runs repetições
    for(k = 0; k < runs; k++)
    {
        // Gerar solução inicial aleatória
        gera_sol_inicial(sol, C, m);

        // Aplicar recristalização simulada com todos os parâmetros
        custo = recristalizacao(sol, mat, C, m, tmax, tmin, farref, viz_tipo);

        // Escrever resultado da repetição k
        printf("\n========== Repeticao %d ==========\n", k + 1);
        escreve_sol(sol, C);
        printf("Distancia Media: %.4f\n", custo);

        // LOG: Adicionar o resultado ao ficheiro CSV
        log_run_result(LOG_FILE, k + 1, custo, C, m, tmax, farref, viz_tipo, 1, nome_fich);

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