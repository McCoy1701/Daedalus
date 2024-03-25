CC=gcc
CFLAGS = -Iinclude/ -Wall -Wextra -fPIC -pedantic -lm

SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
OBJ_DIR=obj

.PHONY: all clean install uninstall updateHeader


all: shared debug


shared: $(OBJ_DIR)/dLinkedList.o $(OBJ_DIR)/dMatrixMath.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dVectorMath.o
	mkdir -p $(BIN_DIR)
	$(CC) -shared $(OBJ_DIR)/dLinkedList.o $(OBJ_DIR)/dMatrixMath.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dVectorMath.o -o $(BIN_DIR)/libDaedalus.so $(CFLAGS)


$(OBJ_DIR)/dKinematicBody2D.o: $(SRC_DIR)/dKinematicBody2D.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dLinkedList.o: $(SRC_DIR)/dLinkedList.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dQuadTree.o: $(SRC_DIR)/dQuadTree.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dStrings.o: $(SRC_DIR)/dStrings.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	$(CC) -c $< -o $@ $(CFLAGS)

install:
	sudo cp $(BIN_DIR)/libDaedalus.so /usr/lib/libDaedalus.so
	sudo cp $(INC_DIR)/Daedalus.h /usr/include/Daedalus.h

uninstall:
	sudo rm /usr/lib/libDaedalus.so
	sudo rm /usr/include/Daedalus.h

updateHeader:
	sudo cp $(INC_DIR)/Daedalus.h /usr/include/Daedalus.h


debug: $(OBJ_DIR)/debug_main.o $(OBJ_DIR)/debug_dVectorMath.o $(OBJ_DIR)/debug_dMatrixMath.o $(OBJ_DIR)/debug_dMatrixCreation.o $(OBJ_DIR)/debug_dLinkedList.o
	mkdir -p $(BIN_DIR)
	$(CC) -ggdb $(OBJ_DIR)/debug_main.o $(OBJ_DIR)/debug_dVectorMath.o $(OBJ_DIR)/debug_dMatrixMath.o $(OBJ_DIR)/debug_dMatrixCreation.o $(OBJ_DIR)/debug_dLinkedList.o -o $(BIN_DIR)/$@.o $(CFLAGS)

$(OBJ_DIR)/debug_main.o: $(SRC_DIR)/main.c
	mkdir -p $(OBJ_DIR)
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/debug_dLinkedList.o: $(SRC_DIR)/dLinkedList.c
	$(CC) -ggdb -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	clear
