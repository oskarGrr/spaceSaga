SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := include $(SRC_DIR)
EXE := $(BIN_DIR)/SpaceSaga
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC := gcc
CFLAGS := -Wall -O3 -std=c17 -g
LDLIBS := -Llib/
LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm -mthreads

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $^ -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	@$(RM) -f $(OBJ_DIR)/*.o
	@$(RM) -f $(EXE)