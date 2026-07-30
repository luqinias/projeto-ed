#include "test_utils.h"
#include "../src/avl.h"
#include <math.h>

int main(void) {
    AVL *arv = avl_criar();
    ASSERT_TRUE(avl_buscar(arv, 1) == NULL);

    /* insere em ordem crescente: uma BST comum degeneraria em lista (altura = n).
     * numa AVL balanceada, a altura de n=7 nos deve ficar em torno de log2(7)~=2.8 */
    int valores[7] = {1, 2, 3, 4, 5, 6, 7};
    for (int i = 0; i < 7; i++) {
        avl_inserir(arv, valores[i], &valores[i]);
        int altura = avl_altura(arv);
        int n = i + 1;
        double limite = 1.44 * log2(n + 2) + 1;
        ASSERT_TRUE((double)altura <= limite);
    }

    for (int i = 0; i < 7; i++) {
        void *dado = avl_buscar(arv, valores[i]);
        ASSERT_TRUE(dado == &valores[i]);
    }
    ASSERT_TRUE(avl_buscar(arv, 999) == NULL);

    avl_destruir(arv, NULL);

    TEST_RESUMO();
}
