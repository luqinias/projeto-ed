#include "test_utils.h"
#include "../src/fila.h"

int main(void) {
    Fila *f = fila_criar();
    ASSERT_TRUE(fila_vazia(f));

    int valor;
    ASSERT_EQ_INT(fila_desenfileirar(f, &valor), 0);

    fila_enfileirar(f, 1);
    fila_enfileirar(f, 2);
    fila_enfileirar(f, 3);
    ASSERT_TRUE(!fila_vazia(f));

    ASSERT_EQ_INT(fila_desenfileirar(f, &valor), 1);
    ASSERT_EQ_INT(valor, 1);
    ASSERT_EQ_INT(fila_desenfileirar(f, &valor), 1);
    ASSERT_EQ_INT(valor, 2);
    ASSERT_EQ_INT(fila_desenfileirar(f, &valor), 1);
    ASSERT_EQ_INT(valor, 3);

    ASSERT_TRUE(fila_vazia(f));
    ASSERT_EQ_INT(fila_desenfileirar(f, &valor), 0);

    fila_destruir(f);

    TEST_RESUMO();
}
