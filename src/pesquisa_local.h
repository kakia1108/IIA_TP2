#ifndef IIA_TP2_PESQUISA_LOCAL_H
#define IIA_TP2_PESQUISA_LOCAL_H

// Funções de Vizinhança
void gera_vizinho(int a[], int b[], int C, int m);
void gera_vizinho2(int a[], int b[], int C, int m);

// Algoritmos de Pesquisa Local
double trepa_colinas(int sol[], double *mat, int C, int m, int num_iter);
double recristalizacao(int sol[], double *mat, int C, int m, double tmax, double tmin, double farref, int viz_tipo);

#endif //IIA_TP2_PESQUISA_LOCAL_H