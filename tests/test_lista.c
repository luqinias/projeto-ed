#include "test_utils.h"
#include "../src/lista.h"
#include <stdlib.h>
#include <string.h>

static void concatenar(void *dado, void *ctx) {
    char *buf = (char *)ctx;
    strcat(buf, (const char *)dado);
}

int main(void) {
    Lista *l = lista_criar();
    ASSERT_TRUE(lista_vazia(l));
    ASSERT_EQ_INT(lista_tamanho(l), 0);

    lista_inserir_fim(l, "a");
    lista_inserir_fim(l, "b");
    lista_inserir_fim(l, "c");
    ASSERT_EQ_INT(lista_tamanho(l), 3);
    ASSERT_TRUE(!lista_vazia(l));

    char buf[16] = {0};
    lista_percorrer(l, concatenar, buf);
    ASSERT_EQ_STR(buf, "abc");

    lista_inserir_inicio(l, "z");
    memset(buf, 0, sizeof(buf));
    lista_percorrer(l, concatenar, buf);
    ASSERT_EQ_STR(buf, "zabc");
    ASSERT_EQ_INT(lista_tamanho(l), 4);

    char *removido = (char *)lista_remover_primeiro(l);
    ASSERT_EQ_STR(removido, "z");
    ASSERT_EQ_INT(lista_tamanho(l), 3);

    lista_destruir(l, NULL);

    Lista *vazia = lista_criar();
    ASSERT_TRUE(lista_remover_primeiro(vazia) == NULL);
    lista_destruir(vazia, NULL);

    TEST_RESUMO();
}
