#ifndef PILHA_H
#define PILHA_H

#include <stddef.h>

/*
 * Pilha (LIFO) de atividades recentes — usada para o histórico do sistema.
 * O topo é sempre a atividade mais recente. Tem uma capacidade máxima (N):
 * ao empilhar além dela, a atividade mais antiga é descartada.
 */

typedef enum {
    ATIV_NOVO_USUARIO,
    ATIV_NOVA_AMIZADE,
    ATIV_REMOCAO_AMIZADE,
    ATIV_NOVA_PUBLICACAO
} TipoAtividade;

typedef struct {
    TipoAtividade tipo;
    char descricao[280];
} Atividade;

typedef struct Pilha Pilha;

Pilha *pilha_criar(int capacidade_max);

void pilha_empilhar(Pilha *p, Atividade a);

int pilha_vazia(const Pilha *p);
int pilha_tamanho(const Pilha *p);

/* Percorre do topo (mais recente) para a base (mais antiga). */
void pilha_percorrer(const Pilha *p, void (*visitar)(const Atividade *a, void *ctx), void *ctx);

void pilha_destruir(Pilha *p);

#endif
