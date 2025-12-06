#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "algoritmo_evolutivo.h"
#include "funcao.h"
#include "utils.h"

// ************************************************************
// FUNÇÕES DE GESTÃO DE POPULAÇÃO
// ************************************************************

// Cria a população, alocando memória e gerando soluções iniciais aleatórias
Populacao cria_populacao(int tam_pop, int C, int m)
{
    Populacao p;
    int i;

    p.tam_pop = tam_pop;
    p.C = C;
    p.m = m;
    
    p.pop = malloc(sizeof(Individuo) * tam_pop);
    if (!p.pop) {
        printf("Erro na alocacao da populacao\n");
        exit(1);
    }

    for (i = 0; i < tam_pop; i++)
    {
        p.pop[i].solucao = malloc(sizeof(int) * C);
        if (!p.pop[i].solucao) {
            printf("Erro na alocacao da solucao do individuo %d\n", i);
            exit(1);
        }
        
        gera_sol_inicial(p.pop[i].solucao, C, m);
        p.pop[i].fitness = 0.0;
    }
    return p;
}

// Liberta a memória alocada para a população
void liberta_populacao(Populacao *p)
{
    int i;
    for (i = 0; i < p->tam_pop; i++)
    {
        free(p->pop[i].solucao);
    }
    free(p->pop);
}

// Avalia todos os indivíduos da população
void avalia_populacao(Populacao *p, double *mat, double (*funcao_avaliacao)(int[], double*, int, int))
{
    int i;
    for (i = 0; i < p->tam_pop; i++)
    {
        p->pop[i].fitness = funcao_avaliacao(p->pop[i].solucao, mat, p->C, p->m);
    }
}

// Encontra o melhor indivíduo da população e retorna a sua solução
void melhor_solucao_populacao(Populacao *p, int *best_sol)
{
    int i, melhor_idx = 0;
    double melhor_custo = p->pop[0].fitness;

    for (i = 1; i < p->tam_pop; i++)
    {
        if (p->pop[i].fitness > melhor_custo)
        {
            melhor_custo = p->pop[i].fitness;
            melhor_idx = i;
        }
    }
    substitui(best_sol, p->pop[melhor_idx].solucao, p->C);
}

// ************************************************************
// OPERADORES
// ************************************************************

// Seleção por Torneio
int selecao_torneio(Populacao *p, int k)
{
    int i, indice_candidato, indice_melhor = -1;
    double melhor_fitness = -1.0; 

    if (k <= 0 || k > p->tam_pop)
        k = p->tam_pop;

    for (i = 0; i < k; i++)
    {
        indice_candidato = random_l_h(0, p->tam_pop - 1);

        if (indice_melhor == -1 || p->pop[indice_candidato].fitness > melhor_fitness)
        {
            melhor_fitness = p->pop[indice_candidato].fitness;
            indice_melhor = indice_candidato;
        }
    }
    return indice_melhor; 
}

// Seleção por Roleta (Roulette Wheel Selection)
int selecao_roleta(Populacao *p)
{
    int i;
    double soma_fitness = 0.0;
    double ponto_aleatorio;

    for (i = 0; i < p->tam_pop; i++)
    {
        if (p->pop[i].fitness > 0.0)
            soma_fitness += p->pop[i].fitness;
    }
    
    if (soma_fitness <= 0.0) {
        return random_l_h(0, p->tam_pop - 1);
    }

    ponto_aleatorio = rand_01() * soma_fitness; 

    double soma_acumulada = 0.0;
    for (i = 0; i < p->tam_pop; i++)
    {
        if (p->pop[i].fitness > 0.0) { 
            soma_acumulada += p->pop[i].fitness;
        }
        
        if (soma_acumulada >= ponto_aleatorio)
        {
            return i;
        }
    }
    
    return p->tam_pop - 1;
}

// Recombinação (Crossover) de Ponto Único
void recombinacao_ponto_unico(int *pai1, int *pai2, int *filho1, int *filho2, int C)
{
    int ponto_corte, i;
    ponto_corte = random_l_h(1, C - 1);
    
    for (i = 0; i < C; i++)
    {
        if (i < ponto_corte)
        {
            filho1[i] = pai1[i];
            filho2[i] = pai2[i];
        }
        else
        {
            filho1[i] = pai2[i];
            filho2[i] = pai1[i];
        }
    }
}

// Recombinação (Crossover) de Dois Pontos
void recombinacao_dois_pontos(int *pai1, int *pai2, int *filho1, int *filho2, int C)
{
    int ponto_corte1, ponto_corte2, i;

    do {
        ponto_corte1 = random_l_h(1, C - 2);
        ponto_corte2 = random_l_h(1, C - 2);
    } while (ponto_corte1 == ponto_corte2);

    if (ponto_corte1 > ponto_corte2) {
        int temp = ponto_corte1;
        ponto_corte1 = ponto_corte2;
        ponto_corte2 = temp;
    }
    
    for (i = 0; i < C; i++)
    {
        if (i < ponto_corte1 || i >= ponto_corte2)
        {
            filho1[i] = pai1[i];
            filho2[i] = pai2[i];
        }
        else
        {
            filho1[i] = pai2[i];
            filho2[i] = pai1[i];
        }
    }
}

// Mutação de Troca (Swap Mutation)
void mutacao_troca(int *sol, int C, int m)
{
    int p1, p2;
    
    do
        p1 = random_l_h(0, C - 1);
    while(sol[p1] != 0);

    do
        p2 = random_l_h(0, C - 1);
    while(sol[p2] != 1);

    sol[p1] = 1; 
    sol[p2] = 0; 
}

// Mutação por Bit (Flip Mutation)
void mutacao_por_bit(int *sol, int C, double prob_mut)
{
    for (int i = 0; i < C; i++) {
        if (rand_01() < prob_mut) {
            sol[i] = 1 - sol[i]; 
        }
    }
}

// Função de Reparação
void repara_solucao(int *sol, int C, int m)
{
    int i, count = 0;
    int p;
    
    for(i = 0; i < C; i++)
        if(sol[i] == 1)
            count++;

    while (count > m)
    {
        do
            p = random_l_h(0, C - 1);
        while(sol[p] != 1);
        
        sol[p] = 0; 
        count--;
    }

    while (count < m)
    {
        do
            p = random_l_h(0, C - 1);
        while(sol[p] != 0);
        
        sol[p] = 1; 
        count++;
    }
}

// ************************************************************
// ALGORITMO EVOLUTIVO PRINCIPAL
// ************************************************************
double algoritmo_evolutivo(int *solucao_final, double *mat, int C, int m, int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio, int selecao_tipo, int cross_tipo, int mut_tipo)
{
    Populacao pop_atual, pop_nova;
    int i, p1_idx, p2_idx;
    int *filho1_sol, *filho2_sol;
    double melhor_custo_geral = -1.0;
    
    filho1_sol = malloc(sizeof(int) * C);
    filho2_sol = malloc(sizeof(int) * C);
    if (!filho1_sol || !filho2_sol) {
        printf("Erro na alocacao de memoria para filhos.\n");
        exit(1);
    }
    
    // 1. Inicialização
    pop_atual = cria_populacao(tam_pop, C, m);
    pop_nova = cria_populacao(tam_pop, C, m);

    // 2. Avaliação Inicial
    avalia_populacao(&pop_atual, mat, calcula_fit);
    
    for (i = 0; i < tam_pop; i++) {
        if (pop_atual.pop[i].fitness > melhor_custo_geral) {
            melhor_custo_geral = pop_atual.pop[i].fitness;
        }
    }
    
    // 3. Loop de Gerações
    for (int geracao = 0; geracao < num_geracoes; geracao++)
    {
        for (i = 0; i < tam_pop; i += 2) 
        {
            // A. SELEÇÃO
            if (selecao_tipo == 1) { // 1: Torneio
                p1_idx = selecao_torneio(&pop_atual, tam_torneio);
                p2_idx = selecao_torneio(&pop_atual, tam_torneio);
            } else { // 2: Roleta
                p1_idx = selecao_roleta(&pop_atual);
                p2_idx = selecao_roleta(&pop_atual);
            }

            substitui(filho1_sol, pop_atual.pop[p1_idx].solucao, C);
            substitui(filho2_sol, pop_atual.pop[p2_idx].solucao, C);
            
            // B. RECOMBINAÇÃO (Crossover)
            if (rand_01() < prob_cross)
            {
                if (cross_tipo == 1) { // 1: Ponto Único
                    recombinacao_ponto_unico(pop_atual.pop[p1_idx].solucao, 
                                            pop_atual.pop[p2_idx].solucao, 
                                            filho1_sol, filho2_sol, C);
                } else { // 2: Dois Pontos
                    recombinacao_dois_pontos(pop_atual.pop[p1_idx].solucao, 
                                            pop_atual.pop[p2_idx].solucao, 
                                            filho1_sol, filho2_sol, C);
                }
                
                // C. REPARAÇÃO (Após Crossover)
                repara_solucao(filho1_sol, C, m);
                repara_solucao(filho2_sol, C, m);
            }
            
            // D. MUTAÇÃO
            if (mut_tipo == 1) { // 1: Mutação de Troca
                if (rand_01() < prob_mut) mutacao_troca(filho1_sol, C, m);
                if (rand_01() < prob_mut) mutacao_troca(filho2_sol, C, m);
            } else { // 2: Mutação por Bit
                mutacao_por_bit(filho1_sol, C, prob_mut);
                mutacao_por_bit(filho2_sol, C, prob_mut);

                // **REPARAÇÃO OBRIGATÓRIA APÓS MUTAÇÃO POR BIT**
                repara_solucao(filho1_sol, C, m);
                repara_solucao(filho2_sol, C, m);
            }

            // E. SUBSTITUIÇÃO
            substitui(pop_nova.pop[i].solucao, filho1_sol, C);
            if (i + 1 < tam_pop) {
                substitui(pop_nova.pop[i+1].solucao, filho2_sol, C);
            }
        }
        
        // F. Avaliar a Nova População
        avalia_populacao(&pop_nova, mat, calcula_fit);
        
        // G. Elite
        int melhor_idx_atual = selecao_torneio(&pop_atual, pop_atual.tam_pop);
        int melhor_idx_nova = selecao_torneio(&pop_nova, pop_nova.tam_pop);
        
        if (pop_atual.pop[melhor_idx_atual].fitness > pop_nova.pop[melhor_idx_nova].fitness) {
            int pior_idx_nova = 0;
            double pior_custo = pop_nova.pop[0].fitness;
            for(i = 1; i < tam_pop; i++) {
                if (pop_nova.pop[i].fitness < pior_custo) {
                    pior_custo = pop_nova.pop[i].fitness;
                    pior_idx_nova = i;
                }
            }
            substitui(pop_nova.pop[pior_idx_nova].solucao, pop_atual.pop[melhor_idx_atual].solucao, C);
            pop_nova.pop[pior_idx_nova].fitness = pop_atual.pop[melhor_idx_atual].fitness;
            melhor_custo_geral = pop_atual.pop[melhor_idx_atual].fitness;
        } else {
            melhor_custo_geral = pop_nova.pop[melhor_idx_nova].fitness;
        }

        // H. Substituir gerações (troca de ponteiros)
        Populacao temp = pop_atual;
        pop_atual = pop_nova;
        pop_nova = temp; 
    }
    
    // 4. Finalização
    melhor_solucao_populacao(&pop_atual, solucao_final);

    liberta_populacao(&pop_atual);
    liberta_populacao(&pop_nova); 
    free(filho1_sol);
    free(filho2_sol);

    return melhor_custo_geral;
}