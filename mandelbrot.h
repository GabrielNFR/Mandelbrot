#ifndef MANDELBROT_H
#define MANDELBROT_H

typedef struct {
    unsigned char *buffer;
    long largura, altura, max_iteracoes;
    long num_threads;
    long id;
} Thread;

int validaInt(const char *s, long *saida);
int contarIteracoes(double cr, double ci, long max_iteracoes);
int escreverPGM(const char *nome, const unsigned char *buffer, long largura, long altura);
void mapearPixel(int col, int row, long largura, long altura, double *cr, double *ci);
void calcularSerial(unsigned char *buffer, long largura, long altura, long max_iteracoes);
void calcularOpenMP(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads);
int calcularPthreads1(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads);
int calcularPthreads2(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads);
void* rotinaBlocos(void *arg);
void* rotinaIntercalada(void *arg);

#endif