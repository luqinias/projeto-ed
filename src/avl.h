#ifndef AVL_H
#define AVL_H

/*
 * Árvore AVL: índice principal do sistema, chaveado por id (int) -> dado
 * (Usuario*). Garante busca em O(log n) real, pois se auto-balanceia a cada
 * inserção. Não implementa remoção de nó porque o enunciado não exige
 * remover usuários (só remover amizade, que é responsabilidade do Grafo).
 */

typedef struct NoAVL NoAVL;
typedef struct AVL AVL;

AVL *avl_criar(void);

void avl_inserir(AVL *arv, int id, void *dado);

/* Retorna o dado associado ao id, ou NULL se não encontrado. */
void *avl_buscar(const AVL *arv, int id);

void avl_percorrer_em_ordem(const AVL *arv, void (*visitar)(void *dado, void *ctx), void *ctx);

/* Altura da árvore (raiz = altura 1, árvore vazia = altura 0). Exposta
 * principalmente para verificação de balanceamento em testes. */
int avl_altura(const AVL *arv);

/* Se destruir_dado != NULL, é chamado para cada dado antes de liberar os nós. */
void avl_destruir(AVL *arv, void (*destruir_dado)(void *dado));

#endif
