#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#include "mandelbrot.h"

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Uso: mandelbrot <largura> <altura> <max_iteracoes> <num_threads>\n");
        return 1;
    }

    long largura, altura, max_iteracoes, num_threads;

    if (validaInt(argv[1], &largura) != 0) return 1;
    if (validaInt(argv[2], &altura) != 0) return 1;
    if (validaInt(argv[3], &max_iteracoes) != 0) return 1;
    if (validaInt(argv[4], &num_threads) != 0) return 1;

    unsigned char *buffer = (unsigned char*)malloc(largura * altura * sizeof(unsigned char));
    if (buffer == NULL) {
        fprintf(stderr, "Erro: alocação de memória falhou\n");
        return 1;
    }

    struct timespec ini, fim;
    clock_gettime(CLOCK_MONOTONIC, &ini);
    calcularSerial(buffer, largura, altura, max_iteracoes);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo_serial = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;

    if (escreverPGM("mandelbrot_gnfr_serial.pgm", buffer, largura, altura) != 0) {
        free(buffer);
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &ini);
    calcularOpenMP(buffer, largura, altura, max_iteracoes, num_threads);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo_openmp = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;

    if (escreverPGM("mandelbrot_gnfr_openmp.pgm", buffer, largura, altura) != 0) {
        free(buffer);
        return 1;
    }

    FILE *t = fopen("times.txt", "w");
    if (t == NULL) {
        fprintf(stderr, "Erro: falha ao criar arquivo de saida times.txt\n");
        free(buffer);
        return 1;
    }

    fprintf(t, "serial %.9f\nopenmp %.9f\n", tempo_serial, tempo_openmp);

    if (fclose(t) != 0) {
        fprintf(stderr, "Erro: falha ao fechar times.txt\n");
        free(buffer);
        return 1;
    }


    free(buffer);

    return 0;
}

int validaInt(const char *s, long *saida) {
    errno = 0;
    char *fim;
    long valor = strtol(s, &fim, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Erro: um ou mais dos argumentos é grande demais\n");
        return 1;
    }
    if (fim == s) {
        fprintf(stderr, "Erro: um ou mais argumentos não são números inteiros positivos\n");
        return 1;
    }
    if (*fim != '\0') {
        fprintf(stderr, "Erro: um ou mais argumentos não são números inteiros positivos\n");
        return 1;
    }
    if (valor <= 0) {
        fprintf(stderr, "Erro: um ou mais argumentos não são números inteiros positivos\n");
        return 1;
    }

    *saida = valor;
    return 0;
}

void mapearPixel(int col, int row, long largura, long altura, double *cr, double *ci) {
    double passo_x = 3.0 / (largura > 1 ? largura - 1 : 1);
    double passo_y = 3.0 / (altura > 1 ? altura - 1 : 1);

    *cr = -2.0 + col * passo_x;
    *ci = 1.5 - row * passo_y;
}

int contarIteracoes(double cr, double ci, long max_iteracoes) {
    double a = 0.0, b = 0.0;
    long iter = 0;

    while (a * a + b * b <= 4.0 && iter < max_iteracoes) {
        double a_novo = a * a - b * b + cr;
        double b_novo = 2.0 * a * b + ci;

        a = a_novo;
        b = b_novo;
        iter++;
    }
    return (int)iter;
}

int escreverPGM(const char *nome, const unsigned char *buffer, long largura, long altura) {
    FILE *f = fopen(nome, "w");
    if (f == NULL) {
        fprintf(stderr, "Erro: falha ao criar arquivo %s\n", nome);
        return 1;
    }

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            fprintf(f, "%d", buffer[i * largura + j]);
            if (j + 1 < largura) {
                fprintf(f, " ");
            }
        }
        fprintf(f, "\n");
    }

    if (fclose(f) != 0) {
        fprintf(stderr, "Erro: falha ao fechar o arquivo de saida mandelbrot_gnfr_serial.pgm\n");
        return 1;
    }
    
    return 0;
}

void calcularSerial(unsigned char *buffer, long largura, long altura, long max_iteracoes) {
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            double cr, ci;
            mapearPixel(j, i, largura, altura, &cr, &ci);
            int iter = contarIteracoes(cr, ci, max_iteracoes);
            int intensidade = (iter * 255) / max_iteracoes;
            buffer[i * largura + j] = intensidade;
        }
    }
}

void calcularOpenMP(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads) {
    #pragma omp parallel for schedule(static) num_threads((int)num_threads)
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            double cr, ci;
            mapearPixel(j, i, largura, altura, &cr, &ci);
            int iter = contarIteracoes(cr, ci, max_iteracoes);
            int intensidade = (iter * 255) / max_iteracoes;
            buffer[i * largura + j] = intensidade;
        }
    }
}