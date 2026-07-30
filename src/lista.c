#include "lista.h"
#include <stdlib.h>

typedef struct NoLista {
    void *dado;
    struct NoLista *prox;
} NoLista;

struct Lista {
    NoLista *inicio;
    NoLista *fim;
    int tamanho;
};

Lista *lista_criar(void) {
    Lista *l = malloc(sizeof(Lista));
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
    return l;
}

void lista_inserir_inicio(Lista *l, void *dado) {
    NoLista *no = malloc(sizeof(NoLista));
    no->dado = dado;
    no->prox = l->inicio;
    l->inicio = no;
    if (l->fim == NULL) {
        l->fim = no;
    }
    l->tamanho++;
}

void lista_inserir_fim(Lista *l, void *dado) {
    NoLista *no = malloc(sizeof(NoLista));
    no->dado = dado;
    no->prox = NULL;
    if (l->fim == NULL) {
        l->inicio = no;
        l->fim = no;
    } else {
        l->fim->prox = no;
        l->fim = no;
    }
    l->tamanho++;
}

void *lista_remover_primeiro(Lista *l) {
    if (l->inicio == NULL) {
        return NULL;
    }
    NoLista *no = l->inicio;
    void *dado = no->dado;
    l->inicio = no->prox;
    if (l->inicio == NULL) {
        l->fim = NULL;
    }
    free(no);
    l->tamanho--;
    return dado;
}

int lista_remover_valor(Lista *l, void *dado) {
    NoLista *anterior = NULL;
    NoLista *atual = l->inicio;
    while (atual != NULL) {
        if (atual->dado == dado) {
            if (anterior == NULL) {
                l->inicio = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            if (atual == l->fim) {
                l->fim = anterior;
            }
            free(atual);
            l->tamanho--;
            return 1;
        }
        anterior = atual;
        atual = atual->prox;
    }
    return 0;
}

int lista_contem(const Lista *l, void *dado) {
    for (NoLista *no = l->inicio; no != NULL; no = no->prox) {
        if (no->dado == dado) {
            return 1;
        }
    }
    return 0;
}

int lista_tamanho(const Lista *l) {
    return l->tamanho;
}

int lista_vazia(const Lista *l) {
    return l->tamanho == 0;
}

void lista_percorrer(const Lista *l, void (*visitar)(void *dado, void *ctx), void *ctx) {
    for (NoLista *no = l->inicio; no != NULL; no = no->prox) {
        visitar(no->dado, ctx);
    }
}

void lista_destruir(Lista *l, void (*destruir_dado)(void *dado)) {
    NoLista *no = l->inicio;
    while (no != NULL) {
        NoLista *prox = no->prox;
        if (destruir_dado != NULL) {
            destruir_dado(no->dado);
        }
        free(no);
        no = prox;
    }
    free(l);
}
