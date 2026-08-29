mandelbrot: mandelbrot.c
	clang -Wall -Wextra -o mandelbrot mandelbrot.c -lrt -fopenmp
clean:
	rm -f mandelbrot
	