// algoritmo.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

// VIZINHANÇA 1: Trocar um local selecionado por um não selecionado
// Garante que a solução mantém exatamente m locais
void gera_vizinho(int a[], int b[], int C, int m)
{
    int i, p1, p2;

    // Copiar solução atual
    for(i = 0; i < C; i++)
        b[i] = a[i];

    // Encontrar uma posição com valor 0 (não selecionado)
    do
        p1 = random_l_h(0, C - 1);
    while(b[p1] != 0);

    // Encontrar uma posição com valor 1 (selecionado)
    do
        p2 = random_l_h(0, C - 1);
    while(b[p2] != 1);

    // Trocar: remover p2, adicionar p1
    b[p1] = 1;
    b[p2] = 0;
}

// VIZINHANÇA 2: Trocar dois pares de locais (remover 2, adicionar 2)
// Mais agressiva que a vizinhança 1
void gera_vizinho2(int a[], int b[], int C, int m)
{
    int i, p1, p2, p3, p4;

    // Copiar solução atual
    for(i = 0; i < C; i++)
        b[i] = a[i];

    // Encontrar primeira posição com valor 0
    do
        p1 = random_l_h(0, C - 1);
    while(b[p1] != 0);

    // Encontrar primeira posição com valor 1
    do
        p2 = random_l_h(0, C - 1);
    while(b[p2] != 1);

    // Encontrar segunda posição com valor 0 (diferente de p1)
    do
        p3 = random_l_h(0, C - 1);
    while(b[p3] != 0 || p1 == p3);

    // Encontrar segunda posição com valor 1 (diferente de p2)
    do
        p4 = random_l_h(0, C - 1);
    while(b[p4] != 1 || p4 == p2);

    // Fazer duas trocas
    b[p1] = 1;
    b[p2] = 0;
    b[p3] = 1;
    b[p4] = 0;
}

// Trepa-colinas com First-Choice
// Problema de MAXIMIZAÇÃO
// Aceita vizinhos que melhoram a solução
double trepa_colinas(int sol[], double *mat, int C, int m, int num_iter)
{
    int *nova_sol, i;
    double custo, custo_viz;

    nova_sol = malloc(sizeof(int) * C);
    if(nova_sol == NULL)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    // Avaliar solução inicial
    custo = calcula_fit(sol, mat, C, m);

    for(i = 0; i < num_iter; i++)
    {
        // Gerar vizinho com vizinhança 1
        gera_vizinho(sol, nova_sol, C, m);

        // Avaliar vizinho
        custo_viz = calcula_fit(nova_sol, mat, C, m);

        // Aceitar vizinho se MELHORAR (maximização)
        if(custo_viz > custo)
        {
            substitui(sol, nova_sol, C);
            custo = custo_viz;
        }
    }

    free(nova_sol);
    return custo;
}

// Recristalização Simulada adaptada ao problema de turismo
// Problema de MAXIMIZAÇÃO
// Recristalização Simulada adaptada ao problema de turismo
// Problema de MAXIMIZAÇÃO
double recristalizacao(int sol[], double *mat, int C, int m, double tmax, double tmin, double farref, int viz_tipo)
{
    int *nova_sol, i, k = 5, itera = 0;
    double custo, custo_viz, t;

    nova_sol = malloc(sizeof(int) * C);
    if(nova_sol == NULL)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    t = tmax;
    custo = calcula_fit(sol, mat, C, m);

    while(t > tmin)
    {
        for(i = 0; i < k; i++)
        {
            // Escolher vizinhança com base no parâmetro
            if(viz_tipo == 1)
                gera_vizinho(sol, nova_sol, C, m);
            else if(viz_tipo == 2)
                gera_vizinho2(sol, nova_sol, C, m);
            else // viz_tipo == 3 (Alternar)
            {
                if(i % 2 == 0)
                    gera_vizinho(sol, nova_sol, C, m);
                else
                    gera_vizinho2(sol, nova_sol, C, m);
            }

            // Avaliar vizinho
            custo_viz = calcula_fit(nova_sol, mat, C, m);

            // Aceitar vizinho se MELHORAR (maximização)
            if(custo_viz > custo)
            {
                substitui(sol, nova_sol, C);
                custo = custo_viz;
            }
            // Ou aceitar com probabilidade baseada em temperatura
            else if(rand_01() < exp((custo_viz - custo) / t))
            {
                substitui(sol, nova_sol, C);
                custo = custo_viz;
            }
        }

        t = t * farref;
        itera++;
    }

    printf("Iteracoes: %d\n", itera);
    free(nova_sol);
    return custo;
}