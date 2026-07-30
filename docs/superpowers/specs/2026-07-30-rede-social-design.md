# Design — Rede Social Simplificada (Projeto Final, Tema 04)

Data: 2026-07-30
Disciplina: Estruturas de Dados — Linguagem C (UFU, Prof. Camila Davi Ramos)

## 1. Objetivo

Implementar em C puro (sem dependências externas além da libc) um sistema de rede
social simplificada, via CLI de menu, atendendo às 10 funcionalidades obrigatórias e
aos 3 requisitos não funcionais do enunciado (`Enunciado_ProjetoFinal_Tema4.pdf`),
usando obrigatoriamente: Árvore, Tabela Hash, Lista Encadeada, Fila, Pilha e Grafo,
cada um como TAD independente (`.h`/`.c`).

## 2. Decisões de projeto (confirmadas com o usuário)

| Decisão | Escolha |
|---|---|
| Árvore de indexação por ID | **AVL balanceada** (garante O(log n) real, não apenas médio) |
| Atribuição de ID | **Automática e sequencial**, gerada pelo sistema no cadastro |
| Tamanho do histórico (N) | **N = 10** |
| Formato de persistência | **Texto simples** (seções delimitadas, um registro por linha, campos separados por `;`) |

Como não é necessário remover usuários (o enunciado só pede remover *amizade*), a
AVL só precisa de `inserir` e `buscar` — não precisa de remoção de nó.

## 3. Estruturas de dados e uso obrigatório

| TAD | Arquivo | Papel no sistema |
|---|---|---|
| Árvore (AVL) | `avl.h/.c` | Índice `id -> Usuario*`, busca O(log n) |
| Tabela Hash | `hash_tabela.h/.c` | Genérica (string -> void*), com encadeamento e *rehash* dinâmico ao ultrapassar fator de carga 0.75. Duas instâncias: uma por `login` (chave única, valida duplicidade) e uma por `nome` (chave não única, cada bucket guarda lista de usuários) |
| Lista encadeada | `lista.h/.c` | Genérica (void* + destrutor). Usada para (a) publicações de cada usuário e (b) listas de adjacência do grafo |
| Fila | `fila.h/.c` | FIFO de inteiros (ids), usada pela BFS do grafo |
| Pilha | `pilha.h/.c` | Pilha de atividades (`Atividade`), usada para o histórico — topo = mais recente |
| Grafo | `grafo.h/.c` | Rede de amizades: vértices = ids de usuário, arestas não direcionadas. `adicionar_aresta`, `remover_aresta`, `bfs` (conectividade e sugestões) |

O Grafo é desacoplado da struct `Usuario`: opera só sobre inteiros (ids), o que o
torna um TAD independente de verdade (não "decorado" dentro de `Usuario`).

## 4. Módulo de fachada

`rede_social.h/.c` concentra o estado do sistema (AVL, as duas hash tables, o grafo,
a pilha de histórico, contador de próximo id) e expõe uma função por funcionalidade
obrigatória (cadastrar_usuario, buscar_por_nome, adicionar_amizade, remover_amizade,
criar_publicacao, exibir_publicacoes, exibir_historico, exibir_amigos,
verificar_conexao, sugerir_amizades). `main.c` só faz o loop de menu e chama essa
fachada — mantém a lógica de domínio testável sem depender de I/O de terminal.

## 5. Persistência (texto simples)

Um único arquivo `rede_social.dat`, gravado na raiz do projeto (evita depender
de criação de diretórios, que não faz parte da libc padrão em C ISO), com
seções:

```
USUARIOS
id;nome;login
...
AMIZADES
id1;id2
...
PUBLICACOES
id_usuario;texto
...
HISTORICO
tipo;descricao
...
```

- Ao carregar: usuários primeiro (repovoa AVL + hashes + contador de próximo id),
  depois amizades (repovoa grafo, sem gerar novo evento de histórico), depois
  publicações (repovoa listas), depois histórico (as últimas N linhas, na ordem em
  que ocorreram — a última linha do arquivo vira o topo da pilha).
- Ao sair do menu (opção "Sair"): sobrescreve o arquivo por completo com o estado
  atual.
- Se o arquivo não existir na primeira execução, o sistema inicia vazio sem erro.

## 6. Validação e tratamento de erros (não funcional II e III)

- Toda leitura de opção de menu e de inteiros usa `fgets` + `strtol` com repetição
  em caso de entrada inválida — nunca `scanf("%d", ...)` cru, para não deixar lixo
  no buffer nem travar/crashar com entrada não numérica.
- Cadastro: login duplicado é rejeitado com mensagem clara; nome/login vazios são
  rejeitados.
- Busca por nome: sem resultado → mensagem informativa (não é erro fatal).
- Amizade: rejeita auto-amizade, id inexistente, amizade duplicada (mensagem
  específica) e remoção de amizade inexistente (mensagem específica) — não
  encerra o programa.
- Publicação: rejeita usuário inexistente e texto vazio.
- Histórico/amigos/publicações vazios: mensagem "nenhum registro", não é erro.

## 7. Sugestão de amizades

Para o usuário consultado U:
1. BFS a partir de U até profundidade 2 usando a Fila.
2. Candidatos = vértices a distância 2 de U, excluindo U e os amigos diretos de U.
3. Para cada candidato, conta quantos amigos em comum tem com U (interseção das
   listas de adjacência).
4. Ordena por (a) número de amigos em comum decrescente, (b) nome em ordem
   alfabética em caso de empate.
5. Retorna no máximo 3 sugestões.

## 8. Estrutura de diretórios

```
projeto-ed/
├── Makefile
├── README.md
├── src/
│   ├── main.c
│   ├── usuario.h / usuario.c
│   ├── avl.h / avl.c
│   ├── hash_tabela.h / hash_tabela.c
│   ├── lista.h / lista.c
│   ├── fila.h / fila.c
│   ├── pilha.h / pilha.c
│   ├── grafo.h / grafo.c
│   ├── persistencia.h / persistencia.c
│   └── rede_social.h / rede_social.c
├── tests/
│   ├── test_utils.h
│   ├── test_avl.c, test_hash_tabela.c, test_lista.c, test_fila.c, test_pilha.c, test_grafo.c
│   └── casos_teste.md  (roteiro manual, 1 caso por funcionalidade obrigatória)
└── rede_social.dat  (gerado em runtime, ignorado no relatório)
```

## 9. Estratégia de testes

- Testes unitários (TDD) por TAD em `tests/`, cada um um executável próprio
  compilado pelo Makefile (`make test`), usando macros de asserção simples
  (`tests/test_utils.h`), sem framework externo.
- `tests/casos_teste.md`: roteiro com 1 caso de teste por funcionalidade
  obrigatória (cenário, entrada, execução, resultado esperado/obtido) — insumo
  direto para a seção "Resultados" do relatório e para a apresentação.

## 10. Fora de escopo

- Autenticação/senha (não pedido).
- Edição/remoção de usuário (não pedido).
- Edição/remoção de publicação (não pedido).
- Interface gráfica (CLI de menu é suficiente e é o padrão esperado na disciplina).
