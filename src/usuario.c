#include "usuario.h"
#include <stdlib.h>
#include <string.h>

Usuario *usuario_criar(int id, const char *nome, const char *login) {
    Usuario *u = malloc(sizeof(Usuario));
    u->id = id;
    strncpy(u->nome, nome, USUARIO_NOME_TAM - 1);
    u->nome[USUARIO_NOME_TAM - 1] = '\0';
    strncpy(u->login, login, USUARIO_LOGIN_TAM - 1);
    u->login[USUARIO_LOGIN_TAM - 1] = '\0';
    u->publicacoes = lista_criar();
    return u;
}

void usuario_adicionar_publicacao(Usuario *u, const char *texto) {
    size_t tam = strlen(texto) + 1;
    char *copia = malloc(tam);
    memcpy(copia, texto, tam);
    lista_inserir_inicio(u->publicacoes, copia);
}

static void liberar_publicacao(void *dado) {
    free(dado);
}

void usuario_destruir(Usuario *u) {
    lista_destruir(u->publicacoes, liberar_publicacao);
    free(u);
}
