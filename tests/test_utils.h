#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <string.h>

static int testes_ok = 0;
static int testes_falhos = 0;

#define ASSERT_TRUE(cond) \
    do { \
        if (cond) { \
            testes_ok++; \
        } else { \
            testes_falhos++; \
            printf("FALHOU (%s:%d): %s\n", __FILE__, __LINE__, #cond); \
        } \
    } while (0)

#define ASSERT_EQ_INT(a, b) \
    do { \
        long va = (long)(a); \
        long vb = (long)(b); \
        if (va == vb) { \
            testes_ok++; \
        } else { \
            testes_falhos++; \
            printf("FALHOU (%s:%d): %s == %s (%ld != %ld)\n", __FILE__, __LINE__, #a, #b, va, vb); \
        } \
    } while (0)

#define ASSERT_EQ_STR(a, b) \
    do { \
        const char *va = (a); \
        const char *vb = (b); \
        if (strcmp(va, vb) == 0) { \
            testes_ok++; \
        } else { \
            testes_falhos++; \
            printf("FALHOU (%s:%d): %s == %s (\"%s\" != \"%s\")\n", __FILE__, __LINE__, #a, #b, va, vb); \
        } \
    } while (0)

#define TEST_RESUMO() \
    do { \
        printf("\n%d passaram, %d falharam\n", testes_ok, testes_falhos); \
        return testes_falhos == 0 ? 0 : 1; \
    } while (0)

#endif
