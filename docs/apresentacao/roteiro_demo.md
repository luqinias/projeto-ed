# Roteiro de Demonstração ao Vivo — Rede Social Simplificada

Roteiro para a apresentação em laboratório (Projeto Final, Tema 04 — Estruturas
de Dados). Guia passo a passo do que digitar, o que vai aparecer na tela e o que
falar, conectando cada ação às estruturas de dados exigidas pelo enunciado.

Toda a saída abaixo foi capturada rodando o binário de verdade (`bin/rede_social`),
não é texto inventado — pode divergir só em ids se vocês rodarem em outra ordem.

## 0. Preparação (antes de entrar no laboratório)

```bash
make clean && make        # build limpo, zero warnings com -Wall -Wextra
make test                 # 290 asserções, 7 binários, todas passando
bash tests/smoke_test.sh  # confirma persistência entre execuções
bash tools/seed.sh        # popula rede_social.dat com o cenário de demo
```

O `seed.sh` cadastra 10 usuários, 8 amizades e 4 publicações **através do próprio
binário** (passa pelas mesmas validações de produção, não escreve o arquivo na
mão). O cenário foi desenhado para acionar todas as funcionalidades:

| id | nome | login | observação |
|---|---|---|---|
| 1 | Ana Souza | ana | "hub" — amiga de Bruno, Carla e Diego |
| 2 | Bruno Lima | bruno | |
| 3 | Carla Dias | carla | |
| 4 | Diego Alves | diego | |
| 5 | Elisa Ramos | elisa | amiga de Bruno e Carla (não de Ana diretamente) |
| 6 | Fabio Nunes | fabio | |
| 7 | Gabriela Reis | gabriela | par isolado com Heitor |
| 8 | Heitor Costa | heitor | par isolado com Gabriela |
| 9 | Igor Teixeira | igor | sem amigos, sem publicações (caso vazio) |
| 10 | Ana Souza | ana2 | nome duplicado de propósito (busca por nome) |

Depois de rodar o seed, abram o programa uma única vez e deixem a janela do
terminal pronta:

```bash
./bin/rede_social
```

Vocês vão ver:

```
Rede Social Simplificada — dados carregados de "rede_social.dat" (se existente).

===== Rede Social Simplificada =====
 1. Cadastrar usuario
 2. Buscar usuario por nome
 3. Adicionar amizade
 4. Remover amizade
 5. Criar publicacao
 6. Exibir publicacoes de um usuario
 7. Exibir historico de atividades
 8. Exibir amigos de um usuario
 9. Verificar conexao entre dois usuarios
10. Sugerir amizades
 0. Sair
Opcao:
```

**Fala de abertura:** "O menu tem uma opção por funcionalidade obrigatória. Por
trás de cada uma tem uma estrutura de dados diferente fazendo o trabalho pesado
— vamos mostrar isso na prática, não só declarado."

---

## 1. Busca por nome → Tabela Hash

**Digitar:** `2`, depois `Ana Souza`

**Sai:**
```
Usuarios encontrados:
  - id 1 | nome: Ana Souza | login: ana
  - id 10 | nome: Ana Souza | login: ana2
```

**Falar:** a busca não percorre a lista de usuários uma por uma — ela vai direto
no bucket da Tabela Hash indexada por **nome** (chave não-única: cada bucket
guarda uma lista dos usuários com aquele nome). Por isso retorna os dois "Ana
Souza" de uma vez, em tempo O(1) médio. É a mesma tabela hash (outra instância)
que indexa por **login**, aí como chave única, para bloquear cadastro duplicado.

---

## 2. Exibir amigos → Grafo

**Digitar:** `8`, depois `1`

**Sai:**
```
Amigos de Ana Souza:
  - id 2 | nome: Bruno Lima | login: bruno
  - id 3 | nome: Carla Dias | login: carla
  - id 4 | nome: Diego Alves | login: diego
```

**Falar:** aqui é o **Grafo** de amizades (não-dirigido, lista de adjacência
indexada por id) devolvendo os vizinhos diretos do vértice 1. Repare que a
resposta veio com objetos `Usuario` completos, não só ids — o grafo guarda só
inteiros, e a fachada (`rede_social.c`) resolve cada id de volta a um usuário
via a **Árvore AVL**. É a integração AVL + Grafo acontecendo em uma única
chamada.

---

## 3. Verificar conexão → BFS com Fila (dois casos: conectado e não-conectado)

**Digitar:** `9`, depois `1`, depois `7`

**Sai:**
```
Os usuarios 1 e 7 NAO estao conectados na rede.
```

**Digitar:** `9`, depois `1`, depois `2`

**Sai:**
```
Os usuarios 1 e 2 estao conectados na rede.
```

**Falar:** Gabriela (7) e Heitor (8) formam um par isolado do resto da rede de
propósito, pra mostrar o caso "não conectados". Por trás dos dois resultados
tem a mesma busca em largura (BFS) usando a **Fila** (FIFO) de verdade — não é
um array simulando fila. A BFS visita os vizinhos nível por nível a partir do
vértice 1 até encontrar (ou não) o vértice alvo.

---

## 4. Sugestão de amizades → BFS (Fila) + ordenação

**Digitar:** `10`, depois `5`

**Sai:**
```
Sugestoes de amizade para Elisa Ramos:
  - Ana Souza (id 1) | 2 amigo(s) em comum
  - Fabio Nunes (id 6) | 1 amigo(s) em comum
```

**Falar:** essa é a funcionalidade mais rica em integração. A mesma **Fila**
usada na conectividade roda uma BFS a partir de Elisa até distância 2
(amigos de amigos), excluindo Elisa e seus amigos diretos. Para cada
candidato, conta amigos em comum comparando listas de adjacência do
**Grafo**, depois ordena por número de amigos em comum decrescente — em
empate, por nome alfabético — e corta em no máximo 3 (o enunciado pede exatamente
isso). Ana tem 2 amigos em comum com Elisa (Bruno e Carla); Fabio tem 1 — por
isso essa ordem.

---

## 5. Publicações — caso vazio → Lista Encadeada

**Digitar:** `6`, depois `9`

**Sai:**
```
Igor Teixeira ainda nao fez nenhuma publicacao.
```

**Falar:** Igor foi cadastrado de propósito sem amigos nem publicações, pra
mostrar que o sistema trata a lista vazia sem erro — cada usuário carrega sua
própria **Lista Encadeada** de publicações (`Usuario.publicacoes`), e aqui ela
está vazia mesmo.

---

## 6. Histórico → Pilha (mostra as 10 mais recentes de 22 eventos)

**Digitar:** `7`

**Sai (mais recente primeiro):**
```
Historico de atividades (mais recente primeiro):
  [Nova publicacao] Nova publicacao de Elisa Ramos (id 5)
  [Nova publicacao] Nova publicacao de Bruno Lima (id 2)
  [Nova publicacao] Nova publicacao de Ana Souza (id 1)
  [Nova publicacao] Nova publicacao de Ana Souza (id 1)
  [Nova amizade] Nova amizade: Gabriela Reis (id 7) e Heitor Costa (id 8)
  [Nova amizade] Nova amizade: Carla Dias (id 3) e Fabio Nunes (id 6)
  [Nova amizade] Nova amizade: Diego Alves (id 4) e Fabio Nunes (id 6)
  [Nova amizade] Nova amizade: Carla Dias (id 3) e Elisa Ramos (id 5)
  [Nova amizade] Nova amizade: Bruno Lima (id 2) e Elisa Ramos (id 5)
  [Nova amizade] Nova amizade: Ana Souza (id 1) e Diego Alves (id 4)
```

**Falar:** o seed gerou 22 eventos (10 cadastros + 8 amizades + 4 publicações),
mas o histórico só guarda as **N = 10** mais recentes — é uma **Pilha** de
capacidade fixa que descarta o evento mais antigo a cada novo empilhamento
acima da capacidade, e percorre do topo pra base (mais recente → mais antigo).
Os 10 primeiros cadastros de usuário já saíram da pilha; só sobrou o rastro das
amizades e publicações mais recentes.

---

## 7. Agora ao vivo: cadastro, amizade, remoção e publicação em tempo real

Essa parte mostra as estruturas sendo **atualizadas ao vivo**, não só
consultadas.

**Cadastrar (Árvore + Tabela Hash + Pilha):**
Digitar: `1`, depois `Julia Prado`, depois `julia`
```
Usuario cadastrado com sucesso! id = 11
```
Falar: id 11 gerado automaticamente e sequencial; nesse único cadastro, a Julia
entra na AVL (índice por id), nas duas tabelas hash (login e nome) e um evento
"Novo usuario" é empilhado no histórico.

**Amizade nova (Grafo):**
Digitar: `3`, depois `11`, depois `1`
```
Amizade criada com sucesso.
```

**Tentar de novo, pra mostrar a validação (opcional mas recomendado):**
Digitar: `3`, depois `11`, depois `1` de novo
```
Erro: Esses usuarios ja sao amigos.
```
Falar: o Grafo rejeita aresta duplicada — é o requisito não funcional III.

**Remover a amizade (Grafo):**
Digitar: `4`, depois `11`, depois `1`
```
Amizade removida com sucesso.
```

**Tentar remover de novo (validação):**
Digitar: `4`, depois `11`, depois `1` de novo
```
Erro: Esses usuarios nao sao amigos.
```

**Nova publicação (Lista Encadeada):**
Digitar: `5`, depois `11`, depois `Oi pessoal, sou nova aqui!`
```
Publicacao criada com sucesso.
```

**Histórico de novo, pra fechar (Pilha atualizada):**
Digitar: `7`
```
Historico de atividades (mais recente primeiro):
  [Nova publicacao] Nova publicacao de Julia Prado (id 11)
  [Remocao de amizade] Amizade removida: Julia Prado (id 11) e Ana Souza (id 1)
  [Nova amizade] Nova amizade: Julia Prado (id 11) e Ana Souza (id 1)
  [Novo usuario] Novo usuario cadastrado: Julia Prado (id 11, login julia)
  [Nova publicacao] Nova publicacao de Elisa Ramos (id 5)
  ...
```
Falar: os 4 eventos que acabamos de gerar entraram no topo da pilha, empurrando
os mais antigos pra baixo (e os que passaram de N=10 caíram fora). Isso fecha o
ciclo mostrando as 6 estruturas atuando juntas numa sequência real de uso.

---

## 8. Sair → Persistência

**Digitar:** `0`

**Sai:**
```
Dados salvos em "rede_social.dat". Ate mais!
```

**Falar:** ao sair, o estado inteiro (usuários, amizades, publicações e as
últimas N atividades do histórico) é gravado em `rede_social.dat`. Se a gente
abrir o programa de novo agora, tudo volta exatamente como estava — inclusive
o próximo id continua de 12 em diante, não reseta. Isso é o requisito não
funcional I, e está automatizado em `tests/smoke_test.sh` (roda o binário 3
vezes seguidas e confere que nada se perde).

Pra reforçar sem precisar demonstrar de novo ao vivo (economiza tempo):
"Isso já está coberto por um teste automatizado que roda 3 execuções
sucessivas do programa e confirma que os dados sobrevivem — está em
`tests/smoke_test.sh`, e passa."

---

## 9. Fechamento — testes automatizados

Se sobrar tempo ou a professora pedir mais rigor:

```bash
make test
```

Mostra as 290 asserções passando nos 7 TADs + na fachada de integração,
cobrindo exatamente os casos de erro que apareceram na demo (login duplicado,
amizade duplicada, remoção inexistente, ordenação de sugestões, rebalanceamento
da AVL, rehash da tabela hash).

---

## Apêndice — casos de teste formais (1 por funcionalidade obrigatória)

O roteiro escrito formalmente, no formato cenário/entrada/execução/resultado
esperado/resultado obtido exigido pelo enunciado, já está em
[`tests/casos_teste.md`](../../tests/casos_teste.md) — pode ser impresso ou
anexado como slide de apoio/apêndice da apresentação, sem necessidade de
reescrever.

## Checklist rápido do grupo antes de entrar na sala

- [ ] `make clean && make` rodado, binário atualizado
- [ ] `bash tools/seed.sh` rodado (ou dados já populados do jeito que preferirem)
- [ ] Terminal com fonte grande o suficiente pra quem estiver longe ler
- [ ] Alguém com `tests/casos_teste.md` aberto para consulta rápida se a
      professora pedir para ver um caso específico
- [ ] Slides abertos na primeira página (ver `slides.html`)
