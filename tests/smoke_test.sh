#!/bin/bash
# Smoke test de persistência: roda o binário em 3 execuções sucessivas,
# num diretório temporário isolado, e confere que os dados sobrevivem entre
# uma execução e outra (Requisito Não Funcional I do enunciado).
set -e

DIR_RAIZ="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$DIR_RAIZ/bin/rede_social"

if [ ! -x "$BIN" ]; then
    echo "Binario nao encontrado em $BIN. Rode 'make' antes de executar este teste." >&2
    exit 1
fi

TMPDIR_TESTE="$(mktemp -d)"
trap 'rm -rf "$TMPDIR_TESTE"' EXIT

cd "$TMPDIR_TESTE"

falhar() {
    echo "FALHOU: $1" >&2
    exit 1
}

# --- 1a execucao: cadastra 2 usuarios, cria amizade e publicacao ---
SAIDA1=$(printf '1\nAna Souza\nana\n1\nBruno Lima\nbruno\n3\n1\n2\n5\n1\nOla, mundo!\n0\n' | "$BIN")

echo "$SAIDA1" | grep -q "Usuario cadastrado com sucesso! id = 1" || falhar "cadastro do usuario id 1"
echo "$SAIDA1" | grep -q "Usuario cadastrado com sucesso! id = 2" || falhar "cadastro do usuario id 2"
echo "$SAIDA1" | grep -q "Amizade criada com sucesso."           || falhar "criacao de amizade"
echo "$SAIDA1" | grep -q "Publicacao criada com sucesso."        || falhar "criacao de publicacao"

[ -f rede_social.dat ] || falhar "arquivo de persistencia nao foi criado ao sair"

# --- 2a execucao: confirma que os dados persistiram (recarregados do arquivo) ---
SAIDA2=$(printf '2\nAna Souza\n8\n1\n6\n1\n0\n' | "$BIN")

echo "$SAIDA2" | grep -q "id 1 | nome: Ana Souza | login: ana"    || falhar "busca por nome nao encontrou usuario apos recarregar"
echo "$SAIDA2" | grep -q "id 2 | nome: Bruno Lima | login: bruno" || falhar "amizade nao persistiu entre execucoes"
echo "$SAIDA2" | grep -q "Ola, mundo!"                            || falhar "publicacao nao persistiu entre execucoes"

# --- 3a execucao: garante que o proximo id continua de onde parou (nao reseta) ---
SAIDA3=$(printf '1\nCarla Dias\ncarla\n0\n' | "$BIN")
echo "$SAIDA3" | grep -q "Usuario cadastrado com sucesso! id = 3" || falhar "proximo id nao continuou apos persistencia (esperava id 3)"

echo "Smoke test de persistencia: OK (3 execucoes sucessivas, dados preservados)"
