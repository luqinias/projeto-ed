# Casos de Teste — Rede Social Simplificada

Um caso de teste por funcionalidade obrigatória do enunciado (Tema 04), executado
manualmente sobre `./bin/rede_social` a partir de uma base vazia (sem
`rede_social.dat` prévio). Reaproveitável na seção "Resultados" do relatório e
na apresentação.

Convenção: `Opcao:` é o prompt do menu; as linhas abaixo dele são a entrada
digitada pelo usuário para aquela ação.

---

## I. Cadastro de usuário

**Cenário:** cadastrar um novo usuário com nome e login válidos.
**Entrada:**
```
Opcao: 1
Nome: Ana Souza
Login: ana
```
**Execução:** opção 1 do menu.
**Resultado esperado:** sistema gera um id sequencial (1, se for o primeiro
cadastro) e confirma o cadastro.
**Resultado obtido:** `Usuario cadastrado com sucesso! id = 1` — OK.

**Caso de erro (login duplicado):** repetir o cadastro com `Login: ana`.
**Resultado obtido:** `Erro: Ja existe um usuario cadastrado com esse login.`
— sistema não encerra, apenas rejeita o cadastro — OK.

## II. Busca de usuário por nome

**Cenário:** localizar um usuário existente pelo nome.
**Entrada:**
```
Opcao: 2
Nome a buscar: Ana Souza
```
**Resultado esperado:** lista o(s) usuário(s) com esse nome (id, nome, login).
**Resultado obtido:** `id 1 | nome: Ana Souza | login: ana` — OK.

**Caso de erro (nome inexistente):** `Nome a buscar: Ninguem Assim`.
**Resultado obtido:** `Nenhum usuario encontrado com o nome "Ninguem Assim".`
— mensagem clara, sem crash — OK.

## III. Adicionar amizade

**Cenário:** criar uma amizade entre dois usuários cadastrados (ids 1 e 2).
**Entrada:**
```
Opcao: 3
Id do primeiro usuario: 1
Id do segundo usuario: 2
```
**Resultado esperado:** liga os dois usuários no grafo de amizades.
**Resultado obtido:** `Amizade criada com sucesso.` — OK.

**Caso de erro (amizade duplicada):** repetir a mesma operação (1 e 2).
**Resultado obtido:** `Erro: Esses usuarios ja sao amigos.` — OK.

## IV. Remover amizade

**Cenário:** remover a amizade criada no caso III.
**Entrada:**
```
Opcao: 4
Id do primeiro usuario: 1
Id do segundo usuario: 2
```
**Resultado esperado:** desfaz a ligação entre os dois usuários.
**Resultado obtido:** `Amizade removida com sucesso.` — OK.

**Caso de erro (remoção de amizade inexistente):** repetir a mesma remoção.
**Resultado obtido:** `Erro: Esses usuarios nao sao amigos.` — OK.

## V. Criar publicação

**Cenário:** registrar uma publicação textual do usuário 1.
**Entrada:**
```
Opcao: 5
Id do usuario: 1
Texto da publicacao: Ola, mundo!
```
**Resultado esperado:** publicação associada ao usuário 1.
**Resultado obtido:** `Publicacao criada com sucesso.` — OK.

## VI. Exibir publicações

**Cenário:** listar as publicações do usuário 1 (após o caso V).
**Entrada:**
```
Opcao: 6
Id do usuario: 1
```
**Resultado esperado:** lista as publicações do usuário, mais recente primeiro.
**Resultado obtido:** `Ola, mundo!` — OK.

## VII. Histórico (N ≥ 5, mais recente primeiro)

**Cenário:** após os cadastros e ações anteriores, consultar o histórico.
**Entrada:**
```
Opcao: 7
```
**Resultado esperado:** até N=10 atividades mais recentes, da mais recente
para a mais antiga (nova publicação, nova amizade, novo usuário, ...).
**Resultado obtido:**
```
[Nova publicacao] Nova publicacao de Ana Souza (id 1)
[Nova amizade] Nova amizade: Ana Souza (id 1) e Carla Dias (id 3)
[Nova amizade] Nova amizade: Ana Souza (id 1) e Bruno Lima (id 2)
[Novo usuario] Novo usuario cadastrado: Carla Dias (id 3, login carla)
[Novo usuario] Novo usuario cadastrado: Bruno Lima (id 2, login bruno)
[Novo usuario] Novo usuario cadastrado: Ana Souza (id 1, login ana)
```
Ordem da mais recente para a mais antiga — OK.

## VIII. Exibir amigos

**Cenário:** listar os amigos do usuário 1, que é amigo de 2 e 3.
**Entrada:**
```
Opcao: 8
Id do usuario: 1
```
**Resultado esperado:** lista de amigos do usuário 1.
**Resultado obtido:**
```
Amigos de Ana Souza:
  - id 2 | nome: Bruno Lima | login: bruno
  - id 3 | nome: Carla Dias | login: carla
```
— OK.

## IX. Verificar conexão entre usuários

**Cenário:** verificar se os usuários 1 e 2 (amigos diretos) estão conectados.
**Entrada:**
```
Opcao: 9
Id do primeiro usuario: 1
Id do segundo usuario: 2
```
**Resultado esperado:** `estao conectados`.
**Resultado obtido:** `Os usuarios 1 e 2 estao conectados na rede.` — OK.

**Caso sem conexão:** usuário isolado (sem amizades) verificado contra outro.
**Resultado obtido:** `Os usuarios X e Y NAO estao conectados na rede.` — OK.

## X. Sugestão de amizades

**Cenário:** rede com A(1)-B(2), A(1)-C(3); usuário C(3) consulta sugestões
(B(2) tem 1 amigo em comum com C: o usuário A).
**Entrada:**
```
Opcao: 10
Id do usuario: 3
```
**Resultado esperado:** sugere usuários a distância 2 (amigos de amigos),
ordenados por amigos em comum decrescente e, em empate, por nome, no máximo 3.
**Resultado obtido:**
```
Sugestoes de amizade para Carla Dias:
  - Bruno Lima (id 2) | 1 amigo(s) em comum
```
— OK.

---

## Requisito não funcional I — Persistência

Ver `tests/smoke_test.sh` (automatizado): cadastra usuários/amizade/publicação,
encerra o programa, reabre em um novo processo e confirma via busca, exibir
amigos e exibir publicações que os dados voltaram; confirma também que o
próximo id gerado continua a sequência anterior (não reseta para 1).

## Requisitos não funcionais II e III — Validação e duplicidade

Cobertos inline nos casos I, II, III, IV e IX acima (login duplicado, nome sem
resultado, amizade duplicada, remoção de amizade inexistente, ids
inexistentes em qualquer operação) — em nenhum caso o programa encerra
abruptamente; todas as automatizadas nos testes unitários
(`tests/test_rede_social.c`) e de integração (`make test`).
