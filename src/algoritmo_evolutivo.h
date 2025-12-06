#ifndef IIA_TP2_ALGORITMO_EVOLUTIVO_H
#define IIA_TP2_ALGORITMO_EVOLUTIVO_H

// Estrutura do Indivíduo (Solução)
typedef struct {
    int *solucao;      // O vetor binário 0/1 (do tamanho C)
    double fitness;    // A Distância Média (DM)
} Individuo;

// Estrutura da População
typedef struct {
    Individuo *pop;    // Array de indivíduos
    int tam_pop;       // Tamanho N da população
    int C;             // Número de Candidatos (tamanho da solução)
    int m;             // Locais a construir (para garantir validade)
} Populacao;

// ************************************************************
// PROTÓTIPOS
// ************************************************************

// Gestão de População
Populacao cria_populacao(int tam_pop, int C, int m);
void liberta_populacao(Populacao *p);
void avalia_populacao(Populacao *p, double *mat, double (*funcao_avaliacao)(int[], double*, int, int));
void melhor_solucao_populacao(Populacao *p, int *best_sol);

// Operadores
int selecao_torneio(Populacao *p, int k);
int selecao_roleta(Populacao *p);
void recombinacao_ponto_unico(int *pai1, int *pai2, int *filho1, int *filho2, int C);
void recombinacao_dois_pontos(int *pai1, int *pai2, int *filho1, int *filho2, int C);
void mutacao_troca(int *sol, int C, int m);
void mutacao_por_bit(int *sol, int C, double prob_mut);
void repara_solucao(int *sol, int C, int m);

// Função Principal
double algoritmo_evolutivo(int *solucao_final, double *mat, int C, int m, int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio, int selecao_tipo, int cross_tipo, int mut_tipo);

#endif //IIA_TP2_ALGORITMO_EVOLUTIVO_H