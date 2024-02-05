CC=gcc
CFLAGS = -Iinclude/

SRC_DIR=src
INCLUDE_DIR=include
BIN_DIR=bin
OBJ_DIR=obj

.PHONY: all clean


all: $(BIN_DIR)

$(BIN_DIR): $(OBJ_DIR)/main.o $(OBJ_DIR)/dVectorMath.o
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_DIR)/main.o $(OBJ_DIR)/dVectorMath.o  $(CFLAGS) -o $(BIN_DIR)/$@


$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	clear
