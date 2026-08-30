mandelbrot: mandelbrot.c
	clang -Wall -Wextra -o mandelbrot mandelbrot.c -lrt -fopenmp -pthreads
clean:
	rm -f mandelbrot
	