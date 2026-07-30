#ifndef REDE_SOCIAL_H
#define REDE_SOCIAL_H

#include <stddef.h>
#include "usuario.h"
#include "lista.h"
#include "pilha.h"
#include "grafo.h"

/*
 * Fachada do sistema: combina AVL (índice por id), duas Tabelas Hash (por
 * login e por nome), Grafo (amizades) e Pilha (histórico). Concentra toda a
 * lógica de negócio e validação — main.c só faz I/O e chama estas funções.
 *
 * Funções _silencioso/_bruto não geram evento de histórico; usadas apenas
 * por persistencia.c ao recarregar o estado salvo (senão duplicaríamos
 * eventos que já estão registrados na seção HISTORICO do arquivo).
 */

typedef struct RedeSocial RedeSocial;

RedeSocial *rede_social_criar(void);

/* Retorna o id gerado, ou -1 em caso de erro (mensagem em 'erro'). */
int rs_cadastrar_usuario(RedeSocial *rs, const char *nome, const char *login, char *erro, size_t erro_tam);

/* Lista de Usuario* com esse nome, ou NULL se nenhum for encontrado. Não
 * destruir os Usuario* contidos — só a Lista, com lista_destruir(l, NULL). */
Lista *rs_buscar_por_nome(RedeSocial *rs, const char *nome);

Usuario *rs_buscar_por_id(RedeSocial *rs, int id);

int rs_adicionar_amizade(RedeSocial *rs, int id1, int id2, char *erro, size_t erro_tam);
int rs_remover_amizade(RedeSocial *rs, int id1, int id2, char *erro, size_t erro_tam);

int rs_criar_publicacao(RedeSocial *rs, int id, const char *texto, char *erro, size_t erro_tam);

/* Lista de Usuario* amigos de 'id' (vazia se não tiver amigos ou id não existir).
 * Destruir só a Lista, com lista_destruir(l, NULL). */
Lista *rs_exibir_amigos(RedeSocial *rs, int id);

int rs_verificar_conexao(RedeSocial *rs, int id1, int id2);

/* Preenche 'saida' (capacidade >= 3) com até 3 sugestões. Retorna quantas. */
int rs_sugerir_amizades(RedeSocial *rs, int id, Sugestao *saida);

/* Percorre o histórico do mais recente para o mais antigo. */
void rs_exibir_historico(const RedeSocial *rs, void (*visitar)(const Atividade *a, void *ctx), void *ctx);

/* --- usadas só por persistencia.c ao carregar o arquivo salvo --- */
int rs_cadastrar_usuario_silencioso(RedeSocial *rs, int id, const char *nome, const char *login);
int rs_adicionar_amizade_silenciosa(RedeSocial *rs, int id1, int id2);
int rs_criar_publicacao_silenciosa(RedeSocial *rs, int id, const char *texto);
void rs_empilhar_historico_bruto(RedeSocial *rs, Atividade a);

/* --- usadas só por persistencia.c ao salvar o arquivo --- */
void rs_percorrer_usuarios(RedeSocial *rs, void (*visitar)(Usuario *u, void *ctx), void *ctx);
void rs_percorrer_amizades(RedeSocial *rs, void (*visitar)(int id1, int id2, void *ctx), void *ctx);

void rs_destruir(RedeSocial *rs);

#endif
