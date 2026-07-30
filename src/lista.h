#ifndef LISTA_H
#define LISTA_H

/*
 * Lista encadeada simples genérica (void*).
 * Usada tanto para as publicações de um Usuario quanto para as listas de
 * adjacência do Grafo (guardando ids inteiros via cast de ponteiro).
 */

typedef struct Lista Lista;

Lista *lista_criar(void);

void lista_inserir_inicio(Lista *l, void *dado);
void lista_inserir_fim(Lista *l, void *dado);

/* Remove e retorna o primeiro elemento, ou NULL se a lista estiver vazia. */
void *lista_remover_primeiro(Lista *l);

/* Remove a primeira ocorrência de 'dado' comparando os ponteiros. Retorna 1 se
 * removeu, 0 se não encontrou. */
int lista_remover_valor(Lista *l, void *dado);

int lista_contem(const Lista *l, void *dado);
int lista_tamanho(const Lista *l);
int lista_vazia(const Lista *l);

void lista_percorrer(const Lista *l, void (*visitar)(void *dado, void *ctx), void *ctx);

/* Se destruir_dado != NULL, é chamado para cada dado antes de liberar os nós. */
void lista_destruir(Lista *l, void (*destruir_dado)(void *dado));

#endif
