#include "persistencia.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINHA 512
#define HISTORICO_MAX_LINHAS 64

static void remover_quebra_linha(char *s) {
    size_t tam = strlen(s);
    while (tam > 0 && (s[tam - 1] == '\n' || s[tam - 1] == '\r')) {
        s[--tam] = '\0';
    }
}

/* Corta 'linha' no primeiro 'delim', substituindo-o por '\0', e retorna o
 * restante da string (ou NULL se o delimitador não existir). */
static char *dividir_primeiro(char *linha, char delim) {
    char *p = strchr(linha, delim);
    if (p == NULL) {
        return NULL;
    }
    *p = '\0';
    return p + 1;
}

/* --- escrita --- */

static void escrever_usuario(Usuario *u, void *ctx) {
    FILE *arq = (FILE *)ctx;
    fprintf(arq, "%d;%s;%s\n", u->id, u->nome, u->login);
}

static void escrever_amizade(int id1, int id2, void *ctx) {
    FILE *arq = (FILE *)ctx;
    fprintf(arq, "%d;%d\n", id1, id2);
}

typedef struct {
    char **itens;
    int n;
    int capacidade;
} BufferTextos;

static void coletar_texto(void *dado, void *ctx) {
    BufferTextos *b = (BufferTextos *)ctx;
    if (b->n == b->capacidade) {
        b->capacidade = b->capacidade == 0 ? 8 : b->capacidade * 2;
        b->itens = realloc(b->itens, (size_t)b->capacidade * sizeof(char *));
    }
    b->itens[b->n++] = (char *)dado;
}

static void escrever_publicacoes_usuario(Usuario *u, void *ctx) {
    FILE *arq = (FILE *)ctx;
    BufferTextos buf = { NULL, 0, 0 };
    lista_percorrer(u->publicacoes, coletar_texto, &buf);

    /* buf.itens[0] = mais recente ... buf.itens[n-1] = mais antiga.
     * Escrevemos da mais antiga para a mais recente: como o carregamento
     * sempre insere no início da lista, a ordem final fica igual à atual. */
    for (int i = buf.n - 1; i >= 0; i--) {
        fprintf(arq, "%d;%s\n", u->id, buf.itens[i]);
    }
    free(buf.itens);
}

typedef struct {
    Atividade itens[HISTORICO_MAX_LINHAS];
    int n;
} BufferHistorico;

static void coletar_atividade(const Atividade *a, void *ctx) {
    BufferHistorico *b = (BufferHistorico *)ctx;
    if (b->n < HISTORICO_MAX_LINHAS) {
        b->itens[b->n++] = *a;
    }
}

void persistencia_salvar(RedeSocial *rs, const char *caminho) {
    FILE *arq = fopen(caminho, "w");
    if (arq == NULL) {
        return;
    }

    fprintf(arq, "USUARIOS\n");
    rs_percorrer_usuarios(rs, escrever_usuario, arq);

    fprintf(arq, "AMIZADES\n");
    rs_percorrer_amizades(rs, escrever_amizade, arq);

    fprintf(arq, "PUBLICACOES\n");
    rs_percorrer_usuarios(rs, escrever_publicacoes_usuario, arq);

    fprintf(arq, "HISTORICO\n");
    BufferHistorico bh = { .n = 0 };
    rs_exibir_historico(rs, coletar_atividade, &bh); /* mais recente -> mais antiga */
    for (int i = bh.n - 1; i >= 0; i--) {
        fprintf(arq, "%d;%s\n", (int)bh.itens[i].tipo, bh.itens[i].descricao);
    }

    fclose(arq);
}

/* --- leitura --- */

typedef enum { SECAO_NENHUMA, SECAO_USUARIOS, SECAO_AMIZADES, SECAO_PUBLICACOES, SECAO_HISTORICO } Secao;

void persistencia_carregar(RedeSocial *rs, const char *caminho) {
    FILE *arq = fopen(caminho, "r");
    if (arq == NULL) {
        return; /* primeira execucao: ainda nao ha arquivo salvo */
    }

    char linha[TAM_LINHA];
    Secao secao = SECAO_NENHUMA;

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        remover_quebra_linha(linha);
        if (linha[0] == '\0') {
            continue;
        }

        if (strcmp(linha, "USUARIOS") == 0) {
            secao = SECAO_USUARIOS;
            continue;
        }
        if (strcmp(linha, "AMIZADES") == 0) {
            secao = SECAO_AMIZADES;
            continue;
        }
        if (strcmp(linha, "PUBLICACOES") == 0) {
            secao = SECAO_PUBLICACOES;
            continue;
        }
        if (strcmp(linha, "HISTORICO") == 0) {
            secao = SECAO_HISTORICO;
            continue;
        }

        switch (secao) {
            case SECAO_USUARIOS: {
                char *campo_id = linha;
                char *campo_nome = dividir_primeiro(campo_id, ';');
                if (campo_nome == NULL) {
                    break;
                }
                char *campo_login = dividir_primeiro(campo_nome, ';');
                if (campo_login == NULL) {
                    break;
                }
                rs_cadastrar_usuario_silencioso(rs, atoi(campo_id), campo_nome, campo_login);
                break;
            }
            case SECAO_AMIZADES: {
                char *campo_id1 = linha;
                char *campo_id2 = dividir_primeiro(campo_id1, ';');
                if (campo_id2 == NULL) {
                    break;
                }
                rs_adicionar_amizade_silenciosa(rs, atoi(campo_id1), atoi(campo_id2));
                break;
            }
            case SECAO_PUBLICACOES: {
                char *campo_id = linha;
                char *texto = dividir_primeiro(campo_id, ';');
                if (texto == NULL) {
                    break;
                }
                rs_criar_publicacao_silenciosa(rs, atoi(campo_id), texto);
                break;
            }
            case SECAO_HISTORICO: {
                char *campo_tipo = linha;
                char *descricao = dividir_primeiro(campo_tipo, ';');
                if (descricao == NULL) {
                    break;
                }
                Atividade a;
                a.tipo = (TipoAtividade)atoi(campo_tipo);
                strncpy(a.descricao, descricao, sizeof(a.descricao) - 1);
                a.descricao[sizeof(a.descricao) - 1] = '\0';
                rs_empilhar_historico_bruto(rs, a);
                break;
            }
            case SECAO_NENHUMA:
            default:
                break;
        }
    }

    fclose(arq);
}
