#include "avl.h"
#include <stdlib.h>

struct NoAVL {
    int id;
    void *dado;
    int altura;
    NoAVL *esq;
    NoAVL *dir;
};

struct AVL {
    NoAVL *raiz;
};

AVL *avl_criar(void) {
    AVL *arv = malloc(sizeof(AVL));
    arv->raiz = NULL;
    return arv;
}

static int altura_no(NoAVL *no) {
    return no == NULL ? 0 : no->altura;
}

static int max_int(int a, int b) {
    return a > b ? a : b;
}

static void atualizar_altura(NoAVL *no) {
    no->altura = 1 + max_int(altura_no(no->esq), altura_no(no->dir));
}

static int fator_balanceamento(NoAVL *no) {
    return altura_no(no->esq) - altura_no(no->dir);
}

static NoAVL *rotacionar_direita(NoAVL *y) {
    NoAVL *x = y->esq;
    NoAVL *t2 = x->dir;

    x->dir = y;
    y->esq = t2;

    atualizar_altura(y);
    atualizar_altura(x);

    return x;
}

static NoAVL *rotacionar_esquerda(NoAVL *x) {
    NoAVL *y = x->dir;
    NoAVL *t2 = y->esq;

    y->esq = x;
    x->dir = t2;

    atualizar_altura(x);
    atualizar_altura(y);

    return y;
}

static NoAVL *rebalancear(NoAVL *no) {
    atualizar_altura(no);
    int fb = fator_balanceamento(no);

    if (fb > 1) {
        if (fator_balanceamento(no->esq) < 0) {
            no->esq = rotacionar_esquerda(no->esq); /* caso esquerda-direita */
        }
        return rotacionar_direita(no); /* caso esquerda-esquerda */
    }

    if (fb < -1) {
        if (fator_balanceamento(no->dir) > 0) {
            no->dir = rotacionar_direita(no->dir); /* caso direita-esquerda */
        }
        return rotacionar_esquerda(no); /* caso direita-direita */
    }

    return no;
}

static NoAVL *inserir_no(NoAVL *no, int id, void *dado) {
    if (no == NULL) {
        NoAVL *novo = malloc(sizeof(NoAVL));
        novo->id = id;
        novo->dado = dado;
        novo->altura = 1;
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    if (id < no->id) {
        no->esq = inserir_no(no->esq, id, dado);
    } else if (id > no->id) {
        no->dir = inserir_no(no->dir, id, dado);
    } else {
        no->dado = dado; /* id já existe: apenas atualiza o dado */
        return no;
    }

    return rebalancear(no);
}

void avl_inserir(AVL *arv, int id, void *dado) {
    arv->raiz = inserir_no(arv->raiz, id, dado);
}

void *avl_buscar(const AVL *arv, int id) {
    NoAVL *no = arv->raiz;
    while (no != NULL) {
        if (id == no->id) {
            return no->dado;
        }
        no = (id < no->id) ? no->esq : no->dir;
    }
    return NULL;
}

static void percorrer_em_ordem_no(NoAVL *no, void (*visitar)(void *dado, void *ctx), void *ctx) {
    if (no == NULL) {
        return;
    }
    percorrer_em_ordem_no(no->esq, visitar, ctx);
    visitar(no->dado, ctx);
    percorrer_em_ordem_no(no->dir, visitar, ctx);
}

void avl_percorrer_em_ordem(const AVL *arv, void (*visitar)(void *dado, void *ctx), void *ctx) {
    percorrer_em_ordem_no(arv->raiz, visitar, ctx);
}

int avl_altura(const AVL *arv) {
    return altura_no(arv->raiz);
}

static void destruir_no(NoAVL *no, void (*destruir_dado)(void *dado)) {
    if (no == NULL) {
        return;
    }
    destruir_no(no->esq, destruir_dado);
    destruir_no(no->dir, destruir_dado);
    if (destruir_dado != NULL) {
        destruir_dado(no->dado);
    }
    free(no);
}

void avl_destruir(AVL *arv, void (*destruir_dado)(void *dado)) {
    destruir_no(arv->raiz, destruir_dado);
    free(arv);
}
