#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <omp.h>
#include <pthread.h>
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

    clock_gettime(CLOCK_MONOTONIC, &ini);
    calcularPthreads1(buffer, largura, altura, max_iteracoes, num_threads);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo_pthreads1 = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;

    if (escreverPGM("mandelbrot_gnfr_pthreads1.pgm", buffer, largura, altura) != 0) {
        free(buffer);
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &ini);
    calcularPthreads2(buffer, largura, altura, max_iteracoes, num_threads);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo_pthreads2 = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;

    if (escreverPGM("mandelbrot_gnfr_pthreads2.pgm", buffer, largura, altura) != 0) {
        free(buffer);
        return 1;
    }

    FILE *t = fopen("times.txt", "w");
    if (t == NULL) {
        fprintf(stderr, "Erro: falha ao criar arquivo de saida times.txt\n");
        free(buffer);
        return 1;
    }

    fprintf(t, "serial %.9f\nopenmp %.9f\npthreads1 %.9f\npthreads2 %.9f\n", tempo_serial, tempo_openmp, tempo_pthreads1, tempo_pthreads2);

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

int calcularPthreads1(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads) {
    pthread_t threads[num_threads];
    Thread args[num_threads];

    for (long i = 0; i < num_threads; i++) {
        args[i].buffer = buffer;
        args[i].largura = largura;
        args[i].altura = altura;
        args[i].max_iteracoes = max_iteracoes;
        args[i].num_threads = num_threads;
        args[i].id = i;

        if (pthread_create(&threads[i], NULL, rotinaBlocos, &args[i]) != 0) {
            fprintf(stderr, "Erro: falha ao criar a thread %ld\n", i);
            return 1;
        }
    }

    for (long i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Erro: falha ao aguardar a thread %ld\n", i);
            return 1;
        }
    }
    return 0;
}

int calcularPthreads2(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads) {
    pthread_t threads[num_threads];
    Thread args[num_threads];

    for (long i = 0; i < num_threads; i++) {
        args[i].buffer = buffer;
        args[i].largura = largura;
        args[i].altura = altura;
        args[i].max_iteracoes = max_iteracoes;
        args[i].num_threads = num_threads;
        args[i].id = i;

        if (pthread_create(&threads[i], NULL, rotinaIntercalada, &args[i]) != 0) {
            fprintf(stderr, "Erro: falha ao criar a thread %ld\n", i);
            return 1;
        }
    }

    for (long i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Erro: falha ao aguardar a thread %ld\n", i);
            return 1;
        }
    }
    return 0;
}

void* rotinaBlocos(void *arg) {
    Thread *t = (Thread*)arg;
    long inicio = (t->id * t->altura) / t->num_threads;
    long fim = ((t->id + 1) * t->altura) / t->num_threads;
    for (long i = inicio; i < fim; i++) {
        for (long j = 0; j < t->largura; j++) {
            double cr, ci;
            mapearPixel((int)j, (int)i, t->largura, t->altura, &cr, &ci);
            int iter = contarIteracoes(cr, ci, t->max_iteracoes);
            t->buffer[i * t->largura + j] = (unsigned char)((iter * 255) / t->max_iteracoes);
        }
    }
    return NULL;
}

void* rotinaIntercalada(void *arg) {   
    Thread *t = (Thread*)arg;
    for (long i = t->id; i < t->altura; i += t->num_threads)
        for (long j = 0; j < t->largura; j++) {
            double cr, ci;
            mapearPixel((int)j, (int)i, t->largura, t->altura, &cr, &ci);
            int iter = contarIteracoes(cr, ci, t->max_iteracoes);
            t->buffer[i * t->largura + j] = (unsigned char)((iter * 255) / t->max_iteracoes);
        }
    return NULL;
}