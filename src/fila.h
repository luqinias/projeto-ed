#ifndef FILA_H
#define FILA_H

/*
 * Fila (FIFO) de inteiros, usada pela busca em largura (BFS) do Grafo.
 */

typedef struct Fila Fila;

Fila *fila_criar(void);

void fila_enfileirar(Fila *f, int valor);

/* Remove o elemento da frente e o copia para *valor_saida.
 * Retorna 1 se havia elemento, 0 se a fila estava vazia. */
int fila_desenfileirar(Fila *f, int *valor_saida);

int fila_vazia(const Fila *f);

void fila_destruir(Fila *f);

#endif
