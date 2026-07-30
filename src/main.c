#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rede_social.h"
#include "persistencia.h"

#define ARQUIVO_DADOS "rede_social.dat"

static int entrada_encerrada = 0;

static void remover_quebra_linha(char *s) {
    size_t tam = strlen(s);
    while (tam > 0 && (s[tam - 1] == '\n' || s[tam - 1] == '\r')) {
        s[--tam] = '\0';
    }
}

/* Le uma linha do teclado em 'buf' (capacidade 'tam'), sem o '\n' final.
 * Retorna 0 e marca fim de entrada (EOF) se nao houver mais dados. */
static int ler_linha(char *buf, size_t tam) {
    if (fgets(buf, (int)tam, stdin) == NULL) {
        entrada_encerrada = 1;
        buf[0] = '\0';
        return 0;
    }
    remover_quebra_linha(buf);
    return 1;
}

static int ler_inteiro(const char *prompt, int *saida) {
    char buf[64];
    while (1) {
        printf("%s", prompt);
        if (!ler_linha(buf, sizeof(buf))) {
            return 0;
        }
        if (buf[0] == '\0') {
            printf("Entrada invalida. Digite um numero inteiro.\n");
            continue;
        }
        char *fim;
        long valor = strtol(buf, &fim, 10);
        if (fim != buf && *fim == '\0') {
            *saida = (int)valor;
            return 1;
        }
        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

static void exibir_menu(void) {
    printf("\n===== Rede Social Simplificada =====\n");
    printf(" 1. Cadastrar usuario\n");
    printf(" 2. Buscar usuario por nome\n");
    printf(" 3. Adicionar amizade\n");
    printf(" 4. Remover amizade\n");
    printf(" 5. Criar publicacao\n");
    printf(" 6. Exibir publicacoes de um usuario\n");
    printf(" 7. Exibir historico de atividades\n");
    printf(" 8. Exibir amigos de um usuario\n");
    printf(" 9. Verificar conexao entre dois usuarios\n");
    printf("10. Sugerir amizades\n");
    printf(" 0. Sair\n");
}

static void exibir_usuario_linha(void *dado, void *ctx) {
    Usuario *u = (Usuario *)dado;
    (void)ctx;
    printf("  - id %d | nome: %s | login: %s\n", u->id, u->nome, u->login);
}

static void acao_cadastrar_usuario(RedeSocial *rs) {
    char nome[USUARIO_NOME_TAM];
    char login[USUARIO_LOGIN_TAM];
    char erro[200];

    printf("Nome: ");
    if (!ler_linha(nome, sizeof(nome))) {
        return;
    }
    printf("Login: ");
    if (!ler_linha(login, sizeof(login))) {
        return;
    }

    if (strchr(nome, ';') != NULL || strchr(login, ';') != NULL) {
        printf("Erro: nome e login nao podem conter o caractere ';'.\n");
        return;
    }

    int id = rs_cadastrar_usuario(rs, nome, login, erro, sizeof(erro));
    if (id < 0) {
        printf("Erro: %s\n", erro);
    } else {
        printf("Usuario cadastrado com sucesso! id = %d\n", id);
    }
}

static void acao_buscar_por_nome(RedeSocial *rs) {
    char nome[USUARIO_NOME_TAM];
    printf("Nome a buscar: ");
    if (!ler_linha(nome, sizeof(nome))) {
        return;
    }

    Lista *achados = rs_buscar_por_nome(rs, nome);
    if (achados == NULL || lista_vazia(achados)) {
        printf("Nenhum usuario encontrado com o nome \"%s\".\n", nome);
        return;
    }

    printf("Usuarios encontrados:\n");
    lista_percorrer(achados, exibir_usuario_linha, NULL);
}

static void acao_adicionar_amizade(RedeSocial *rs) {
    int id1, id2;
    char erro[200];
    if (!ler_inteiro("Id do primeiro usuario: ", &id1)) {
        return;
    }
    if (!ler_inteiro("Id do segundo usuario: ", &id2)) {
        return;
    }
    if (rs_adicionar_amizade(rs, id1, id2, erro, sizeof(erro))) {
        printf("Amizade criada com sucesso.\n");
    } else {
        printf("Erro: %s\n", erro);
    }
}

static void acao_remover_amizade(RedeSocial *rs) {
    int id1, id2;
    char erro[200];
    if (!ler_inteiro("Id do primeiro usuario: ", &id1)) {
        return;
    }
    if (!ler_inteiro("Id do segundo usuario: ", &id2)) {
        return;
    }
    if (rs_remover_amizade(rs, id1, id2, erro, sizeof(erro))) {
        printf("Amizade removida com sucesso.\n");
    } else {
        printf("Erro: %s\n", erro);
    }
}

static void acao_criar_publicacao(RedeSocial *rs) {
    int id;
    char texto[500];
    char erro[200];
    if (!ler_inteiro("Id do usuario: ", &id)) {
        return;
    }
    printf("Texto da publicacao: ");
    if (!ler_linha(texto, sizeof(texto))) {
        return;
    }
    if (rs_criar_publicacao(rs, id, texto, erro, sizeof(erro))) {
        printf("Publicacao criada com sucesso.\n");
    } else {
        printf("Erro: %s\n", erro);
    }
}

static void exibir_publicacao_linha(void *dado, void *ctx) {
    char *texto = (char *)dado;
    (void)ctx;
    printf("  - %s\n", texto);
}

static void acao_exibir_publicacoes(RedeSocial *rs) {
    int id;
    if (!ler_inteiro("Id do usuario: ", &id)) {
        return;
    }
    Usuario *u = rs_buscar_por_id(rs, id);
    if (u == NULL) {
        printf("Erro: usuario com id %d nao existe.\n", id);
        return;
    }
    if (lista_vazia(u->publicacoes)) {
        printf("%s ainda nao fez nenhuma publicacao.\n", u->nome);
        return;
    }
    printf("Publicacoes de %s (mais recente primeiro):\n", u->nome);
    lista_percorrer(u->publicacoes, exibir_publicacao_linha, NULL);
}

static const char *nome_tipo_atividade(TipoAtividade tipo) {
    switch (tipo) {
        case ATIV_NOVO_USUARIO: return "Novo usuario";
        case ATIV_NOVA_AMIZADE: return "Nova amizade";
        case ATIV_REMOCAO_AMIZADE: return "Remocao de amizade";
        case ATIV_NOVA_PUBLICACAO: return "Nova publicacao";
        default: return "Atividade";
    }
}

static void exibir_atividade(const Atividade *a, void *ctx) {
    (void)ctx;
    printf("  [%s] %s\n", nome_tipo_atividade(a->tipo), a->descricao);
}

static void acao_exibir_historico(RedeSocial *rs) {
    printf("Historico de atividades (mais recente primeiro):\n");
    rs_exibir_historico(rs, exibir_atividade, NULL);
}

static void acao_exibir_amigos(RedeSocial *rs) {
    int id;
    if (!ler_inteiro("Id do usuario: ", &id)) {
        return;
    }
    Usuario *u = rs_buscar_por_id(rs, id);
    if (u == NULL) {
        printf("Erro: usuario com id %d nao existe.\n", id);
        return;
    }
    Lista *amigos = rs_exibir_amigos(rs, id);
    if (lista_vazia(amigos)) {
        printf("%s ainda nao tem amigos.\n", u->nome);
    } else {
        printf("Amigos de %s:\n", u->nome);
        lista_percorrer(amigos, exibir_usuario_linha, NULL);
    }
    lista_destruir(amigos, NULL);
}

static void acao_verificar_conexao(RedeSocial *rs) {
    int id1, id2;
    if (!ler_inteiro("Id do primeiro usuario: ", &id1)) {
        return;
    }
    if (!ler_inteiro("Id do segundo usuario: ", &id2)) {
        return;
    }
    if (rs_buscar_por_id(rs, id1) == NULL || rs_buscar_por_id(rs, id2) == NULL) {
        printf("Erro: um ou ambos os ids informados nao existem.\n");
        return;
    }
    if (rs_verificar_conexao(rs, id1, id2)) {
        printf("Os usuarios %d e %d estao conectados na rede.\n", id1, id2);
    } else {
        printf("Os usuarios %d e %d NAO estao conectados na rede.\n", id1, id2);
    }
}

static void acao_sugerir_amizades(RedeSocial *rs) {
    int id;
    if (!ler_inteiro("Id do usuario: ", &id)) {
        return;
    }
    Usuario *u = rs_buscar_por_id(rs, id);
    if (u == NULL) {
        printf("Erro: usuario com id %d nao existe.\n", id);
        return;
    }

    Sugestao sugestoes[3];
    int n = rs_sugerir_amizades(rs, id, sugestoes);
    if (n == 0) {
        printf("Nenhuma sugestao de amizade disponivel para %s no momento.\n", u->nome);
        return;
    }
    printf("Sugestoes de amizade para %s:\n", u->nome);
    for (int i = 0; i < n; i++) {
        Usuario *sugerido = rs_buscar_por_id(rs, sugestoes[i].id);
        printf("  - %s (id %d) | %d amigo(s) em comum\n",
               sugerido != NULL ? sugerido->nome : "?",
               sugestoes[i].id, sugestoes[i].amigos_em_comum);
    }
}

int main(void) {
    RedeSocial *rs = rede_social_criar();
    persistencia_carregar(rs, ARQUIVO_DADOS);

    printf("Rede Social Simplificada — dados carregados de \"%s\" (se existente).\n", ARQUIVO_DADOS);

    int opcao = -1;
    while (!entrada_encerrada && opcao != 0) {
        exibir_menu();
        if (!ler_inteiro("Opcao: ", &opcao)) {
            break;
        }

        switch (opcao) {
            case 0:
                break;
            case 1:
                acao_cadastrar_usuario(rs);
                break;
            case 2:
                acao_buscar_por_nome(rs);
                break;
            case 3:
                acao_adicionar_amizade(rs);
                break;
            case 4:
                acao_remover_amizade(rs);
                break;
            case 5:
                acao_criar_publicacao(rs);
                break;
            case 6:
                acao_exibir_publicacoes(rs);
                break;
            case 7:
                acao_exibir_historico(rs);
                break;
            case 8:
                acao_exibir_amigos(rs);
                break;
            case 9:
                acao_verificar_conexao(rs);
                break;
            case 10:
                acao_sugerir_amizades(rs);
                break;
            default:
                printf("Opcao invalida. Escolha um numero entre 0 e 10.\n");
        }
    }

    persistencia_salvar(rs, ARQUIVO_DADOS);
    rs_destruir(rs);
    printf("\nDados salvos em \"%s\". Ate mais!\n", ARQUIVO_DADOS);
    return 0;
}
