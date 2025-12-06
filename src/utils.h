#ifndef IIA_TP2_UTILS_H
#define IIA_TP2_UTILS_H

double* init_dados(char *nome, int *C, int *m);
void gera_sol_inicial(int *sol, int C, int m);
void escreve_sol(int *sol, int C);
void substitui(int a[], int b[], int n);
void init_rand();
int random_l_h(int min, int max);
float rand_01();

// Protótipo ATUALIZADO (21 PARÂMETROS): Suporta o log de todos os parâmetros (RS e AE) para o Excel
void log_run_result(char *filepath, int run_number, double custo, int C, int m, char *alg_nome,
                    double tmax, double tmin, double farref, double temperatura_final,
                    int tam_pop, int num_geracoes, double prob_mut, double prob_cross, int tam_torneio,
                    int viz_tipo, int num_mutacoes, double prob_sel_roleta, double prob_cross_uniforme, int num_cross_dois_pontos,
                    char *nome_fich);

#endif //IIA_TP2_UTILS_H