CC         := mpicc
CFLAGS     := -std=c11 -Wall -Wextra -Wpedantic -Iinclude -DCL_TARGET_OPENCL_VERSION=300
LDFLAGS    := -lpthread -lOpenCL

SRC_DIR    := src
TEST_DIR   := tests
OBJ_DIR    := build
BIN_DIR    := bin

OBJS       := $(OBJ_DIR)/main.o \
              $(OBJ_DIR)/base.o \
              $(OBJ_DIR)/base_sequential.o \
              $(OBJ_DIR)/base_pthreads.o \
			  $(OBJ_DIR)/base_pthreads_vieja.o \
              $(OBJ_DIR)/base_mpi.o \
              $(OBJ_DIR)/base_opencl.o \
              $(OBJ_DIR)/params.o \
              $(OBJ_DIR)/tests.o


TARGET     := $(BIN_DIR)/dna_search

all: $(TARGET)
	@echo "Listo para ejecución."


$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@
	@echo "Ejecutable creado en ./$@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/tests.o: $(TEST_DIR)/tests.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@


test: $(TARGET)
	@echo "Ejecutando suite de pruebas unitarias..."
	./$(TARGET) -m 5

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	
PHONY: all clean test

sanitize_all: sanitize_asan sanitize_tsan sanitize_ubsan sanitize_mpi
	./$(BIN_DIR)/dna_asan -m 2
	./$(BIN_DIR)/dna_tsan -m 2
	./$(BIN_DIR)/dna_ubsan -m 2
	mpirun --oversubscribe -n 4 ./$(BIN_DIR)/dna_mpi_san -m 3

sanitize_asan: $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -fsanitize=address,leak -fno-omit-frame-pointer -o $(BIN_DIR)/dna_asan

sanitize_tsan: $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -fsanitize=thread -fno-omit-frame-pointer -o $(BIN_DIR)/dna_tsan

sanitize_ubsan: $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -fsanitize=undefined -fno-omit-frame-pointer -o $(BIN_DIR)/dna_ubsan

sanitize_msan: | $(BIN_DIR)
	clang $(CFLAGS) $(shell mpicc --showme:compile) $(SRC_DIR)/main.c \
		$(SRC_DIR)/base.c $(SRC_DIR)/base_sequential.c \
		$(SRC_DIR)/base_pthreads.c $(SRC_DIR)/base_mpi.c \
		$(SRC_DIR)/base_opencl.c $(SRC_DIR)/params.c \
		$(TEST_DIR)/tests.c \
		$(LDFLAGS) $(shell mpicc --showme:link) \
		-fsanitize=memory -fno-omit-frame-pointer -o $(BIN_DIR)/dna_msan

sanitize_mpi: $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -o $(BIN_DIR)/dna_mpi_san

.PHONY: sanitize_all sanitize_asan sanitize_tsan sanitize_ubsan sanitize_msan sanitize_mpi