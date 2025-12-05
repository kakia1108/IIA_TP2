// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "utils.h"

#define DEFAULT_RUNS 10
#define DADOS_PATH "dados/"

int main(int argc, char *argv[])
{
    char nome_fich[100];
    char filepath[300];
    int C, m, k, runs;
    double *mat = NULL;
    int *sol = NULL, *best = NULL;
    double custo, best_custo = 0.0;
    double soma_custos = 0.0;

    // Argumentos da linha de comando
    if(argc == 3)
    {
        runs = atoi(argv[2]);
        strcpy(nome_fich, argv[1]);
    }
    else if(argc == 2)
    {
        runs = DEFAULT_RUNS;
        strcpy(nome_fich, argv[1]);
    }
    else
    {
        runs = DEFAULT_RUNS;
        printf("Nome do Ficheiro: ");
        scanf("%s", nome_fich);
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

    // Executar runs repetições
    for(k = 0; k < runs; k++)
    {
        // Gerar solução inicial aleatória
        gera_sol_inicial(sol, C, m);

        // Aplicar recristalização simulada
        custo = recristalização(sol, mat, C, m);
        // Alternativa: trepa_colinas
        // custo = trepa_colinas(sol, mat, C, m, 1000);

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