#include "test_utils.h"
#include "../src/grafo.h"
#include "../src/lista.h"
#include <stdint.h>

static const char *nome_por_id(int id, void *ctx) {
    static const char *nomes[] = {"", "Ana", "Bruno", "Carla", "Duda", "Elis"};
    (void)ctx;
    return nomes[id];
}

int main(void) {
    Grafo *g = grafo_criar();

    for (int i = 1; i <= 5; i++) {
        grafo_adicionar_vertice(g, i);
    }

    ASSERT_EQ_INT(grafo_adicionar_aresta(g, 1, 2), 1);
    ASSERT_EQ_INT(grafo_adicionar_aresta(g, 1, 2), 0); /* duplicada */
    ASSERT_TRUE(grafo_sao_amigos(g, 1, 2));
    ASSERT_TRUE(!grafo_sao_amigos(g, 1, 3));

    ASSERT_EQ_INT(grafo_remover_aresta(g, 2, 3), 0); /* nao existia */
    ASSERT_EQ_INT(grafo_adicionar_aresta(g, 2, 3), 1);
    ASSERT_EQ_INT(grafo_remover_aresta(g, 2, 3), 1);
    ASSERT_TRUE(!grafo_sao_amigos(g, 2, 3));

    /* cadeia 1-2-3 (recriando 2-3) para testar conectividade indireta */
    grafo_adicionar_aresta(g, 2, 3);
    ASSERT_TRUE(grafo_conectado(g, 1, 3));
    ASSERT_TRUE(!grafo_conectado(g, 1, 4)); /* 4 esta isolado */

    Lista *vizinhos_de_1 = grafo_vizinhos(g, 1);
    ASSERT_EQ_INT(lista_tamanho(vizinhos_de_1), 1); /* so o 2 */
    lista_destruir(vizinhos_de_1, NULL);

    grafo_destruir(g);

    /* cenario de sugestao: A(1)-B(2), A(1)-C(3), A(1)-D(4), E(5)-B(2), E(5)-C(3) */
    Grafo *g2 = grafo_criar();
    for (int i = 1; i <= 5; i++) {
        grafo_adicionar_vertice(g2, i);
    }
    grafo_adicionar_aresta(g2, 1, 2);
    grafo_adicionar_aresta(g2, 1, 3);
    grafo_adicionar_aresta(g2, 1, 4);
    grafo_adicionar_aresta(g2, 5, 2);
    grafo_adicionar_aresta(g2, 5, 3);

    Sugestao saida[3];
    int n = grafo_sugerir_amigos(g2, 5, 3, nome_por_id, NULL, saida);
    ASSERT_EQ_INT(n, 1);
    ASSERT_EQ_INT(saida[0].id, 1);
    ASSERT_EQ_INT(saida[0].amigos_em_comum, 2);

    grafo_destruir(g2);

    TEST_RESUMO();
}
