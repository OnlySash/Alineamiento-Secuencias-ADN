# --- COMPILER CFG ---
CC         := cc
MPI_CC     := mpicc
CFLAGS     := -Wall -Wextra -Wpedantic -std=c11 -Iinclude
LDFLAGS    := -lpthread

# --- DIRECTORIES ---
SRC_DIR    := src
OBJ_DIR    := build
BIN_DIR    := bin

# --- OBJECTS ---
BASE_OBJ   := $(OBJ_DIR)/base.o
SEQ_OBJ    := $(OBJ_DIR)/base_sequential.o
PTH_OBJ    := $(OBJ_DIR)/base_pthread.o
MPI_OBJ    := $(OBJ_DIR)/base_mpi.o
PARAMS_OBJ := $(OBJ_DIR)/params.o

# --- TARGETS ---
TARGET_SEQ := $(BIN_DIR)/base_sequential
TARGET_PTH := $(BIN_DIR)/base_pthread
TARGET_MPI := $(BIN_DIR)/base_mpi

# --- RULES ---
# Default [all targets]
all: $(TARGET_SEQ) $(TARGET_PTH) $(TARGET_MPI)
	@echo "All targets compiled. Me go home."

seq: $(TARGET_SEQ)
	@echo "Sequential target ready. Me go home."

pth: $(TARGET_PTH)
	@echo "Pthreads target ready. Me go home."

mpi: $(TARGET_MPI)
	@echo "MPI target ready. Me go home."

# Linking Sequential
$(TARGET_SEQ): $(SEQ_OBJ) $(BASE_OBJ) $(PARAMS_OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@
	@echo "Compilation of $@ completed."

# Linking Pthreads
$(TARGET_PTH): $(PTH_OBJ) $(BASE_OBJ) $(PARAMS_OBJ) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@
	@echo "Compilation of $@ completed."

# Linking MPI
$(TARGET_MPI): $(MPI_OBJ) $(BASE_OBJ) $(PARAMS_OBJ) | $(BIN_DIR)
	$(MPI_CC) $^ -o $@
	@echo "Compilation of $@ completed."

# Compilation generic rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
# Compilation specific rule for MPI
$(TARGET_MPI): CC = $(MPI_CC)

# Folders
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Cleaning
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaning done. Me go home."

# Treat as execution cmds
.PHONY: all clean seq pth mpi