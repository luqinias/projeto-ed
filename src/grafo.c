#include "grafo.h"
#include "fila.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Grafo {
    Lista **adjacencia; /* adjacencia[id] = Lista de ids vizinhos (ou NULL se o vertice nao existe) */
    int capacidade;      /* tamanho atual do vetor adjacencia */
};

static void *id_para_ptr(int id) {
    return (void *)(intptr_t)id;
}

static int ptr_para_id(void *p) {
    return (int)(intptr_t)p;
}

Grafo *grafo_criar(void) {
    Grafo *g = malloc(sizeof(Grafo));
    g->adjacencia = NULL;
    g->capacidade = 0;
    return g;
}

static void garantir_capacidade(Grafo *g, int id) {
    if (id < g->capacidade) {
        return;
    }
    int nova_capacidade = g->capacidade == 0 ? (id + 1) : g->capacidade;
    while (nova_capacidade <= id) {
        nova_capacidade *= 2;
    }
    Lista **novo = realloc(g->adjacencia, (size_t)nova_capacidade * sizeof(Lista *));
    for (int i = g->capacidade; i < nova_capacidade; i++) {
        novo[i] = NULL;
    }
    g->adjacencia = novo;
    g->capacidade = nova_capacidade;
}

static int vertice_existe(const Grafo *g, int id) {
    return id >= 0 && id < g->capacidade && g->adjacencia[id] != NULL;
}

void grafo_adicionar_vertice(Grafo *g, int id) {
    garantir_capacidade(g, id);
    if (g->adjacencia[id] == NULL) {
        g->adjacencia[id] = lista_criar();
    }
}

int grafo_adicionar_aresta(Grafo *g, int id1, int id2) {
    grafo_adicionar_vertice(g, id1);
    grafo_adicionar_vertice(g, id2);

    if (grafo_sao_amigos(g, id1, id2)) {
        return 0;
    }

    lista_inserir_fim(g->adjacencia[id1], id_para_ptr(id2));
    lista_inserir_fim(g->adjacencia[id2], id_para_ptr(id1));
    return 1;
}

int grafo_remover_aresta(Grafo *g, int id1, int id2) {
    if (!grafo_sao_amigos(g, id1, id2)) {
        return 0;
    }
    lista_remover_valor(g->adjacencia[id1], id_para_ptr(id2));
    lista_remover_valor(g->adjacencia[id2], id_para_ptr(id1));
    return 1;
}

int grafo_sao_amigos(const Grafo *g, int id1, int id2) {
    if (!vertice_existe(g, id1) || !vertice_existe(g, id2)) {
        return 0;
    }
    return lista_contem(g->adjacencia[id1], id_para_ptr(id2));
}

typedef struct {
    Lista *destino;
} CtxCopiarVizinhos;

static void copiar_vizinho(void *dado, void *ctx) {
    CtxCopiarVizinhos *c = (CtxCopiarVizinhos *)ctx;
    lista_inserir_fim(c->destino, dado);
}

Lista *grafo_vizinhos(const Grafo *g, int id) {
    Lista *copia = lista_criar();
    if (vertice_existe(g, id)) {
        CtxCopiarVizinhos ctx = { copia };
        lista_percorrer(g->adjacencia[id], copiar_vizinho, &ctx);
    }
    return copia;
}

int grafo_conectado(const Grafo *g, int origem, int destino) {
    if (!vertice_existe(g, origem) || !vertice_existe(g, destino)) {
        return 0;
    }
    if (origem == destino) {
        return 1;
    }

    int *visitado = calloc((size_t)g->capacidade, sizeof(int));
    Fila *fila = fila_criar();

    visitado[origem] = 1;
    fila_enfileirar(fila, origem);

    int encontrado = 0;
    int atual;
    while (!encontrado && fila_desenfileirar(fila, &atual)) {
        Lista *vizinhos = grafo_vizinhos(g, atual);
        void *no_dado;
        while ((no_dado = lista_remover_primeiro(vizinhos)) != NULL) {
            int vizinho = ptr_para_id(no_dado);
            if (vizinho == destino) {
                encontrado = 1;
            }
            if (!visitado[vizinho]) {
                visitado[vizinho] = 1;
                fila_enfileirar(fila, vizinho);
            }
        }
        lista_destruir(vizinhos, NULL);
    }

    fila_destruir(fila);
    free(visitado);
    return encontrado;
}

static int contar_amigos_em_comum(const Grafo *g, int id1, int id2) {
    int comuns = 0;
    Lista *vizinhos1 = grafo_vizinhos(g, id1);
    void *dado;
    /* como grafo_vizinhos retorna uma copia, podemos consumi-la com
     * lista_remover_primeiro sem afetar o grafo original */
    while ((dado = lista_remover_primeiro(vizinhos1)) != NULL) {
        if (lista_contem(g->adjacencia[id2], dado)) {
            comuns++;
        }
    }
    lista_destruir(vizinhos1, NULL);
    return comuns;
}

int grafo_sugerir_amigos(const Grafo *g, int origem, int max_sugestoes,
                          const char *(*nome_por_id)(int id, void *ctx), void *ctx,
                          Sugestao *saida) {
    if (!vertice_existe(g, origem)) {
        return 0;
    }

    int *dist = malloc((size_t)g->capacidade * sizeof(int));
    for (int i = 0; i < g->capacidade; i++) {
        dist[i] = -1;
    }
    dist[origem] = 0;

    Fila *fila = fila_criar();
    fila_enfileirar(fila, origem);

    int atual;
    while (fila_desenfileirar(fila, &atual)) {
        if (dist[atual] >= 2) {
            continue;
        }
        Lista *vizinhos = grafo_vizinhos(g, atual);
        void *dado;
        while ((dado = lista_remover_primeiro(vizinhos)) != NULL) {
            int vizinho = ptr_para_id(dado);
            if (dist[vizinho] == -1) {
                dist[vizinho] = dist[atual] + 1;
                if (dist[vizinho] <= 2) {
                    fila_enfileirar(fila, vizinho);
                }
            }
        }
        lista_destruir(vizinhos, NULL);
    }
    fila_destruir(fila);

    /* candidatos: distancia exatamente 2 (amigos de amigos, exclui amigos diretos e o proprio) */
    Sugestao *candidatos = malloc((size_t)g->capacidade * sizeof(Sugestao));
    int total_candidatos = 0;
    for (int id = 0; id < g->capacidade; id++) {
        if (dist[id] == 2) {
            candidatos[total_candidatos].id = id;
            candidatos[total_candidatos].amigos_em_comum = contar_amigos_em_comum(g, origem, id);
            total_candidatos++;
        }
    }
    free(dist);

    /* ordenacao por insercao: amigos_em_comum desc, nome asc em empate */
    for (int i = 1; i < total_candidatos; i++) {
        Sugestao chave = candidatos[i];
        const char *nome_chave = nome_por_id(chave.id, ctx);
        int j = i - 1;
        while (j >= 0) {
            int deve_mover;
            if (candidatos[j].amigos_em_comum != chave.amigos_em_comum) {
                deve_mover = candidatos[j].amigos_em_comum < chave.amigos_em_comum;
            } else {
                deve_mover = strcmp(nome_por_id(candidatos[j].id, ctx), nome_chave) > 0;
            }
            if (!deve_mover) {
                break;
            }
            candidatos[j + 1] = candidatos[j];
            j--;
        }
        candidatos[j + 1] = chave;
    }

    int total_saida = total_candidatos < max_sugestoes ? total_candidatos : max_sugestoes;
    for (int i = 0; i < total_saida; i++) {
        saida[i] = candidatos[i];
    }
    free(candidatos);

    return total_saida;
}

void grafo_destruir(Grafo *g) {
    for (int i = 0; i < g->capacidade; i++) {
        if (g->adjacencia[i] != NULL) {
            lista_destruir(g->adjacencia[i], NULL);
        }
    }
    free(g->adjacencia);
    free(g);
}
