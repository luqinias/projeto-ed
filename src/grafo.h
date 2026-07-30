#ifndef GRAFO_H
#define GRAFO_H

#include "lista.h"

/*
 * Grafo não direcionado representando a rede de amizades. Os vértices são
 * ids de usuário (int); o Grafo não conhece a struct Usuario (é um TAD
 * independente). Usa Fila internamente para BFS (conectividade e sugestão
 * de amizades) e Lista para as listas de adjacência.
 */

typedef struct Grafo Grafo;

typedef struct {
    int id;
    int amigos_em_comum;
} Sugestao;

Grafo *grafo_criar(void);

void grafo_adicionar_vertice(Grafo *g, int id);

/* Retorna 1 se criou a aresta, 0 se ela já existia (amizade duplicada). */
int grafo_adicionar_aresta(Grafo *g, int id1, int id2);

/* Retorna 1 se removeu a aresta, 0 se ela não existia. */
int grafo_remover_aresta(Grafo *g, int id1, int id2);

int grafo_sao_amigos(const Grafo *g, int id1, int id2);

/* Retorna uma NOVA lista com os ids dos vizinhos de 'id' (lista vazia se o
 * vértice não existir ou não tiver amigos). O chamador deve destruí-la com
 * lista_destruir(l, NULL). */
Lista *grafo_vizinhos(const Grafo *g, int id);

/* BFS: existe caminho entre origem e destino? */
int grafo_conectado(const Grafo *g, int origem, int destino);

/* Preenche 'saida' (capacidade >= max_sugestoes) com até max_sugestoes
 * sugestões de amizade para 'origem': usuários a distância 2 (amigos de
 * amigos, excluindo amigos diretos e o próprio usuário), ordenados por
 * número de amigos em comum decrescente e, em empate, por nome (usando o
 * callback nome_por_id). Retorna quantas sugestões foram preenchidas. */
int grafo_sugerir_amigos(const Grafo *g, int origem, int max_sugestoes,
                          const char *(*nome_por_id)(int id, void *ctx), void *ctx,
                          Sugestao *saida);

void grafo_destruir(Grafo *g);

#endif
