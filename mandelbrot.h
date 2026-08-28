#ifndef MANDELBROT_H
#define MANDELBROT_H

int validaInt(const char *s, long *saida);
void mapearPixel(int col, int row, long largura, long altura, double *cr, double *ci);
int contarIteracoes(double cr, double ci, long max_iteracoes);

#endif