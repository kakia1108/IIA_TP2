// pesquisa_local.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pesquisa_local.h"
#include "funcao.h"
#include "utils.h"

// VIZINHANÇA 1: Trocar um local selecionado por um não selecionado
void gera_vizinho(int a[], int b[], int C, int m)
{
    int i, p1, p2;

    for(i = 0; i < C; i++)
        b[i] = a[i];

    do
        p1 = random_l_h(0, C - 1);
    while(b[p1] != 0);

    do
        p2 = random_l_h(0, C - 1);
    while(b[p2] != 1);

    b[p1] = 1;
    b[p2] = 0;
}

// VIZINHANÇA 2: Trocar dois pares de locais (remover 2, adicionar 2)
void gera_vizinho2(int a[], int b[], int C, int m)
{
    int i, p1, p2, p3, p4;

    for(i = 0; i < C; i++)
        b[i] = a[i];

    int *zeros = malloc(C * sizeof(int));
    int *uns = malloc(C * sizeof(int));
    int count_zeros = 0;
    int count_uns = 0;

    for (i = 0; i < C; i++) {
        if (b[i] == 0) {
            zeros[count_zeros++] = i;
        } else {
            uns[count_uns++] = i;
        }
    }

    if (count_zeros < 2 || count_uns < 2) {
        free(zeros);
        free(uns);
        gera_vizinho(a, b, C, m);
        return;
    }

    int idx_p1_zeros = random_l_h(0, count_zeros - 1);
    int idx_p3_zeros = random_l_h(0, count_zeros - 1);
    while (idx_p1_zeros == idx_p3_zeros) {
        idx_p3_zeros = random_l_h(0, count_zeros - 1);
    }
    p1 = zeros[idx_p1_zeros];
    p3 = zeros[idx_p3_zeros];

    int idx_p2_uns = random_l_h(0, count_uns - 1);
    int idx_p4_uns = random_l_h(0, count_uns - 1);
    while (idx_p2_uns == idx_p4_uns) {
        idx_p4_uns = random_l_h(0, count_uns - 1);
    }
    p2 = uns[idx_p2_uns];
    p4 = uns[idx_p4_uns];

    free(zeros);
    free(uns);

    b[p1] = 1; b[p2] = 0;
    b[p3] = 1; b[p4] = 0;
}

// Trepa-colinas com First-Choice (MAXIMIZAÇÃO)
double trepa_colinas(int sol[], double *mat, int C, int m, int num_iter)
{
    int *nova_sol, i;
    double custo, custo_viz;

    nova_sol = malloc(sizeof(int) * C);
    if(nova_sol == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    custo = calcula_fit(sol, mat, C, m);

    for(i = 0; i < num_iter; i++)
    {
        gera_vizinho(sol, nova_sol, C, m);
        custo_viz = calcula_fit(nova_sol, mat, C, m);

        if(custo_viz > custo)
        {
            substitui(sol, nova_sol, C);
            custo = custo_viz;
        }
        // Nota: O Trepa-Colinas puro não costuma aceitar igualdade, mas pode ser configurado.
        // Como o enunciado não obriga a testar o Trepa-Colinas puro, mantemos a versão simples.
    }

    free(nova_sol);
    return custo;
}

// Recristalização Simulada (MAXIMIZAÇÃO)
double recristalizacao(int sol[], double *mat, int C, int m, double tmax, double tmin, double farref, int viz_tipo, int aceita_igual)
{
    int *nova_sol, i, k = 5, itera = 0;
    double custo, custo_viz, t;
    double temperatura_final = 0.0;

    nova_sol = malloc(sizeof(int) * C);
    if(nova_sol == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    t = tmax;
    custo = calcula_fit(sol, mat, C, m);

    while(t > tmin)
    {
        for(i = 0; i < k; i++)
        {
            if(viz_tipo == 1)
                gera_vizinho(sol, nova_sol, C, m);
            else if(viz_tipo == 2)
                gera_vizinho2(sol, nova_sol, C, m);
            else
            {
                if(i % 2 == 0)
                    gera_vizinho(sol, nova_sol, C, m);
                else
                    gera_vizinho2(sol, nova_sol, C, m);
            }

            custo_viz = calcula_fit(nova_sol, mat, C, m);

            // Lógica de aceitação:
            if(custo_viz > custo)
            {
                // 1. MELHOROU: Aceita sempre
                substitui(sol, nova_sol, C);
                custo = custo_viz;
            }
            else if (custo_viz == custo && aceita_igual == 1)
            {
                // 2. IGUAL: Aceita se o parâmetro 'aceita_igual' for 1
                substitui(sol, nova_sol, C);
                custo = custo_viz;
            }
            else
            {
                // 3. PIOROU: Aceita com probabilidade baseada em temperatura
                if (rand_01() < exp((custo_viz - custo) / t))
                {
                    substitui(sol, nova_sol, C);
                    custo = custo_viz;
                }
            }
        }

        t = t * farref;
        itera++;
    }

    // Apenas para fins de log (opcional, mas útil para o estudo)
    temperatura_final = t;

    // Devolve o melhor custo e a temperatura final no ponteiro
    return custo;
}