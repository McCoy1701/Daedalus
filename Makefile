CC=gcc
ECC=emcc
EMAR = emar rcs
CINC = -Iinclude/
CFLAGS = -std=c99 -Wall -Wextra -lm

# Emscripten web deployment targets
INDEX_DIR=index

$(INDEX_DIR):
	mkdir -p $(INDEX_DIR)

SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
OBJ_DIR=obj
EMS_DIR=ems_obj
SHA_DIR=shared_obj
LIB_DIR=lib

.PHONY: native
native: $(BIN_DIR)/debug

NATIVE_OBJS = \
							$(OBJ_DIR)/main.o\
							$(OBJ_DIR)/dArrays.o\
							$(OBJ_DIR)/dDUFIO.o\
							$(OBJ_DIR)/dDUFLexer.o\
							$(OBJ_DIR)/dDUFParser.o\
							$(OBJ_DIR)/dDUFQuery.o\
							$(OBJ_DIR)/dDUFValue.o\
							$(OBJ_DIR)/dFunctions.o\
							$(OBJ_DIR)/dKinematicBody.o\
							$(OBJ_DIR)/dLinkedList.o\
							$(OBJ_DIR)/dLogs.o\
							$(OBJ_DIR)/dMatrixMath.o\
							$(OBJ_DIR)/dStaticArrays.o\
							$(OBJ_DIR)/dStaticTables.o\
							$(OBJ_DIR)/dStrings-dArrays.o\
							$(OBJ_DIR)/dStrings.o\
							$(OBJ_DIR)/dTables.o\
							$(OBJ_DIR)/dVectorMath.o\

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ -ggdb $(CINC) $(CFLAGS)

$(BIN_DIR)/debug: $(NATIVE_OBJS) | $(BIN_DIR)
	$(CC) $^ -ggdb $(CINC) $(CFLAGS) -o $@


.PHONY: shared
shared: $(BIN_DIR)/libDaedalus

SHARED_OBJS = \
							$(SHA_DIR)/dArrays.o\
							$(SHA_DIR)/dDUFIO.o\
							$(SHA_DIR)/dDUFLexer.o\
							$(SHA_DIR)/dDUFParser.o\
							$(SHA_DIR)/dDUFQuery.o\
							$(SHA_DIR)/dDUFValue.o\
							$(SHA_DIR)/dFunctions.o\
							$(SHA_DIR)/dKinematicBody.o\
							$(SHA_DIR)/dLinkedList.o\
							$(SHA_DIR)/dLogs.o\
							$(SHA_DIR)/dMatrixMath.o\
							$(SHA_DIR)/dStaticArrays.o\
							$(SHA_DIR)/dStaticTables.o\
							$(SHA_DIR)/dStrings-dArrays.o\
							$(SHA_DIR)/dStrings.o\
							$(SHA_DIR)/dTables.o\
							$(SHA_DIR)/dVectorMath.o\

$(SHA_DIR)/%.o: $(SRC_DIR)/%.c | $(SHA_DIR)
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(BIN_DIR)/libDaedalus: $(SHARED_OBJS)  | $(BIN_DIR)
	$(CC) $^ $(CINC) -shared -fPIC -pedantic  $(CFLAGS) -o $@.so


.PHONY: EM
EM: $(BIN_DIR)/libDaedalus.a

EMS_OBJS = \
							$(EMS_DIR)/dArrays.o\
							$(EMS_DIR)/dDUFIO.o\
							$(EMS_DIR)/dDUFLexer.o\
							$(EMS_DIR)/dDUFParser.o\
							$(EMS_DIR)/dDUFQuery.o\
							$(EMS_DIR)/dDUFValue.o\
							$(EMS_DIR)/dFunctions.o\
							$(EMS_DIR)/dKinematicBody.o\
							$(EMS_DIR)/dLinkedList.o\
							$(EMS_DIR)/dLogs.o\
							$(EMS_DIR)/dMatrixMath.o\
							$(EMS_DIR)/dStaticArrays.o\
							$(EMS_DIR)/dStaticTables.o\
							$(EMS_DIR)/dStrings-dArrays.o\
							$(EMS_DIR)/dStrings.o\
							$(EMS_DIR)/dTables.o\
							$(EMS_DIR)/dVectorMath.o\

$(EMS_DIR)/%.o: $(SRC_DIR)/%.c | $(EMS_DIR)
	$(ECC) -c $< $(CINC) -o $@

$(BIN_DIR)/libDaedalus.a: $(EMS_OBJS) | $(BIN_DIR)
	$(EMAR) $@ $^


$(EMS_DIR):
	mkdir -p $(EMS_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SHA_DIR):
	mkdir -p $(SHA_DIR)


.PHONY: install
install:
	sudo cp $(BIN_DIR)/libDaedalus.so /usr/lib/
	sudo cp $(INC_DIR)/Daedalus.h /usr/include/

.PHONY: uninstall
uninstall:
	sudo rm /usr/lib/libDaedalus.so
	sudo rm /usr/include/Daedalus.h

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(EMS_DIR) $(SHA_DIR)
	clear

.PHONY: bear
bear:
	bear -- make

.PHONY: bearclean
bearclean:
	rm compile_commands.json

.PHONY: verify
verify:
	./verify_architecture.sh

.PHONY: always
always:
	mkdir -p $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR)

.PHONY: all
all: shared

# Test target for DUF parser
.PHONY: test_duf
test_duf: $(BIN_DIR)/test_duf $(BIN_DIR)/test_edge_cases
	$(BIN_DIR)/test_duf
	$(BIN_DIR)/test_edge_cases

TEST_DUF_OBJS = \
							$(OBJ_DIR)/dArrays.o\
							$(OBJ_DIR)/dDUFIO.o\
							$(OBJ_DIR)/dDUFLexer.o\
							$(OBJ_DIR)/dDUFParser.o\
							$(OBJ_DIR)/dDUFQuery.o\
							$(OBJ_DIR)/dDUFValue.o\
							$(OBJ_DIR)/dFunctions.o\
							$(OBJ_DIR)/dKinematicBody.o\
							$(OBJ_DIR)/dLinkedList.o\
							$(OBJ_DIR)/dLogs.o\
							$(OBJ_DIR)/dMatrixMath.o\
							$(OBJ_DIR)/dStaticArrays.o\
							$(OBJ_DIR)/dStaticTables.o\
							$(OBJ_DIR)/dStrings-dArrays.o\
							$(OBJ_DIR)/dStrings.o\
							$(OBJ_DIR)/dTables.o\
							$(OBJ_DIR)/dVectorMath.o\

$(BIN_DIR)/test_duf: tests/test_duf.c $(TEST_DUF_OBJS) | $(BIN_DIR)
	$(CC) $^ -ggdb $(CINC) $(CFLAGS) -o $@

# Test target for DUF edge cases
.PHONY: test_edge_cases
test_edge_cases: $(BIN_DIR)/test_edge_cases
	$(BIN_DIR)/test_edge_cases

$(BIN_DIR)/test_edge_cases: tests/test_edge_cases.c $(TEST_DUF_OBJS) | $(BIN_DIR)
	$(CC) $^ -ggdb $(CINC) $(CFLAGS) -o $@

# Run all DUF tests
.PHONY: test_duf_all
test_duf_all: test_duf test_edge_cases