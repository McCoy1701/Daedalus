CC=gcc
CFLAGS = -Iinclude/ -Wall -pedantic -lm

SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
OBJ_DIR=obj

.PHONY: all clean install uninstall


all: shared debug


shared: $(OBJ_DIR)/main.o $(OBJ_DIR)/dVectorMath.o $(OBJ_DIR)/dMatrixMath.o $(OBJ_DIR)/dMatrixCreation.o $(OBJ_DIR)/dDeltaTime.o $(OBJ_DIR)/dLinkedList.o
	mkdir -p $(BIN_DIR)
	$(CC) -shared $(OBJ_DIR)/main.o $(OBJ_DIR)/dVectorMath.o $(OBJ_DIR)/dMatrixMath.o $(OBJ_DIR)/dMatrixCreation.o $(OBJ_DIR)/dDeltaTime.o $(OBJ_DIR)/dLinkedList.o -o $(BIN_DIR)/libdaedalus.so $(CFLAGS)


$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dDeltaTime.o: $(SRC_DIR)/dDeltaTime.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dLinkedList.o: $(SRC_DIR)/dLinkedList.c
	$(CC) -c $< -o $@ $(CFLAGS)

install:
	sudo cp $(BIN_DIR)/libdaedalus.so /usr/lib/libdaedalus.so
	sudo cp $(INC_DIR)/daedalus.h /usr/include/daedalus.h

uninstall:
	sudo rm /usr/lib/libdaedalus.so
	sudo rm /usr/include/daedalus.h


debug: $(OBJ_DIR)/debug_main.o $(OBJ_DIR)/debug_dVectorMath.o $(OBJ_DIR)/debug_dMatrixMath.o $(OBJ_DIR)/debug_dMatrixCreation.o $(OBJ_DIR)/debug_dDeltaTime.o $(OBJ_DIR)/debug_dLinkedList.o
	mkdir -p $(BIN_DIR)
	$(CC) -ggdb $(OBJ_DIR)/debug_main.o $(OBJ_DIR)/debug_dVectorMath.o $(OBJ_DIR)/debug_dMatrixMath.o $(OBJ_DIR)/debug_dMatrixCreation.o $(OBJ_DIR)/debug_dDeltaTime.o $(OBJ_DIR)/debug_dLinkedList.o -o $(BIN_DIR)/$@.o $(CFLAGS)

$(OBJ_DIR)/debug_main.o: $(SRC_DIR)/main.c
	mkdir -p $(OBJ_DIR)
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dDeltaTime.o: $(SRC_DIR)/dDeltaTime.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dLinkedList.o: $(SRC_DIR)/dLinkedList.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	clear
