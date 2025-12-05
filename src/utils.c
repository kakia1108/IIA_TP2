#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

// Leitura do ficheiro de input (formato turismo)
// Recebe: nome do ficheiro, número de candidatos C (ptr), número de locais m (ptr)
// Devolve: matriz de distâncias (double)
double* init_dados(char *nome, int *C, int *m)
{
    FILE *f;
    double *p, *q;
    int i, j;

    f = fopen(nome, "r");
    if(!f)
    {
        printf("Erro no acesso ao ficheiro dos dados\n");
        exit(1);
    }

    // Ler C (número de candidatos) e m (número de locais a construir)
    fscanf(f, "%d %d", C, m);

    printf("Candidatos: %d, Locais a construir: %d\n", *C, *m);

    // Alocação dinâmica da matriz de distâncias
    p = malloc(sizeof(double) * (*C) * (*C));
    if(!p)
    {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    q = p;

    // Preenchimento da matriz de distâncias
    for(i = 0; i < *C; i++)
        for(j = 0; j < *C; j++)
            fscanf(f, "%lf", q++);

    fclose(f);
    return p;
}

// Gera a solução inicial com exatamente m locais selecionados
// Parametros: solução, número de candidatos, número de locais a construir
void gera_sol_inicial(int *sol, int C, int m)
{
    int i, x;

    // Inicializar todos com 0 (não selecionados)
    for(i = 0; i < C; i++)
        sol[i] = 0;
    
    // Selecionar m locais aleatoriamente
    for(i = 0; i < m; i++)
    {
        do
            x = random_l_h(0, C - 1);
        while(sol[x] != 0);  // Se já foi selecionado, tenta outro
        sol[x] = 1;
    }
}

// Escreve solução
// Parametros: solução, número de candidatos
void escreve_sol(int *sol, int C)
{
    int i, count = 0;

    printf("\nLocais selecionados: ");
    for(i = 0; i < C; i++)
    {
        if(sol[i] == 1)
        {
            printf("%d  ", i + 1);  // Mostrar de 1 a C (mais intuitivo)
            count++;
        }
    }
    printf("\n(Total: %d locais)\n", count);
}

// Copia vector b para a (tamanho n)
void substitui(int a[], int b[], int n)
{
    int i;
    for(i = 0; i < n; i++)
        a[i] = b[i];
}

// Inicializa o gerador de números aleatórios
void init_rand()
{
    srand((unsigned)time(NULL));
}

// Devolve valor inteiro aleatório entre min e max (inclusive)
int random_l_h(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Devolve um valor real aleatório do intervalo [0, 1]
float rand_01()
{
    return ((float)rand()) / RAND_MAX;
}