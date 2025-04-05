CC=gcc
CINC = -Iinclude/
CFLAGS = -std=c99 -Wall -Wextra -lm

SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
OBJ_DIR=obj
LIB_DIR=lib

.PHONY: shared
shared: always $(BIN_DIR)/libDaedalus

$(OBJ_DIR)/dKinematicBody2D.o: $(SRC_DIR)/dKinematicBody2D.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(OBJ_DIR)/dLinkedList.o: $(SRC_DIR)/dLinkedList.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(OBJ_DIR)/dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(OBJ_DIR)/dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(OBJ_DIR)/dQuadTree.o: $(SRC_DIR)/dQuadTree.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(OBJ_DIR)/dStrings.o: $(SRC_DIR)/dStrings.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(OBJ_DIR)/dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(BIN_DIR)/libDaedalus: $(OBJ_DIR)/dLinkedList.o $(OBJ_DIR)/dMatrixMath.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dVectorMath.o
	$(CC) $^ $(CINC) -shared -fPIC -pedantic  $(CFLAGS) -o $@.so

.PHONY: native
native: always $(BIN_DIR)/debug_bin

$(OBJ_DIR)/debug_main.o: $(SRC_DIR)/main.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -ggdb

$(OBJ_DIR)/debug_dVectorMath.o: $(SRC_DIR)/dVectorMath.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -ggdb

$(OBJ_DIR)/debug_dMatrixMath.o: $(SRC_DIR)/dMatrixMath.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -ggdb

$(OBJ_DIR)/debug_dMatrixCreation.o: $(SRC_DIR)/dMatrixCreation.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -ggdb

$(OBJ_DIR)/debug_dLinkedList.o: $(SRC_DIR)/dLinkedList.c
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -ggdb

$(BIN_DIR)/debug_bin: $(OBJ_DIR)/debug_dLinkedList.o $(OBJ_DIR)/debug_dMatrixMath.o $(OBJ_DIR)/debug_dVectorMath.o $(OBJ_DIR)/debug_main.o
	$(CC) $^ $(CINC) $(CFLAGS) -ggdb -o $@


.PHONY: install
install:
	sudo cp $(BIN_DIR)/libDaedalus.so /usr/lib/libDaedalus.so
	sudo cp $(INC_DIR)/Daedalus.h /usr/include/Daedalus.h

.PHONY: uninstall
uninstall:
	sudo rm /usr/lib/libDaedalus.so
	sudo rm /usr/include/Daedalus.h

.PHONY: updateHeader
updateHeader:
	sudo cp $(INC_DIR)/Daedalus.h /usr/include/Daedalus.h

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)
	clear

.PHONY: bear
bear:
	bear -- make

.PHONY: bearclean
bearclean:
	rm compile_commands.json

.PHONY: always
always:
	mkdir -p $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR)

.PHONY: all
all: always native shared

