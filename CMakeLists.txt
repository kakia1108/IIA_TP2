cmake_minimum_required(VERSION 4.0)
project(IIA_TP2 C)

set(CMAKE_C_STANDARD 11)

add_executable(IIA_TP2 src/main.c
        src/funcao.c
        src/algoritmo.c
        src/utils.c
        src/algoritmo.h
        src/funcao.h
        src/utils.h)
