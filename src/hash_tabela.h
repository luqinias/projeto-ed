#ifndef HASH_TABELA_H
#define HASH_TABELA_H

#include "lista.h"

/*
 * Tabela hash genérica (string -> void*), com encadeamento separado e rehash
 * dinâmico (dobra o número de baldes quando o fator de carga ultrapassa
 * 0.75), garantindo O(1) médio mesmo com o crescimento da rede social.
 *
 * Usada com duas instâncias por rede_social.c:
 *   - uma por login (chave única -> valida duplicidade no cadastro)
 *   - uma por nome  (chave não única -> busca de usuário por nome)
 */

typedef struct HashTabela HashTabela;

HashTabela *hash_criar(void);

/* permite_duplicata=0: sobrescreve o valor de uma chave já existente.
 * permite_duplicata=1: acumula múltiplos valores por chave numa Lista. */
void hash_inserir(HashTabela *h, const char *chave, void *valor, int permite_duplicata);

/* Retorna o valor único associado à chave (uso com permite_duplicata=0), ou
 * NULL se não encontrado. */
void *hash_buscar(HashTabela *h, const char *chave);

/* Retorna a Lista de valores associada à chave (uso com permite_duplicata=1),
 * ou NULL se a chave não existir. Não destruir a lista retornada. */
Lista *hash_buscar_todos(HashTabela *h, const char *chave);

int hash_contem(HashTabela *h, const char *chave);

void hash_destruir(HashTabela *h);

#endif
