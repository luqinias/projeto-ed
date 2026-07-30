#include "pilha.h"
#include <stdlib.h>

typedef struct NoPilha {
    Atividade dado;
    struct NoPilha *prox; /* em direção à base (mais antigo) */
    struct NoPilha *ant;  /* em direção ao topo (mais recente) */
} NoPilha;

struct Pilha {
    NoPilha *topo;
    NoPilha *base;
    int tamanho;
    int capacidade_max;
};

Pilha *pilha_criar(int capacidade_max) {
    Pilha *p = malloc(sizeof(Pilha));
    p->topo = NULL;
    p->base = NULL;
    p->tamanho = 0;
    p->capacidade_max = capacidade_max;
    return p;
}

void pilha_empilhar(Pilha *p, Atividade a) {
    NoPilha *no = malloc(sizeof(NoPilha));
    no->dado = a;
    no->prox = p->topo;
    no->ant = NULL;
    if (p->topo != NULL) {
        p->topo->ant = no;
    }
    p->topo = no;
    if (p->base == NULL) {
        p->base = no;
    }
    p->tamanho++;

    if (p->tamanho > p->capacidade_max) {
        NoPilha *velho = p->base;
        p->base = velho->ant;
        if (p->base != NULL) {
            p->base->prox = NULL;
        } else {
            p->topo = NULL;
        }
        free(velho);
        p->tamanho--;
    }
}

int pilha_vazia(const Pilha *p) {
    return p->tamanho == 0;
}

int pilha_tamanho(const Pilha *p) {
    return p->tamanho;
}

void pilha_percorrer(const Pilha *p, void (*visitar)(const Atividade *a, void *ctx), void *ctx) {
    for (NoPilha *no = p->topo; no != NULL; no = no->prox) {
        visitar(&no->dado, ctx);
    }
}

void pilha_destruir(Pilha *p) {
    NoPilha *no = p->topo;
    while (no != NULL) {
        NoPilha *prox = no->prox;
        free(no);
        no = prox;
    }
    free(p);
}
