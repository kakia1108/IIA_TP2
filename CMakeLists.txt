cmake_minimum_required(VERSION 4.0)
project(IIA_TP2 C)

set(CMAKE_C_STANDARD 11)

add_executable(IIA_TP2 src/main.c
        src/funcao.c
        src/utils.c
        src/funcao.h
        src/utils.h
        src/pesquisa_local.c
        src/algoritmo_evolutivo.c
        src/pesquisa_local.h
        src/algoritmo_evolutivo.h
        src/algoritmo_hibrido.c
        src/algoritmo_hibrido.h)
