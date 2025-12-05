#ifndef IIA_TP2_ALGORITMO_H
#define IIA_TP2_ALGORITMO_H

// Funções de Vizinhança
void gera_vizinho(int a[], int b[], int C, int m);
void gera_vizinho2(int a[], int b[], int C, int m);

// Algoritmos de Pesquisa Local
double trepa_colinas(int sol[], double *mat, int C, int m, int num_iter);

// Recristalização Simulada com novos parâmetros configuráveis
// viz_tipo: 1 -> Vizinhança 1, 2 -> Vizinhança 2, 3 -> Alternar V1 e V2
double recristalizacao(int sol[], double *mat, int C, int m, double tmax, double tmin, double farref, int viz_tipo);

// ************************************************************
// NOVAS ESTRUTURAS E PROTÓTIPOS PARA ALGORITMO EVOLUTIVO (AE)
// ************************************************************

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

// Funções de Gestão de População
Populacao cria_populacao(int tam_pop, int C, int m);
void liberta_populacao(Populacao *p);
// Usa um ponteiro para a função de avaliação (que é calcula_fit)
void avalia_populacao(Populacao *p, double *mat, double (*funcao_avaliacao)(int[], double*, int, int));
void melhor_solucao_populacao(Populacao *p, int *best_sol);


#endif //IIA_TP2_ALGORITMO_H