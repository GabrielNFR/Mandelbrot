# Mandelbrot — Entrega Threads 1

Implementação do fractal de Mandelbrot em linguagem C com **quatro estratégias de
paralelização**, comparando desempenho entre execução serial, OpenMP e duas
variações com pthreads (blocos e intercalada).

## Como compilar

```bash
make
```

O `Makefile` usa `clang` com as flags `-Wall -Wextra -fopenmp` e a biblioteca de
threads (`-pthreads`).

Para limpar o binário:

```bash
make clean
```

## Como executar

```bash
./mandelbrot <largura> <altura> <max_iteracoes> <num_threads>
```

### Parâmetros

| Parâmetro         | Descrição                                        | Restrição |
|-------------------|--------------------------------------------------|-----------|
| `largura`         | Largura da imagem em pixels                      | > 0       |
| `altura`          | Altura da imagem em pixels                       | > 0       |
| `max_iteracoes`   | Número máximo de iterações por ponto             | > 0       |
| `num_threads`     | Quantidade de threads para OpenMP e pthreads     | > 0       |

### Exemplo

```bash
./mandelbrot 800 600 1000 4
```

## Saída gerada

O programa executa as 4 variantes em sequência e gera os seguintes arquivos:

| Arquivo                              | Descrição                                        |
|--------------------------------------|--------------------------------------------------|
| `mandelbrot_gnfr_serial.pgm`         | Execução serial (uma thread)                     |
| `mandelbrot_gnfr_openmp.pgm`         | Execução com OpenMP (schedule static)            |
| `mandelbrot_gnfr_pthreads1.pgm`      | Execução com pthreads — divisão em blocos        |
| `mandelbrot_gnfr_pthreads2.pgm`      | Execução com pthreads — divisão intercalada      |
| `times.txt`                          | Tempos de cada variante (9 casas decimais)       |

O `times.txt` segue o formato:

```
serial 1.234567890
openmp 0.456789012
pthreads1 0.478901234
pthreads2 0.490123456
```

Os arquivos `.pgm` são gravados apenas com os valores de intensidade dos pixels,
um por linha (formato sem cabeçalho, conforme o padrão adotado na atividade).

## Arquitetura

| Arquivo           | Responsabilidade                                             |
|-------------------|--------------------------------------------------------------|
| `mandelbrot.c`    | `main`, validação de argumentos, medição de tempo e saída    |
| `mandelbrot.h`    | Declarações das funções e da struct `Thread`                 |
| `Makefile`        | Compilação e limpeza do projeto                              |

## Estratégias de paralelização

### Serial — `calcularSerial`
Percorre todos os pixels em dois laços aninhados, calculando cada ponto
individualmente.

### OpenMP — `calcularOpenMP`
Distribui as linhas da imagem entre as threads com
`#pragma omp parallel for schedule(static)`, repartindo as linhas em blocos
contíguos de tamanho aproximadamente igual.

### Pthreads 1 (blocos) — `calcularPthreads1` / `rotinaBlocos`
Cada thread `id` processa um intervalo contíguo de linhas, calculado como:

```
inicio = (id * altura) / num_threads
fim    = ((id + 1) * altura) / num_threads
```

### Pthreads 2 (intercalada) — `calcularPthreads2` / `rotinaIntercalada`
Cada thread `id` processa as linhas intercaladas:

```
linha = id, id + num_threads, id + 2*num_threads, ...
```

## Detalhes de implementação

- **Região do fractal:** eixo real $[-2.0, 1.0]$ e eixo imaginário $[-1.5, 1.5]$.
- **Mapeamento de pixel:** `mapearPixel` converte coluna/linha em coordenadas
  complexas $c_r$ e $c_i$.
- **Convergência:** `contarIteracoes` itera $z_{n+1} = z_n^2 + c$ até
  $|z|^2 > 4$ ou atingir `max_iteracoes`.
- **Intensidade:** cada pixel recebe `(iter * 255) / max_iteracoes`.
- **Memória:** um único buffer é alocado, compartilhado entre as variantes. Como
  cada thread escreve apenas nos pixels que lhe cabem, não há condição de corrida.
- **Medição de tempo:** usa `clock_gettime(CLOCK_MONOTONIC)` em cada variante.

## Validação de argumentos

`validaInt` rejeita argumentos que não sejam inteiros positivos (valor inválido,
caracteres extras, overflow de `long` ou valor ≤ 0), exibindo mensagem de erro e
encerrando com código de retorno 1.
