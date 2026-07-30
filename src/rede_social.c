#include "rede_social.h"
#include "avl.h"
#include "hash_tabela.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define HISTORICO_TAMANHO 10

struct RedeSocial {
    AVL *por_id;
    HashTabela *por_login;
    HashTabela *por_nome;
    Grafo *grafo;
    Pilha *historico;
    int proximo_id;
};

RedeSocial *rede_social_criar(void) {
    RedeSocial *rs = malloc(sizeof(RedeSocial));
    rs->por_id = avl_criar();
    rs->por_login = hash_criar();
    rs->por_nome = hash_criar();
    rs->grafo = grafo_criar();
    rs->historico = pilha_criar(HISTORICO_TAMANHO);
    rs->proximo_id = 1;
    return rs;
}

static void definir_erro(char *erro, size_t erro_tam, const char *msg) {
    if (erro != NULL && erro_tam > 0) {
        strncpy(erro, msg, erro_tam - 1);
        erro[erro_tam - 1] = '\0';
    }
}

static void registrar_atividade(RedeSocial *rs, TipoAtividade tipo, const char *descricao) {
    Atividade a;
    a.tipo = tipo;
    strncpy(a.descricao, descricao, sizeof(a.descricao) - 1);
    a.descricao[sizeof(a.descricao) - 1] = '\0';
    pilha_empilhar(rs->historico, a);
}

static void inserir_usuario_interno(RedeSocial *rs, int id, const char *nome, const char *login) {
    Usuario *u = usuario_criar(id, nome, login);
    avl_inserir(rs->por_id, id, u);
    hash_inserir(rs->por_login, login, u, 0);
    hash_inserir(rs->por_nome, nome, u, 1);
    grafo_adicionar_vertice(rs->grafo, id);
    if (id >= rs->proximo_id) {
        rs->proximo_id = id + 1;
    }
}

int rs_cadastrar_usuario(RedeSocial *rs, const char *nome, const char *login, char *erro, size_t erro_tam) {
    if (nome == NULL || nome[0] == '\0' || login == NULL || login[0] == '\0') {
        definir_erro(erro, erro_tam, "Nome e login sao obrigatorios.");
        return -1;
    }
    if (hash_contem(rs->por_login, login)) {
        definir_erro(erro, erro_tam, "Ja existe um usuario cadastrado com esse login.");
        return -1;
    }

    int id = rs->proximo_id;
    inserir_usuario_interno(rs, id, nome, login);

    char desc[280];
    snprintf(desc, sizeof(desc), "Novo usuario cadastrado: %s (id %d, login %s)", nome, id, login);
    registrar_atividade(rs, ATIV_NOVO_USUARIO, desc);

    return id;
}

int rs_cadastrar_usuario_silencioso(RedeSocial *rs, int id, const char *nome, const char *login) {
    if (hash_contem(rs->por_login, login)) {
        return 0;
    }
    inserir_usuario_interno(rs, id, nome, login);
    return 1;
}

Lista *rs_buscar_por_nome(RedeSocial *rs, const char *nome) {
    return hash_buscar_todos(rs->por_nome, nome);
}

Usuario *rs_buscar_por_id(RedeSocial *rs, int id) {
    return (Usuario *)avl_buscar(rs->por_id, id);
}

int rs_adicionar_amizade(RedeSocial *rs, int id1, int id2, char *erro, size_t erro_tam) {
    Usuario *u1 = rs_buscar_por_id(rs, id1);
    Usuario *u2 = rs_buscar_por_id(rs, id2);
    if (u1 == NULL || u2 == NULL) {
        definir_erro(erro, erro_tam, "Um ou ambos os ids informados nao existem.");
        return 0;
    }
    if (id1 == id2) {
        definir_erro(erro, erro_tam, "Um usuario nao pode ser amigo de si mesmo.");
        return 0;
    }
    if (!grafo_adicionar_aresta(rs->grafo, id1, id2)) {
        definir_erro(erro, erro_tam, "Esses usuarios ja sao amigos.");
        return 0;
    }

    char desc[280];
    snprintf(desc, sizeof(desc), "Nova amizade: %s (id %d) e %s (id %d)", u1->nome, id1, u2->nome, id2);
    registrar_atividade(rs, ATIV_NOVA_AMIZADE, desc);
    return 1;
}

int rs_adicionar_amizade_silenciosa(RedeSocial *rs, int id1, int id2) {
    return grafo_adicionar_aresta(rs->grafo, id1, id2);
}

int rs_remover_amizade(RedeSocial *rs, int id1, int id2, char *erro, size_t erro_tam) {
    Usuario *u1 = rs_buscar_por_id(rs, id1);
    Usuario *u2 = rs_buscar_por_id(rs, id2);
    if (u1 == NULL || u2 == NULL) {
        definir_erro(erro, erro_tam, "Um ou ambos os ids informados nao existem.");
        return 0;
    }
    if (!grafo_remover_aresta(rs->grafo, id1, id2)) {
        definir_erro(erro, erro_tam, "Esses usuarios nao sao amigos.");
        return 0;
    }

    char desc[280];
    snprintf(desc, sizeof(desc), "Amizade removida: %s (id %d) e %s (id %d)", u1->nome, id1, u2->nome, id2);
    registrar_atividade(rs, ATIV_REMOCAO_AMIZADE, desc);
    return 1;
}

int rs_criar_publicacao(RedeSocial *rs, int id, const char *texto, char *erro, size_t erro_tam) {
    Usuario *u = rs_buscar_por_id(rs, id);
    if (u == NULL) {
        definir_erro(erro, erro_tam, "Usuario nao encontrado.");
        return 0;
    }
    if (texto == NULL || texto[0] == '\0') {
        definir_erro(erro, erro_tam, "O texto da publicacao nao pode ser vazio.");
        return 0;
    }

    usuario_adicionar_publicacao(u, texto);

    char desc[280];
    snprintf(desc, sizeof(desc), "Nova publicacao de %s (id %d)", u->nome, id);
    registrar_atividade(rs, ATIV_NOVA_PUBLICACAO, desc);
    return 1;
}

int rs_criar_publicacao_silenciosa(RedeSocial *rs, int id, const char *texto) {
    Usuario *u = rs_buscar_por_id(rs, id);
    if (u == NULL) {
        return 0;
    }
    usuario_adicionar_publicacao(u, texto);
    return 1;
}

void rs_empilhar_historico_bruto(RedeSocial *rs, Atividade a) {
    pilha_empilhar(rs->historico, a);
}

Lista *rs_exibir_amigos(RedeSocial *rs, int id) {
    Lista *resultado = lista_criar();
    Lista *vizinhos = grafo_vizinhos(rs->grafo, id);
    void *dado;
    while ((dado = lista_remover_primeiro(vizinhos)) != NULL) {
        int vid = (int)(intptr_t)dado;
        Usuario *u = rs_buscar_por_id(rs, vid);
        if (u != NULL) {
            lista_inserir_fim(resultado, u);
        }
    }
    lista_destruir(vizinhos, NULL);
    return resultado;
}

int rs_verificar_conexao(RedeSocial *rs, int id1, int id2) {
    return grafo_conectado(rs->grafo, id1, id2);
}

static const char *nome_por_id_cb(int id, void *ctx) {
    RedeSocial *rs = (RedeSocial *)ctx;
    Usuario *u = rs_buscar_por_id(rs, id);
    return u != NULL ? u->nome : "";
}

int rs_sugerir_amizades(RedeSocial *rs, int id, Sugestao *saida) {
    return grafo_sugerir_amigos(rs->grafo, id, 3, nome_por_id_cb, rs, saida);
}

void rs_exibir_historico(const RedeSocial *rs, void (*visitar)(const Atividade *a, void *ctx), void *ctx) {
    pilha_percorrer(rs->historico, visitar, ctx);
}

typedef struct {
    void (*visitar)(Usuario *u, void *ctx);
    void *ctx;
} CtxPercorrerUsuarios;

static void trampolim_percorrer_usuarios(void *dado, void *ctx_bruto) {
    CtxPercorrerUsuarios *c = (CtxPercorrerUsuarios *)ctx_bruto;
    c->visitar((Usuario *)dado, c->ctx);
}

void rs_percorrer_usuarios(RedeSocial *rs, void (*visitar)(Usuario *u, void *ctx), void *ctx) {
    CtxPercorrerUsuarios c = { visitar, ctx };
    avl_percorrer_em_ordem(rs->por_id, trampolim_percorrer_usuarios, &c);
}

typedef struct {
    RedeSocial *rs;
    void (*visitar)(int id1, int id2, void *ctx);
    void *ctx;
} CtxAmizades;

static void visitar_usuario_para_amizades(Usuario *u, void *ctx_bruto) {
    CtxAmizades *c = (CtxAmizades *)ctx_bruto;
    Lista *vizinhos = grafo_vizinhos(c->rs->grafo, u->id);
    void *dado;
    while ((dado = lista_remover_primeiro(vizinhos)) != NULL) {
        int vizinho = (int)(intptr_t)dado;
        if (vizinho > u->id) {
            c->visitar(u->id, vizinho, c->ctx);
        }
    }
    lista_destruir(vizinhos, NULL);
}

void rs_percorrer_amizades(RedeSocial *rs, void (*visitar)(int id1, int id2, void *ctx), void *ctx) {
    CtxAmizades c = { rs, visitar, ctx };
    rs_percorrer_usuarios(rs, visitar_usuario_para_amizades, &c);
}

static void destruir_usuario_cb(void *dado) {
    usuario_destruir((Usuario *)dado);
}

void rs_destruir(RedeSocial *rs) {
    hash_destruir(rs->por_login);
    hash_destruir(rs->por_nome);
    grafo_destruir(rs->grafo);
    pilha_destruir(rs->historico);
    avl_destruir(rs->por_id, destruir_usuario_cb);
    free(rs);
}
