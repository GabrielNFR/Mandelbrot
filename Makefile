mandelbrot: mandelbrot.c
	clang -Wall -Wextra -D_POSIX_C_SOURCE=200809L -o mandelbrot mandelbrot.c
clean:
	rm -f mandelbrot
	