#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo_hibrido.h"
#include "algoritmo_evolutivo.h"
#include "pesquisa_local.h"
#include "utils.h"

// ************************************************************
// ABORDAGEM HÍBRIDA 1: Algoritmo Evolutivo (AE) + Recristalização Simulada (RS) - Sequencial
// ************************************************************
double hibrido_AE_RS(int *solucao_final, double *mat, int C, int m,
                      int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio, int selecao_tipo, int cross_tipo, int mut_tipo,
                      double tmax, double tmin, double farref, int viz_tipo_rs, int aceita_igual)
{
    double custo_ae;
    int *solucao_intermedia;

    // 1. Alocar memória para a solução intermediária (melhor do AE)
    solucao_intermedia = malloc(sizeof(int) * C);
    if (!solucao_intermedia) {
        printf("Erro na alocacao de memoria para solucao intermedia.\n");
        exit(1);
    }

    // 2. Executar o Algoritmo Evolutivo (AE)
    printf("\n[Hibrido 1] Fase 1/2: Algoritmo Evolutivo\n");
    // O AE retorna o melhor custo global e armazena a melhor solução em solucao_intermedia.
    custo_ae = algoritmo_evolutivo(solucao_intermedia, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo);

    // 3. Refinamento com Recristalização Simulada (RS)
    printf("[Hibrido 1] Fase 2/2: Refinamento com Recristalizacao Simulada (RS)\n");
    // A melhor solução do AE é usada como solução inicial para o RS.
    double custo_final = recristalizacao(solucao_intermedia, mat, C, m, tmax, tmin, farref, viz_tipo_rs, aceita_igual);

    // 4. Copiar o resultado (refinado) para a solução final
    substitui(solucao_final, solucao_intermedia, C);

    free(solucao_intermedia);

    // Devolve o custo final (melhorado pelo RS)
    return custo_final;
}

// ************************************************************
// ABORDAGEM HÍBRIDA 2: Algoritmo Evolutivo (AE) + Trepa-Colinas (HC) - Sequencial
// ************************************************************
double hibrido_AE_HC(int *solucao_final, double *mat, int C, int m,
                      int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio, int selecao_tipo, int cross_tipo, int mut_tipo,
                      int num_iter_hc)
{
    double custo_ae;
    int *solucao_intermedia;

    // 1. Alocar memória para a solução intermediária (melhor do AE)
    solucao_intermedia = malloc(sizeof(int) * C);
    if (!solucao_intermedia) {
        printf("Erro na alocacao de memoria para solucao intermedia.\n");
        exit(1);
    }

    // 2. Executar o Algoritmo Evolutivo (AE)
    printf("\n[Hibrido 2] Fase 1/2: Algoritmo Evolutivo\n");
    custo_ae = algoritmo_evolutivo(solucao_intermedia, mat, C, m, tam_pop, num_geracoes, prob_mut, prob_cross, tam_torneio, selecao_tipo, cross_tipo, mut_tipo);

    // 3. Refinamento com Trepa-Colinas (HC)
    printf("[Hibrido 2] Fase 2/2: Refinamento com Trepa-Colinas (HC)\n");
    // A melhor solução do AE é usada como solução inicial para o HC.
    // Usamos o Trepa-Colinas (que já está implementado em pesquisa_local.c) como segunda abordagem de busca local.
    double custo_final = trepa_colinas(solucao_intermedia, mat, C, m, num_iter_hc);

    // 4. Copiar o resultado (refinado) para a solução final
    substitui(solucao_final, solucao_intermedia, C);

    free(solucao_intermedia);

    // Devolve o custo final (melhorado pelo HC)
    return custo_final;
}