#ifndef IIA_TP2_ALGORITMO_HIBRIDO_H
#define IIA_TP2_ALGORITMO_HIBRIDO_H

// ************************************************************
// PROTÓTIPOS
// ************************************************************

// ABORDAGEM HÍBRIDA 1: Algoritmo Evolutivo (AE) seguido de Recristalização Simulada (RS)
// A melhor solução do AE é refinada pelo RS.
double hibrido_AE_RS(int *solucao_final, double *mat, int C, int m,
                      int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio, int selecao_tipo, int cross_tipo, int mut_tipo,
                      double tmax, double tmin, double farref, int viz_tipo_rs, int aceita_igual);

// ABORDAGEM HÍBRIDA 2: Algoritmo Evolutivo (AE) seguido de Trepa-Colinas (HC)
// A melhor solução do AE é refinada pelo Trepa-Colinas (HC) com um número fixo de iterações.
double hibrido_AE_HC(int *solucao_final, double *mat, int C, int m,
                      int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio, int selecao_tipo, int cross_tipo, int mut_tipo,
                      int num_iter_hc);

#endif //IIA_TP2_ALGORITMO_HIBRIDO_H