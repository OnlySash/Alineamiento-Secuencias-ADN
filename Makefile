# --- COMPILER CFG ---
CC       := gcc
MPI_CC   := mpicc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c11 -Iinclude
LDFLAGS  := -lpthread

# --- DIRECTORIES ---
SRC_DIR   := src
OBJ_DIR   := build
BIN_DIR   := bin

# --- ARCHIVES ---
BASE_OBJ  := $(OBJ_DIR)/base.o

# --- TARGETS ---
TARGET_SEQ := $(BIN_DIR)/base_sequential
TARGET_PTH := $(BIN_DIR)/base_pthread

# --- RULES ---
# Default [all targets]
all: $(TARGET_SEQ) $(TARGET_PTH)
	@echo "All targets compiled. Me go home."

seq: $(TARGET_SEQ)
	@echo "Sequential target ready. Me go home."

pth: $(TARGET_PTH)
	@echo "Pthreads target ready. Me go home."

# Linking Sequential
$(TARGET_SEQ): $(OBJ_DIR)/base_sequential.o $(BASE_OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@
	@echo "Compilation of $@ completed."

# Linking Pthreads
$(TARGET_PTH): $(OBJ_DIR)/base_pthread.o $(BASE_OBJ) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@
	@echo "Compilation of $@ completed."

# Compilation (change to MPI/CC when needed)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Folders
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# 6. Cleaning
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaning done. Me go home."

# Treat as execution cmds
.PHONY: all clean seq pth