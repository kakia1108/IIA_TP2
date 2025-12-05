#ifndef IIA_TP2_UTILS_H
#define IIA_TP2_UTILS_H

double* init_dados(char *nome, int *C, int *m);
void gera_sol_inicial(int *sol, int C, int m);
void escreve_sol(int *sol, int C);
void substitui(int a[], int b[], int n);
void init_rand();
int random_l_h(int min, int max);
float rand_01();
void log_run_result(char *filepath, int run_number, double custo, int C, int m, double tmax, double farref, int viz_tipo, int append, char *nome_fich);

#endif //IIA_TP2_UTILS_H