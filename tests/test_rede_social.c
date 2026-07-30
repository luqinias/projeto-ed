#include "test_utils.h"
#include "../src/rede_social.h"
#include <string.h>

typedef struct {
    char linhas[16][160];
    int n;
} ColetorHistorico;

static void coletar_historico(const Atividade *a, void *ctx) {
    ColetorHistorico *c = (ColetorHistorico *)ctx;
    strcpy(c->linhas[c->n], a->descricao);
    c->n++;
}

int main(void) {
    RedeSocial *rs = rede_social_criar();
    char erro[160];

    int id1 = rs_cadastrar_usuario(rs, "Ana Souza", "ana", erro, sizeof(erro));
    int id2 = rs_cadastrar_usuario(rs, "Bruno Lima", "bruno", erro, sizeof(erro));
    ASSERT_EQ_INT(id1, 1);
    ASSERT_EQ_INT(id2, 2);

    /* login duplicado deve falhar */
    int id_dup = rs_cadastrar_usuario(rs, "Outro Nome", "ana", erro, sizeof(erro));
    ASSERT_EQ_INT(id_dup, -1);
    ASSERT_TRUE(strlen(erro) > 0);

    /* busca por nome */
    Lista *achados = rs_buscar_por_nome(rs, "Ana Souza");
    ASSERT_TRUE(achados != NULL);
    ASSERT_EQ_INT(lista_tamanho(achados), 1);
    ASSERT_TRUE(rs_buscar_por_nome(rs, "Ninguem Assim") == NULL);

    /* amizade */
    ASSERT_EQ_INT(rs_adicionar_amizade(rs, id1, id2, erro, sizeof(erro)), 1);
    ASSERT_EQ_INT(rs_adicionar_amizade(rs, id1, id2, erro, sizeof(erro)), 0); /* duplicada */
    ASSERT_EQ_INT(rs_adicionar_amizade(rs, id1, 999, erro, sizeof(erro)), 0); /* id inexistente */

    Lista *amigos_de_1 = rs_exibir_amigos(rs, id1);
    ASSERT_EQ_INT(lista_tamanho(amigos_de_1), 1);
    lista_destruir(amigos_de_1, NULL);

    ASSERT_TRUE(rs_verificar_conexao(rs, id1, id2));

    /* publicacao */
    ASSERT_EQ_INT(rs_criar_publicacao(rs, id1, "Meu primeiro post!", erro, sizeof(erro)), 1);
    ASSERT_EQ_INT(rs_criar_publicacao(rs, 999, "post orfao", erro, sizeof(erro)), 0);

    /* historico: 4 eventos (2 cadastros + 1 amizade + 1 publicacao), mais recente primeiro */
    ColetorHistorico hist = {0};
    rs_exibir_historico(rs, coletar_historico, &hist);
    ASSERT_EQ_INT(hist.n, 4);
    ASSERT_TRUE(strstr(hist.linhas[0], "publicacao") != NULL);
    ASSERT_TRUE(strstr(hist.linhas[1], "amizade") != NULL);

    /* remocao de amizade */
    ASSERT_EQ_INT(rs_remover_amizade(rs, id1, id2, erro, sizeof(erro)), 1);
    ASSERT_EQ_INT(rs_remover_amizade(rs, id1, id2, erro, sizeof(erro)), 0); /* ja removida */
    ASSERT_TRUE(!rs_verificar_conexao(rs, id1, id2));

    rs_destruir(rs);

    TEST_RESUMO();
}
