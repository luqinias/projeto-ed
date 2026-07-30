#include "test_utils.h"
#include "../src/pilha.h"
#include <string.h>

typedef struct {
    char linhas[32][160];
    int n;
} Coletor;

static void coletar(const Atividade *a, void *ctx) {
    Coletor *c = (Coletor *)ctx;
    strcpy(c->linhas[c->n], a->descricao);
    c->n++;
}

int main(void) {
    Pilha *p = pilha_criar(3);
    ASSERT_TRUE(pilha_vazia(p));

    Atividade a1 = { ATIV_NOVO_USUARIO, "usuario 1" };
    Atividade a2 = { ATIV_NOVA_AMIZADE, "amizade 1-2" };
    Atividade a3 = { ATIV_NOVA_PUBLICACAO, "publicacao de 1" };

    pilha_empilhar(p, a1);
    pilha_empilhar(p, a2);
    pilha_empilhar(p, a3);
    ASSERT_EQ_INT(pilha_tamanho(p), 3);

    Coletor c = {0};
    pilha_percorrer(p, coletar, &c);
    ASSERT_EQ_INT(c.n, 3);
    ASSERT_EQ_STR(c.linhas[0], "publicacao de 1");
    ASSERT_EQ_STR(c.linhas[1], "amizade 1-2");
    ASSERT_EQ_STR(c.linhas[2], "usuario 1");

    /* excede a capacidade máxima (3): o mais antigo deve ser descartado */
    Atividade a4 = { ATIV_REMOCAO_AMIZADE, "remocao 1-2" };
    pilha_empilhar(p, a4);
    ASSERT_EQ_INT(pilha_tamanho(p), 3);

    Coletor c2 = {0};
    pilha_percorrer(p, coletar, &c2);
    ASSERT_EQ_INT(c2.n, 3);
    ASSERT_EQ_STR(c2.linhas[0], "remocao 1-2");
    ASSERT_EQ_STR(c2.linhas[1], "publicacao de 1");
    ASSERT_EQ_STR(c2.linhas[2], "amizade 1-2");

    pilha_destruir(p);

    TEST_RESUMO();
}
