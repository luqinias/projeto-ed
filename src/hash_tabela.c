#include "hash_tabela.h"
#include <stdlib.h>
#include <string.h>

#define HASH_BALDES_INICIAIS 16
#define HASH_FATOR_CARGA_MAX 0.75

typedef struct EntradaHash {
    char *chave;
    void *valor;         /* usado quando a chave foi inserida com permite_duplicata=0 */
    Lista *valores;       /* usado quando a chave foi inserida com permite_duplicata=1 */
    struct EntradaHash *prox;
} EntradaHash;

struct HashTabela {
    EntradaHash **baldes;
    int num_baldes;
    int num_elementos;
};

static unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++) != 0) {
        hash = ((hash << 5) + hash) + (unsigned long)c;
    }
    return hash;
}

static char *duplicar_string(const char *s) {
    size_t tam = strlen(s) + 1;
    char *copia = malloc(tam);
    memcpy(copia, s, tam);
    return copia;
}

HashTabela *hash_criar(void) {
    HashTabela *h = malloc(sizeof(HashTabela));
    h->num_baldes = HASH_BALDES_INICIAIS;
    h->num_elementos = 0;
    h->baldes = calloc((size_t)h->num_baldes, sizeof(EntradaHash *));
    return h;
}

static EntradaHash *buscar_entrada(HashTabela *h, const char *chave) {
    unsigned long indice = hash_djb2(chave) % (unsigned long)h->num_baldes;
    for (EntradaHash *e = h->baldes[indice]; e != NULL; e = e->prox) {
        if (strcmp(e->chave, chave) == 0) {
            return e;
        }
    }
    return NULL;
}

static void hash_rehash(HashTabela *h) {
    int novo_num_baldes = h->num_baldes * 2;
    EntradaHash **novos_baldes = calloc((size_t)novo_num_baldes, sizeof(EntradaHash *));

    for (int i = 0; i < h->num_baldes; i++) {
        EntradaHash *e = h->baldes[i];
        while (e != NULL) {
            EntradaHash *prox = e->prox;
            unsigned long novo_indice = hash_djb2(e->chave) % (unsigned long)novo_num_baldes;
            e->prox = novos_baldes[novo_indice];
            novos_baldes[novo_indice] = e;
            e = prox;
        }
    }

    free(h->baldes);
    h->baldes = novos_baldes;
    h->num_baldes = novo_num_baldes;
}

void hash_inserir(HashTabela *h, const char *chave, void *valor, int permite_duplicata) {
    EntradaHash *existente = buscar_entrada(h, chave);
    if (existente != NULL) {
        if (permite_duplicata) {
            if (existente->valores == NULL) {
                existente->valores = lista_criar();
            }
            lista_inserir_fim(existente->valores, valor);
        } else {
            existente->valor = valor;
        }
        return;
    }

    EntradaHash *nova = malloc(sizeof(EntradaHash));
    nova->chave = duplicar_string(chave);
    if (permite_duplicata) {
        nova->valor = NULL;
        nova->valores = lista_criar();
        lista_inserir_fim(nova->valores, valor);
    } else {
        nova->valor = valor;
        nova->valores = NULL;
    }

    unsigned long indice = hash_djb2(chave) % (unsigned long)h->num_baldes;
    nova->prox = h->baldes[indice];
    h->baldes[indice] = nova;
    h->num_elementos++;

    if ((double)h->num_elementos / (double)h->num_baldes > HASH_FATOR_CARGA_MAX) {
        hash_rehash(h);
    }
}

void *hash_buscar(HashTabela *h, const char *chave) {
    EntradaHash *e = buscar_entrada(h, chave);
    return e == NULL ? NULL : e->valor;
}

Lista *hash_buscar_todos(HashTabela *h, const char *chave) {
    EntradaHash *e = buscar_entrada(h, chave);
    return e == NULL ? NULL : e->valores;
}

int hash_contem(HashTabela *h, const char *chave) {
    return buscar_entrada(h, chave) != NULL;
}

void hash_destruir(HashTabela *h) {
    for (int i = 0; i < h->num_baldes; i++) {
        EntradaHash *e = h->baldes[i];
        while (e != NULL) {
            EntradaHash *prox = e->prox;
            if (e->valores != NULL) {
                lista_destruir(e->valores, NULL);
            }
            free(e->chave);
            free(e);
            e = prox;
        }
    }
    free(h->baldes);
    free(h);
}
