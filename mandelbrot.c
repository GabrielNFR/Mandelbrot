#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
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


    for (int row = 0; row < altura; row++) {
        for (int col = 0; col < largura; col++) {
            double cr, ci;
            mapearPixel(col, row, largura, altura, &cr, &ci);
            int it = contarIteracoes(cr, ci, max_iteracoes);
            fprintf(stderr, "pixel(%d,%d) c=(%.2f,%.2f) it=%d\n",
                    col, row, cr, ci, it);
        }
    }

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