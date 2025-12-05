// funcao.c
// Calcula a qualidade de uma solução para o problema de turismo
// Recebe: A solução (array de 0/1), a matriz de distâncias, e o número de candidatos
// Devolve: A distância média entre os m pontos selecionados (MAXIMIZAR)

#include "funcao.h"
#include <stdio.h>
#include <math.h>

double calcula_fit(int sol[], double *mat, int candidatos, int m)
{
    int i, j, count = 0;
    double soma_distancias = 0.0;

    // Contar quantos pontos foram selecionados
    for(i = 0; i < candidatos; i++)
        if(sol[i] == 1)
            count++;

    // Se não foram selecionados exatamente m pontos, penalizar muito
    if(count != m)
    {
        return -1000.0;
    }

    // Calcular soma de todas as distâncias entre pares selecionados
    for(i = 0; i < candidatos; i++)
    {
        if(sol[i] == 1)
        {
            for(j = i + 1; j < candidatos; j++)
            {
                if(sol[j] == 1)
                {
                    // Aceder à matriz de distâncias: mat[i*candidatos + j]
                    double dist = *(mat + i * candidatos + j);
                    soma_distancias += dist;
                }
            }
        }
    }

    // Calcular distância média: DM(P) = 2 / (m * (m-1)) * soma
    if(m <= 1)
        return 0.0;

    double dm = (1.0 / m) * soma_distancias;

    return dm;
}