#include "test_utils.h"
#include "../src/hash_tabela.h"
#include "../src/lista.h"
#include <stdio.h>

int main(void) {
    HashTabela *h = hash_criar();

    int v1 = 1, v2 = 2, v3 = 3;

    /* chave unica (login): sobrescreve em caso de repeticao */
    hash_inserir(h, "joao123", &v1, 0);
    ASSERT_TRUE(hash_buscar(h, "joao123") == &v1);
    ASSERT_TRUE(hash_buscar(h, "inexistente") == NULL);
    ASSERT_TRUE(hash_contem(h, "joao123"));
    ASSERT_TRUE(!hash_contem(h, "inexistente"));

    hash_inserir(h, "joao123", &v2, 0);
    ASSERT_TRUE(hash_buscar(h, "joao123") == &v2);

    /* chave nao unica (nome): acumula em lista */
    hash_inserir(h, "Maria", &v1, 1);
    hash_inserir(h, "Maria", &v2, 1);
    hash_inserir(h, "Maria", &v3, 1);
    Lista *todos = hash_buscar_todos(h, "Maria");
    ASSERT_TRUE(todos != NULL);
    ASSERT_EQ_INT(lista_tamanho(todos), 3);
    ASSERT_TRUE(hash_buscar_todos(h, "ninguem") == NULL);

    /* forca rehash: insere muitas chaves unicas e confere que todas sao
     * encontradas depois (prova que o rehash nao perde dados) */
    char chave[16];
    int valores[200];
    for (int i = 0; i < 200; i++) {
        valores[i] = i;
        snprintf(chave, sizeof(chave), "user%d", i);
        hash_inserir(h, chave, &valores[i], 0);
    }
    for (int i = 0; i < 200; i++) {
        snprintf(chave, sizeof(chave), "user%d", i);
        void *achado = hash_buscar(h, chave);
        ASSERT_TRUE(achado == &valores[i]);
    }

    hash_destruir(h);

    TEST_RESUMO();
}
