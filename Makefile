# --- CONFIGURACION DEL COMPILADOR ---
CC         := mpicc
CFLAGS     := -Wall -Wextra -Wpedantic -std=c11 -Iinclude -DCL_TARGET_OPENCL_VERSION=300
LDFLAGS    := -lpthread -lOpenCL

# --- DIRECTORIOS ---
SRC_DIR    := src
TEST_DIR   := tests
OBJ_DIR    := build
BIN_DIR    := bin

# --- OBJETOS ---
# Lista de todos los archivos .o que necesitamos generar
OBJS       := $(OBJ_DIR)/main.o \
              $(OBJ_DIR)/base.o \
              $(OBJ_DIR)/base_sequential.o \
              $(OBJ_DIR)/base_pthreads.o \
              $(OBJ_DIR)/base_mpi.o \
              $(OBJ_DIR)/base_opencl.o \
              $(OBJ_DIR)/params.o \
              $(OBJ_DIR)/tests.o

# --- EJECUTABLE FINAL ---
TARGET     := $(BIN_DIR)/dna_search
# --- REGLAS PRINCIPALES ---
all: $(TARGET)
	@echo "Listo para ejecución."

# Como enlazar el ejecutable final
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@
	@echo "Ejecutable creado en ./$@"

# --- REGLAS DE COMPILACION DE OBJETOS ---
# Regla para compilar los .c que estan en src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla especifica para compilar el archivo de tests/
$(OBJ_DIR)/tests.o: $(TEST_DIR)/tests.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para crear las carpetas si no existen
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# --- LIMPIEZA ---
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpieza completada. '/build' y '/bin' removidas."

.PHONY: all clean