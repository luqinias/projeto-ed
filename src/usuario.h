#ifndef USUARIO_H
#define USUARIO_H

#include "lista.h"

#define USUARIO_NOME_TAM 100
#define USUARIO_LOGIN_TAM 50

/*
 * Registro de usuário. Não é um TAD com estado interno complexo — é o
 * "payload" indexado pela AVL (por id) e pelas tabelas hash (por login e por
 * nome). As publicações usam a Lista encadeada genérica, sempre inserindo no
 * início para que a mais recente fique em primeiro lugar.
 */

typedef struct {
    int id;
    char nome[USUARIO_NOME_TAM];
    char login[USUARIO_LOGIN_TAM];
    Lista *publicacoes; /* Lista de char* (texto da publicação) */
} Usuario;

Usuario *usuario_criar(int id, const char *nome, const char *login);

void usuario_adicionar_publicacao(Usuario *u, const char *texto);

void usuario_destruir(Usuario *u);

#endif
