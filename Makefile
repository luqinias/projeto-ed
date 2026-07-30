CC ?= gcc
CFLAGS = -std=c11 -Wall -Wextra -Isrc -g
BIN_DIR = bin
OBJ_DIR = obj

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = $(BIN_DIR)/rede_social

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# --- Testes unitarios por TAD ---
TEST_BINS = $(BIN_DIR)/test_lista $(BIN_DIR)/test_fila $(BIN_DIR)/test_pilha \
            $(BIN_DIR)/test_avl $(BIN_DIR)/test_hash_tabela $(BIN_DIR)/test_grafo \
            $(BIN_DIR)/test_rede_social

$(BIN_DIR)/test_lista: tests/test_lista.c $(SRC_DIR)/lista.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_fila: tests/test_fila.c $(SRC_DIR)/fila.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_pilha: tests/test_pilha.c $(SRC_DIR)/pilha.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_avl: tests/test_avl.c $(SRC_DIR)/avl.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(BIN_DIR)/test_hash_tabela: tests/test_hash_tabela.c $(SRC_DIR)/hash_tabela.c $(SRC_DIR)/lista.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_grafo: tests/test_grafo.c $(SRC_DIR)/grafo.c $(SRC_DIR)/fila.c $(SRC_DIR)/lista.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_rede_social: tests/test_rede_social.c $(SRC_DIR)/rede_social.c $(SRC_DIR)/avl.c \
            $(SRC_DIR)/hash_tabela.c $(SRC_DIR)/grafo.c $(SRC_DIR)/pilha.c $(SRC_DIR)/usuario.c \
            $(SRC_DIR)/lista.c $(SRC_DIR)/fila.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_BINS)
	@status=0; \
	for t in $(TEST_BINS); do \
		echo "== $$t =="; \
		./$$t || status=1; \
	done; \
	exit $$status

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
