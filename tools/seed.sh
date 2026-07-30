#!/bin/bash
# Popula rede_social.dat com uma base de demonstração, através do próprio
# binário (ou seja, passando pelas mesmas validações e regras de negócio
# usadas em produção — não escreve o arquivo de persistência na mão).
#
# Cenário criado, pensado para demonstrar TODAS as funcionalidades:
#   - Ana (1) é um "hub": amiga de Bruno, Carla e Diego.
#   - Elisa (5) é amiga de Bruno e Carla -> sugestão de amizade para Elisa
#     deve indicar Ana (2 amigos em comum) e Fábio (1 amigo em comum).
#   - Gabriela (7) e Heitor (8) formam um par isolado do resto da rede
#     -> "verificar conexão" entre eles e Ana deve dar NÃO conectados.
#   - Igor (9) não tem nenhum amigo nem publicação -> casos "vazio".
#   - Existem DOIS usuários chamados "Ana Souza" (ids 1 e 10, logins
#     diferentes) -> "buscar por nome" deve retornar os dois.
#   - 22 eventos são gerados (10 cadastros + 8 amizades + 4 publicações),
#     mais que o histórico (N=10) guarda -> mostra o truncamento.
set -e

DIR_RAIZ="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$DIR_RAIZ/bin/rede_social"
DAT="$DIR_RAIZ/rede_social.dat"

if [ ! -x "$BIN" ]; then
    echo "Binario nao encontrado; compilando com 'make'..."
    make -C "$DIR_RAIZ"
fi

if [ -f "$DAT" ]; then
    BACKUP="$DAT.bak.$(date +%Y%m%d%H%M%S)"
    echo "Já existe $DAT — fazendo backup em $(basename "$BACKUP") antes de recriar."
    mv "$DAT" "$BACKUP"
fi

cd "$DIR_RAIZ"

printf '%s\n' \
  "1" "Ana Souza" "ana" \
  "1" "Bruno Lima" "bruno" \
  "1" "Carla Dias" "carla" \
  "1" "Diego Alves" "diego" \
  "1" "Elisa Ramos" "elisa" \
  "1" "Fabio Nunes" "fabio" \
  "1" "Gabriela Reis" "gabriela" \
  "1" "Heitor Costa" "heitor" \
  "1" "Igor Teixeira" "igor" \
  "1" "Ana Souza" "ana2" \
  "3" "1" "2" \
  "3" "1" "3" \
  "3" "1" "4" \
  "3" "2" "5" \
  "3" "3" "5" \
  "3" "4" "6" \
  "3" "3" "6" \
  "3" "7" "8" \
  "5" "1" "Bom dia, pessoal!" \
  "5" "1" "Terminando o projeto de Estruturas de Dados." \
  "5" "2" "Alguem mais estudando para a prova de ED?" \
  "5" "5" "Adorei o final de semana!" \
  "0" \
  | "$BIN" > /tmp/seed_saida.log

echo "Seed aplicada com sucesso em $DAT"
echo
echo "Usuarios criados:"
printf '  %-4s %-14s %s\n' "id" "nome" "login"
printf '  %-4s %-14s %s\n' "1"  "Ana Souza"      "ana"
printf '  %-4s %-14s %s\n' "2"  "Bruno Lima"     "bruno"
printf '  %-4s %-14s %s\n' "3"  "Carla Dias"     "carla"
printf '  %-4s %-14s %s\n' "4"  "Diego Alves"    "diego"
printf '  %-4s %-14s %s\n' "5"  "Elisa Ramos"    "elisa"
printf '  %-4s %-14s %s\n' "6"  "Fabio Nunes"    "fabio"
printf '  %-4s %-14s %s\n' "7"  "Gabriela Reis"  "gabriela"
printf '  %-4s %-14s %s\n' "8"  "Heitor Costa"   "heitor"
printf '  %-4s %-14s %s\n' "9"  "Igor Teixeira"  "igor"
printf '  %-4s %-14s %s\n' "10" "Ana Souza"      "ana2"
echo
echo "Sugestoes de demonstracao:"
echo "  - Sugestao de amizades para o id 5 (Elisa)  -> Ana (2 em comum), Fabio (1 em comum)"
echo "  - Verificar conexao entre 1 e 7              -> NAO conectados"
echo "  - Buscar por nome \"Ana Souza\"                -> 2 resultados (ids 1 e 10)"
echo "  - Exibir amigos/publicacoes do id 9 (Igor)   -> vazio"
echo "  - Historico (opcao 7)                        -> mostra so os 10 mais recentes dos 22 eventos gerados"
echo
echo "Rode ./bin/rede_social para explorar os dados."
