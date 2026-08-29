#ifndef MANDELBROT_H
#define MANDELBROT_H

int validaInt(const char *s, long *saida);
int contarIteracoes(double cr, double ci, long max_iteracoes);
int escreverPGM(const char *nome, const unsigned char *buffer, long largura, long altura);
void mapearPixel(int col, int row, long largura, long altura, double *cr, double *ci);
void calcularSerial(unsigned char *buffer, long largura, long altura, long max_iteracoes);
void calcularOpenMP(unsigned char *buffer, long largura, long altura, long max_iteracoes, long num_threads);

#endif