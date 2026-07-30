# Rede Social Simplificada — Projeto Final (Tema 04)

Estruturas de Dados — Linguagem C. UFU, Prof. Camila Davi Ramos, 2026/1.

Implementação em C11 puro (apenas biblioteca padrão) das 10 funcionalidades
obrigatórias de uma rede social simplificada, usando Árvore (AVL), Tabela
Hash, Lista Encadeada, Fila, Pilha e Grafo como TADs independentes.

## Requisitos

- `gcc` (ou outro compilador C11) e `make`.
- Nenhuma dependência externa além da libc padrão.

## Como compilar

```bash
make
```

Gera o binário em `bin/rede_social`.

## Como executar

```bash
./bin/rede_social
```

Menu interativo com uma opção por funcionalidade obrigatória (cadastro,
busca por nome, amizade, remoção de amizade, publicação, exibir publicações,
histórico, exibir amigos, verificar conexão, sugestão de amizades) + sair.

Os dados são gravados em `rede_social.dat` (raiz do projeto) ao sair e
recarregados automaticamente na próxima execução. Se o arquivo não existir
ainda, o sistema simplesmente começa vazio.

## Como rodar os testes

```bash
make test          # testes unitarios de cada TAD + teste de integracao da fachada
bash tests/smoke_test.sh   # smoke test de persistencia entre execucoes
```

`tests/casos_teste.md` traz um roteiro manual com 1 caso de teste por
funcionalidade obrigatória (cenário, entrada, execução, resultado
esperado/obtido).

```bash
make clean   # remove bin/ e obj/
```

## Popular com dados de demonstração (seed)

```bash
bash tools/seed.sh
```

Cadastra 10 usuários, 8 amizades e 4 publicações através do próprio binário
(ou seja, passando pelas mesmas validações de negócio, não escrevendo o
arquivo direto). Se já existir um `rede_social.dat`, ele é renomeado para
`rede_social.dat.bak.<timestamp>` antes de recriar — nenhum dado é perdido.

O cenário foi desenhado para exercitar todas as funcionalidades de uma vez:
um "hub" de amigos em comum (bom para ver a sugestão de amizades
ordenada), um par de usuários isolado do resto da rede (para "verificar
conexão" retornar não conectados), um usuário sem amigos nem publicações
(casos vazios) e dois usuários com o mesmo nome (para a busca por nome
retornar mais de um resultado). O script imprime ao final a lista de ids e
sugestões de o que testar. Depois é só rodar `./bin/rede_social`.

## Estrutura do projeto

```
projeto-ed/
├── Makefile
├── README.md
├── src/
│   ├── main.c            # menu CLI (so I/O; nao contem regra de negocio)
│   ├── usuario.h/.c       # struct Usuario (id, nome, login, publicacoes)
│   ├── avl.h/.c           # Arvore AVL
│   ├── hash_tabela.h/.c   # Tabela Hash generica (encadeamento + rehash)
│   ├── lista.h/.c         # Lista encadeada generica
│   ├── fila.h/.c          # Fila (FIFO)
│   ├── pilha.h/.c         # Pilha (LIFO)
│   ├── grafo.h/.c         # Grafo de amizades
│   ├── persistencia.h/.c  # Leitura/escrita do arquivo de dados
│   └── rede_social.h/.c   # Fachada: liga os TADs e implementa as regras
├── tests/
│   ├── test_utils.h                          # macros de asserção
│   ├── test_lista.c, test_fila.c, test_pilha.c,
│   │   test_avl.c, test_hash_tabela.c, test_grafo.c,
│   │   test_rede_social.c                    # testes unitarios/integracao
│   ├── smoke_test.sh                         # smoke test de persistencia
│   └── casos_teste.md                        # roteiro de testes manuais
├── docs/
│   ├── superpowers/
│   │   ├── specs/2026-07-30-rede-social-design.md  # design detalhado
│   │   └── plans/2026-07-30-rede-social-plan.md    # plano de implementacao
│   └── relatorio│       
│       └── relatorio.pdf   # relatorio 
└── Enunciado_ProjetoFinal_Tema4.pdf
```

## Estruturas de dados exigidas → onde estão

| Estrutura | Arquivo | Uso no sistema |
|---|---|---|
| Árvore (AVL) | `src/avl.h/.c` | Índice `id -> Usuario*`, busca O(log n) real (balanceada) |
| Tabela Hash | `src/hash_tabela.h/.c` | Índice por `login` (único) e por `nome` (busca de usuário), O(1) médio, com rehash dinâmico |
| Lista encadeada | `src/lista.h/.c` | Publicações de cada usuário; também usada nas listas de adjacência do grafo |
| Fila | `src/fila.h/.c` | BFS do grafo (conectividade e sugestão de amizades) |
| Pilha | `src/pilha.h/.c` | Histórico das N=10 atividades mais recentes |
| Grafo | `src/grafo.h/.c` | Rede de amizades (não direcionado) |

Detalhes de projeto, decisões e justificativas em
`docs/superpowers/specs/2026-07-30-rede-social-design.md`.

## Limitações conhecidas

- Nome e login de usuário não podem conter o caractere `;` (delimitador do
  formato de persistência); o cadastro valida e rejeita isso com mensagem
  clara. Texto de publicação pode conter `;` livremente.
- Não há edição/remoção de usuário nem de publicação (não exigido pelo
  enunciado).
