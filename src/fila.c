#include "fila.h"
#include <stdlib.h>

typedef struct NoFila {
    int valor;
    struct NoFila *prox;
} NoFila;

struct Fila {
    NoFila *inicio;
    NoFila *fim;
};

Fila *fila_criar(void) {
    Fila *f = malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    return f;
}

void fila_enfileirar(Fila *f, int valor) {
    NoFila *no = malloc(sizeof(NoFila));
    no->valor = valor;
    no->prox = NULL;
    if (f->fim == NULL) {
        f->inicio = no;
        f->fim = no;
    } else {
        f->fim->prox = no;
        f->fim = no;
    }
}

int fila_desenfileirar(Fila *f, int *valor_saida) {
    if (f->inicio == NULL) {
        return 0;
    }
    NoFila *no = f->inicio;
    *valor_saida = no->valor;
    f->inicio = no->prox;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    free(no);
    return 1;
}

int fila_vazia(const Fila *f) {
    return f->inicio == NULL;
}

void fila_destruir(Fila *f) {
    NoFila *no = f->inicio;
    while (no != NULL) {
        NoFila *prox = no->prox;
        free(no);
        no = prox;
    }
    free(f);
}
