#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "rede_social.h"

/*
 * Persistência em arquivo de texto simples (seções USUARIOS / AMIZADES /
 * PUBLICACOES / HISTORICO, campos separados por ';'). Ver
 * docs/superpowers/specs/2026-07-30-rede-social-design.md secao 5.
 *
 * Restrição de formato: nome e login de usuário não podem conter ';'
 * (validado no cadastro, em main.c). Texto de publicação e descrição de
 * histórico podem conter ';' livremente, pois são sempre o último campo da
 * linha.
 */

/* Sobrescreve o arquivo em 'caminho' com o estado atual de 'rs'. */
void persistencia_salvar(RedeSocial *rs, const char *caminho);

/* Recarrega o estado salvo em 'caminho' para dentro de 'rs' (que deve estar
 * recém-criado e vazio). Se o arquivo não existir, não faz nada (primeira
 * execução do sistema). */
void persistencia_carregar(RedeSocial *rs, const char *caminho);

#endif
