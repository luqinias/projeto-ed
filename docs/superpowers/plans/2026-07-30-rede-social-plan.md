# Rede Social Simplificada — Plano de Implementação

> Execução: inline, nesta mesma sessão (sem subagents), seguindo TDD por TAD.
> Spec: `docs/superpowers/specs/2026-07-30-rede-social-design.md`

**Goal:** Sistema em C (padrão C11, só libc) que implementa as 10 funcionalidades
obrigatórias do Tema 04 usando AVL, Tabela Hash, Lista Encadeada, Fila, Pilha e
Grafo como TADs independentes, com persistência em texto e tratamento de erros
sem crash.

**Architecture:** Cada TAD é um par `.h/.c` sem dependências entre TADs de baixo
nível (exceto Grafo, que usa Fila e Lista internamente). Um módulo de fachada
(`rede_social`) compõe os TADs e implementa as regras de negócio. `main.c` é só
menu + I/O.

**Tech Stack:** C11, gcc, make, sem bibliotecas externas.

## Global Constraints

- Sem dependências além da libc padrão (requisito de entrega I).
- Toda estrutura da Tabela 1 deve ser usada de forma funcional, não decorativa.
- Nenhuma operação pode encerrar o programa abruptamente com entrada inválida.
- Persistência: grava ao sair, recarrega automaticamente ao iniciar.
- Histórico: N=10, mais recente primeiro.

---

## Task 1: Scaffolding (Makefile + harness de teste)

**Files:**
- Create: `Makefile`
- Create: `tests/test_utils.h`
- Create: `.gitignore`

**Interfaces:**
- Produz macros `ASSERT_TRUE(cond)`, `ASSERT_EQ_INT(a,b)`, `ASSERT_EQ_STR(a,b)`
  em `test_utils.h`, cada uma imprimindo PASS/FAIL e incrementando contadores
  globais `testes_ok`/`testes_falhos` (extern int, definidos em cada `main` de
  teste).
- Makefile: `make` compila `bin/rede_social` a partir de `src/*.c`; `make test`
  compila e roda cada `tests/test_*.c` linkado com o `.c` do TAD correspondente;
  `make clean` remove `bin/` e `obj/`.

- [ ] Escrever `tests/test_utils.h` com as macros de asserção.
- [ ] Escrever `Makefile` com alvos `all`, `test`, `clean`, flags `-Wall -Wextra -std=c11 -Isrc`.
- [ ] Escrever `.gitignore` (`bin/`, `obj/`, `data/*.dat`).
- [ ] Rodar `make` (deve falhar/no-op pois não há `src/main.c` ainda — só validar que o Makefile parseia).

## Task 2: Lista (Lista Encadeada genérica)

**Files:**
- Create: `src/lista.h`, `src/lista.c`
- Test: `tests/test_lista.c`

**Interfaces:**
```c
typedef struct NoLista NoLista;
typedef struct Lista Lista;

Lista *lista_criar(void);
void lista_inserir_inicio(Lista *l, void *dado);
void lista_inserir_fim(Lista *l, void *dado);
void *lista_remover_primeiro(Lista *l); /* retorna o dado removido, ou NULL se vazia */
int lista_tamanho(const Lista *l);
int lista_vazia(const Lista *l);
void lista_percorrer(const Lista *l, void (*visitar)(void *dado, void *ctx), void *ctx);
void lista_destruir(Lista *l, void (*destruir_dado)(void *dado));
```
- Produz: `Lista`/`NoLista` opacos (implementação em `lista.c`, só forward decl no `.h`).
- Consumido depois por: `usuario.c` (publicações) e `grafo.c` (adjacência, guardando ids via cast `(void*)(intptr_t)id`).

- [ ] Escrever `tests/test_lista.c`: cria lista, insere 3 elementos no fim, confere tamanho e ordem via `lista_percorrer`; insere no início e confere nova ordem; remove primeiro e confere valor e novo tamanho; destrói lista vazia e lista com elementos (sem vazamento perceptível/sem crash sob valgrind se disponível).
- [ ] Rodar `gcc -Isrc tests/test_lista.c src/lista.c -o /tmp/test_lista` (deve falhar: `lista.c` não existe).
- [ ] Implementar `src/lista.h` e `src/lista.c` (lista simplesmente encadeada, `struct Lista { NoLista *inicio, *fim; int tamanho; }`).
- [ ] Rodar o teste novamente e confirmar PASS em todas as asserções.
- [ ] Adicionar alvo `test_lista` ao Makefile e rodar `make test`.

## Task 3: Fila (FIFO de inteiros)

**Files:**
- Create: `src/fila.h`, `src/fila.c`
- Test: `tests/test_fila.c`

**Interfaces:**
```c
typedef struct Fila Fila;

Fila *fila_criar(void);
void fila_enfileirar(Fila *f, int valor);
int fila_desenfileirar(Fila *f, int *valor_saida); /* retorna 0 se vazia, 1 se ok */
int fila_vazia(const Fila *f);
void fila_destruir(Fila *f);
```
- Consumido por: `grafo.c` (BFS).

- [ ] Escrever `tests/test_fila.c`: enfileira 1,2,3; desenfileira e confere ordem FIFO; confere `fila_vazia` antes/depois; desenfileirar fila vazia retorna 0.
- [ ] Rodar teste (deve falhar por falta de `fila.c`).
- [ ] Implementar `src/fila.h`/`src/fila.c` (encadeada, ponteiros `inicio`/`fim`).
- [ ] Rodar teste e confirmar PASS.

## Task 4: Pilha (histórico de atividades)

**Files:**
- Create: `src/pilha.h`, `src/pilha.c`
- Test: `tests/test_pilha.c`

**Interfaces:**
```c
typedef enum { ATIV_NOVO_USUARIO, ATIV_NOVA_AMIZADE, ATIV_REMOCAO_AMIZADE, ATIV_NOVA_PUBLICACAO } TipoAtividade;

typedef struct {
    TipoAtividade tipo;
    char descricao[160];
} Atividade;

typedef struct Pilha Pilha;

Pilha *pilha_criar(int capacidade_max); /* capacidade_max = N do histórico, ex. 10 */
void pilha_empilhar(Pilha *p, Atividade a); /* se exceder capacidade_max, descarta o mais antigo */
int pilha_vazia(const Pilha *p);
int pilha_tamanho(const Pilha *p);
/* itera do topo (mais recente) para a base (mais antiga) */
void pilha_percorrer(const Pilha *p, void (*visitar)(const Atividade *a, void *ctx), void *ctx);
void pilha_destruir(Pilha *p);
```
- Consumido por: `rede_social.c` (registra evento em toda operação de escrita) e `persistencia.c` (grava/recarrega histórico).

- [ ] Escrever `tests/test_pilha.c`: empilha 3 atividades, confere `pilha_percorrer` retorna na ordem inversa (mais recente primeiro); empilha mais que `capacidade_max` e confere que o tamanho não ultrapassa e que o item mais antigo foi descartado.
- [ ] Rodar teste (falha esperada).
- [ ] Implementar `src/pilha.h`/`src/pilha.c` como lista encadeada própria (não reusa `Lista` genérica, pois carrega `Atividade` por valor e precisa de limite de capacidade).
- [ ] Rodar teste e confirmar PASS.

## Task 5: AVL (índice por ID)

**Files:**
- Create: `src/avl.h`, `src/avl.c`
- Test: `tests/test_avl.c`

**Interfaces:**
```c
typedef struct NoAVL NoAVL;
typedef struct AVL AVL;

AVL *avl_criar(void);
void avl_inserir(AVL *arv, int id, void *dado); /* dado = Usuario* */
void *avl_buscar(const AVL *arv, int id); /* NULL se não encontrado */
void avl_percorrer_em_ordem(const AVL *arv, void (*visitar)(void *dado, void *ctx), void *ctx);
void avl_destruir(AVL *arv, void (*destruir_dado)(void *dado));
```
- Sem remoção de nó (não é funcionalidade exigida).
- Consumido por: `rede_social.c` (índice principal por id).

- [ ] Escrever `tests/test_avl.c`: insere ids em ordem crescente 1..7 e confere, após cada inserção, que a altura da árvore nunca ultrapassa `~1.44*log2(n+2)` (checagem de balanceamento indireta) — expor função auxiliar `avl_altura(const AVL*)` só para teste; busca por id existente e inexistente.
- [ ] Rodar teste (falha esperada).
- [ ] Implementar rotações simples/duplas (LL, RR, LR, RL) e inserção com rebalanceamento clássico.
- [ ] Rodar teste e confirmar PASS, inclusive o caso de inserção em ordem (prova que balanceia).

## Task 6: Tabela Hash genérica

**Files:**
- Create: `src/hash_tabela.h`, `src/hash_tabela.c`
- Test: `tests/test_hash_tabela.c`

**Interfaces:**
```c
typedef struct HashTabela HashTabela;

HashTabela *hash_criar(void);
/* permite_duplicata=0: sobrescreve valor de chave já existente (uso: login, único).
   permite_duplicata=1: acumula múltiplos valores por chave numa Lista* (uso: nome). */
void hash_inserir(HashTabela *h, const char *chave, void *valor, int permite_duplicata);
void *hash_buscar(HashTabela *h, const char *chave); /* único valor, ou NULL */
Lista *hash_buscar_todos(HashTabela *h, const char *chave); /* para permite_duplicata=1; NULL se não achar */
int hash_contem(HashTabela *h, const char *chave);
void hash_destruir(HashTabela *h);
```
- Depende de `lista.h` (para o caso `permite_duplicata=1`).
- Consumido por: `rede_social.c` (uma instância por login, uma por nome).

- [ ] Escrever `tests/test_hash_tabela.c`: insere pares únicos, busca e confere valor; insere chave duplicada com `permite_duplicata=0` e confere sobrescrita; insere 3 valores na mesma chave com `permite_duplicata=1` e confere `hash_buscar_todos` retorna lista de tamanho 3; insere >  fator de carga 0.75 do tamanho inicial de baldes e confere que `hash_buscar` ainda encontra todos os itens após o rehash (prova indireta de que o rehash não perde dados).
- [ ] Rodar teste (falha esperada).
- [ ] Implementar tabela com encadeamento separado, função hash djb2, rehash dinâmico (dobra baldes) quando `elementos/baldes > 0.75`.
- [ ] Rodar teste e confirmar PASS.

## Task 7: Grafo (rede de amizades)

**Files:**
- Create: `src/grafo.h`, `src/grafo.c`
- Test: `tests/test_grafo.c`

**Interfaces:**
```c
typedef struct Grafo Grafo;

typedef struct {
    int id;
    int amigos_em_comum;
} Sugestao;

Grafo *grafo_criar(void);
void grafo_adicionar_vertice(Grafo *g, int id);
int grafo_adicionar_aresta(Grafo *g, int id1, int id2); /* 0 = já existia (duplicada), 1 = ok */
int grafo_remover_aresta(Grafo *g, int id1, int id2);   /* 0 = não existia, 1 = removida */
int grafo_sao_amigos(const Grafo *g, int id1, int id2);
Lista *grafo_vizinhos(const Grafo *g, int id); /* Lista de ids (void*)(intptr_t), não liberar os dados, só a Lista com lista_destruir(l, NULL) */
int grafo_conectado(const Grafo *g, int origem, int destino); /* BFS; 1 = existe caminho, 0 = não */
/* preenche até max_sugestoes elementos de saida[]; retorna quantos preencheu.
   nomes_por_id é usado só para o critério de desempate alfabético. */
int grafo_sugerir_amigos(const Grafo *g, int origem, int max_sugestoes,
                          const char *(*nome_por_id)(int id, void *ctx), void *ctx,
                          Sugestao *saida);
void grafo_destruir(Grafo *g);
```
- Depende de `fila.h` (BFS) e `lista.h` (adjacência e retorno de `grafo_vizinhos`).
- Consumido por: `rede_social.c` (todas as operações de amizade/conexão/sugestão).

- [ ] Escrever `tests/test_grafo.c` cobrindo: adicionar vértices e aresta simples; `grafo_adicionar_aresta` retorna 0 na segunda tentativa da mesma aresta; `grafo_remover_aresta` retorna 0 se não existia; `grafo_conectado` verdadeiro para vértices ligados direta e indiretamente (cadeia 1-2-3), falso para vértice isolado; `grafo_sugerir_amigos` num grafo em estrela (A amigo de B,C,D; E amigo de B,C) — sugerir para E deve retornar A com 2 amigos em comum, respeitando o limite de 3 e a ordenação por contagem/nome.
- [ ] Rodar teste (falha esperada).
- [ ] Implementar `Grafo` como array dinâmico de `Lista*` (adjacência), indexado por id (realloc quando id excede capacidade); BFS com `Fila` e vetor `visitado` alocado por altura do id máximo.
- [ ] Rodar teste e confirmar PASS.

## Task 8: Usuario

**Files:**
- Create: `src/usuario.h`, `src/usuario.c`
- Test: (coberto pelos testes de integração da fachada, Task 10; não precisa de test file dedicado pois é uma struct simples + construtor/destrutor)

**Interfaces:**
```c
typedef struct {
    int id;
    char nome[100];
    char login[50];
    Lista *publicacoes; /* Lista de char* (texto), inseridos no início = mais recente primeiro */
} Usuario;

Usuario *usuario_criar(int id, const char *nome, const char *login);
void usuario_adicionar_publicacao(Usuario *u, const char *texto);
void usuario_destruir(Usuario *u);
```

- [ ] Implementar `src/usuario.h`/`src/usuario.c` diretamente (struct simples, sem TDD dedicado — validado via Task 10).

## Task 9: Persistência (texto)

**Files:**
- Create: `src/persistencia.h`, `src/persistencia.c`

**Interfaces:**
```c
#include "rede_social.h"

void persistencia_salvar(const RedeSocial *rs, const char *caminho);
void persistencia_carregar(RedeSocial *rs, const char *caminho); /* no-op silencioso se arquivo não existir */
```
- Depende de `rede_social.h` (Task 10) — implementar **depois** da Task 10.
- Formato: seções `USUARIOS` / `AMIZADES` / `PUBLICACOES` / `HISTORICO` conforme spec §5.

- [ ] Implementar `persistencia_salvar`: percorre AVL em ordem (usuários), depois grafo (arestas, evitando duplicar A-B/B-A com `id1<id2`), depois publicações de cada usuário, depois pilha de histórico (percorrida do fim para o topo, i.e. mais antiga primeiro no arquivo).
- [ ] Implementar `persistencia_carregar`: lê seção a seção com `fgets`+`strtok`; usa as funções internas de `rede_social.c` marcadas para não duplicar histórico durante a carga de amizades/usuários (ver Task 10, `*_silencioso`).
- [ ] Teste manual: rodar o binário, cadastrar 2 usuários + 1 amizade + 1 publicação, sair, rodar de novo, confirmar via menu que os dados voltaram (parte do smoke test da Task 12).

## Task 10: Rede Social (fachada)

**Files:**
- Create: `src/rede_social.h`, `src/rede_social.c`

**Interfaces:**
```c
typedef struct RedeSocial RedeSocial;

RedeSocial *rede_social_criar(void);

/* retorna id gerado, ou -1 se login duplicado/nome-login vazio (mensagem em *erro) */
int rs_cadastrar_usuario(RedeSocial *rs, const char *nome, const char *login, char *erro, size_t erro_tam);
Lista *rs_buscar_por_nome(RedeSocial *rs, const char *nome); /* Lista de Usuario*, não destruir os dados */
int rs_adicionar_amizade(RedeSocial *rs, int id1, int id2, char *erro, size_t erro_tam); /* 1 ok / 0 erro */
int rs_remover_amizade(RedeSocial *rs, int id1, int id2, char *erro, size_t erro_tam);
int rs_criar_publicacao(RedeSocial *rs, int id, const char *texto, char *erro, size_t erro_tam);
Usuario *rs_buscar_por_id(RedeSocial *rs, int id); /* usado pelo menu para validar ids digitados */
Lista *rs_exibir_amigos(RedeSocial *rs, int id); /* Lista de Usuario* */
int rs_verificar_conexao(RedeSocial *rs, int id1, int id2);
int rs_sugerir_amizades(RedeSocial *rs, int id, Sugestao *saida); /* delega a grafo_sugerir_amigos */
void rs_exibir_historico(const RedeSocial *rs, void (*visitar)(const Atividade *a, void *ctx), void *ctx);

/* variantes _silencioso (não empilham Atividade) usadas só por persistencia_carregar */
int rs_cadastrar_usuario_silencioso(RedeSocial *rs, int id, const char *nome, const char *login);
int rs_adicionar_amizade_silenciosa(RedeSocial *rs, int id1, int id2);
int rs_criar_publicacao_silenciosa(RedeSocial *rs, int id, const char *texto);
void rs_empilhar_historico_bruto(RedeSocial *rs, Atividade a); /* usado ao carregar HISTORICO do arquivo */

void rs_destruir(RedeSocial *rs);
```
- Depende de: `avl.h`, `hash_tabela.h`, `grafo.h`, `pilha.h`, `usuario.h`, `lista.h`.
- Internamente mantém `int proximo_id` (começa em 1, incrementa a cada cadastro real ou carregado).

- [ ] Implementar struct `RedeSocial { AVL *por_id; HashTabela *por_login; HashTabela *por_nome; Grafo *grafo; Pilha *historico; int proximo_id; }`.
- [ ] Implementar cada função de negócio, reaproveitando as validações descritas na spec §6 (mensagens em `erro`/`erro_tam` em vez de `printf` direto, para manter a fachada testável e desacoplada de I/O).
- [ ] Escrever teste de integração leve `tests/test_rede_social.c`: cadastra 2 usuários, tenta duplicar login (deve falhar com id -1), cria amizade, tenta duplicar amizade (deve retornar 0), cria publicação, verifica `rs_exibir_historico` traz 4 eventos com o mais recente (publicação) primeiro.
- [ ] Rodar teste e confirmar PASS.

## Task 11: main.c (menu CLI)

**Files:**
- Create: `src/main.c`

**Interfaces:** nenhuma nova — só consome `rede_social.h` e `persistencia.h`.

- [ ] Implementar loop de menu com as opções 1–10 (uma por funcionalidade obrigatória) + 0 (Sair).
- [ ] Implementar leitura robusta de linha (`ler_linha(char *buf, size_t tam)` usando `fgets` + remoção do `\n`) e de inteiro (`ler_inteiro(const char *prompt)` com `strtol` e repetição em caso de erro) dentro do próprio `main.c` (funções `static`).
- [ ] Ao iniciar: `persistencia_carregar(rs, "rede_social.dat")`. Ao escolher "Sair": `persistencia_salvar(...)` antes de `rs_destruir` e `return 0`.
- [ ] Rodar `make` e testar manualmente cada opção do menu uma vez (cadastro, busca, amizade, remoção, publicação, exibir publicações, histórico, amigos, conexão, sugestão, sair) confirmando que nenhuma opção crasha com entrada inválida (testar letra no lugar de número, id inexistente, nome inexistente).

## Task 12: Casos de teste e smoke test de persistência

**Files:**
- Create: `tests/casos_teste.md`
- Create: `tests/smoke_test.sh`

- [ ] Escrever `tests/casos_teste.md` com 1 caso por funcionalidade obrigatória (cenário, entrada, execução, resultado esperado, resultado obtido) — reaproveitável no relatório.
- [ ] Escrever `tests/smoke_test.sh`: roda o binário duas vezes via `printf ... | ./bin/rede_social` (primeira execução cadastra dados e sai; segunda execução confirma, via grep na saída, que os dados persistiram) e retorna código de saída não-zero se algo não bater.
- [ ] Rodar `bash tests/smoke_test.sh` e confirmar sucesso.

## Task 13: README e build final

**Files:**
- Create: `README.md`

- [ ] Documentar em `README.md`: requisitos (gcc, make), como compilar (`make`), como rodar (`./bin/rede_social`), como rodar os testes (`make test`, `bash tests/smoke_test.sh`), estrutura de diretórios, mapeamento estrutura→arquivo (tabela igual à da spec §3).
- [ ] Rodar `make clean && make && make test && bash tests/smoke_test.sh` do zero e confirmar que tudo passa sem warnings (`-Wall -Wextra`).

---

## Self-review (spec coverage)

- Cadastro/Busca/Amizade/Remoção/Publicação/Exibir publicações/Histórico/Exibir
  amigos/Verificar conexão/Sugestão → Task 10 + Task 11 (uma função de fachada +
  uma opção de menu cada).
- Persistência → Task 9.
- Validação sem crash → Task 11 (leitura robusta) + mensagens de erro em cada
  função da Task 10.
- Amizade duplicada / remoção inexistente → `grafo_adicionar_aresta` /
  `grafo_remover_aresta` (Task 7) retornam código específico, propagado pela
  Task 10.
- Árvore/Hash/Lista/Fila/Pilha/Grafo, cada um `.h`/`.c` independente → Tasks 2–7.
