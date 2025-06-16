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

$(OBJ_DIR)/dArrays.o: $(SRC_DIR)/dArrays.c
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

# =============================================================================
# TESTING
# =============================================================================
TEST_DIR=true_tests
TEST_CFLAGS = -Wall -Wextra -ggdb $(CINC)

# Individual test targets
.PHONY: test-create-string-from-file
test-create-string-from-file: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_create_string_from_file $(TEST_DIR)/strings/test_create_string_from_file.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-create-string-from-file
run-test-create-string-from-file: test-create-string-from-file
	@./$(BIN_DIR)/test_create_string_from_file

.PHONY: test-string-builder
test-string-builder: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_builder $(TEST_DIR)/strings/test_string_builder.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-string-builder
run-test-string-builder: test-string-builder
	@./$(BIN_DIR)/test_string_builder

.PHONY: test-string-builder-edge
test-string-builder-edge: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_builder_edge $(TEST_DIR)/strings/test_string_builder_edge.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-string-builder-edge
run-test-string-builder-edge: test-string-builder-edge
	@./$(BIN_DIR)/test_string_builder_edge

.PHONE: test-string-advanced
test-string-advanced: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_advanced $(TEST_DIR)/strings/test_string_advanced.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-string-advanced
run-test-string-advanced: test-string-advanced
	@./$(BIN_DIR)/test_string_advanced

.PHONY: test-string-padding
test-string-padding: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_padding $(TEST_DIR)/strings/test_string_padding.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-string-padding
run-test-string-padding: test-string-padding
	@./$(BIN_DIR)/test_string_padding

.PHONY: test-string-pythonic
test-string-pythonic: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_pythonic $(TEST_DIR)/strings/test_string_pythonic.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-string-pythonic
run-test-string-pythonic: test-string-pythonic
	@./$(BIN_DIR)/test_string_pythonic

.PHONY: test-dynamic-array-basic
test-dynamic-array-basic: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_basic $(TEST_DIR)/dynamicarrays/test_dynamic_array_basic.c $(OBJ_DIR)/dArrays.o

.PHONY: run-test-dynamic-array-basic
run-test-dynamic-array-basic: test-dynamic-array-basic
	@./$(BIN_DIR)/test_dynamic_array_basic

.PHONY: test-dynamic-array-edge
test-dynamic-array-edge: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_edge $(TEST_DIR)/dynamicarrays/test_dynamic_array_edge.c $(OBJ_DIR)/dArrays.o

.PHONY: run-test-dynamic-array-edge
run-test-dynamic-array-edge: test-dynamic-array-edge
	@./$(BIN_DIR)/test_dynamic_array_edge

.PHONY: test-dynamic-array-resize
test-dynamic-array-resize: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_resize $(TEST_DIR)/dynamicarrays/test_dynamic_array_resize.c $(OBJ_DIR)/dArrays.o

.PHONY: run-test-dynamic-array-resize
run-test-dynamic-array-resize: test-dynamic-array-resize
	@./$(BIN_DIR)/test_dynamic_array_resize

.PHONY: test-dynamic-array-performance
test-dynamic-array-performance: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_performance $(TEST_DIR)/dynamicarrays/test_dynamic_array_performance.c $(OBJ_DIR)/dArrays.o

.PHONY: run-test-dynamic-array-performance
run-test-dynamic-array-performance: test-dynamic-array-performance
	@./$(BIN_DIR)/test_dynamic_array_performance

.PHONY: test-dynamic-array-advanced
test-dynamic-array-advanced: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_advanced $(TEST_DIR)/dynamicarrays/test_dynamic_array_advanced.c $(OBJ_DIR)/dArrays.o

.PHONY: run-test-dynamic-array-advanced
run-test-dynamic-array-advanced: test-dynamic-array-advanced
	@./$(BIN_DIR)/test_dynamic_array_advanced

.PHONY: run-test-dynamic-array-errors
run-test-dynamic-array-errors: test-dynamic-array-errors
	@./$(BIN_DIR)/test_dynamic_array_errors

.PHONY: test-dynamic-array-errors
test-dynamic-array-errors: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_errors $(TEST_DIR)/dynamicarrays/test_dynamic_array_errors.c $(OBJ_DIR)/dArrays.o

# Test help
.PHONY: test-help
test-help:
	@echo "Available test commands:"
	@echo "  make test                              - Run all tests with global summary"
	@echo "  make run-test-create-string-from-file  - Run test for create_string_from_file"
	@echo "  make run-test-string-builder           - Run test for string builder"
	@echo "  make run-test-string-builder-edge      - Run test for string builder edge cases"
	@echo "  make run-test-string-advanced          - Run test for advanced string operations"
	@echo "  make run-test-string-padding           - Run test for string padding"
	@echo "  make run-test-string-pythonic          - Run test for string pythonic"
	@echo "  make run-test-dynamic-array-basic      - Run test for basic dynamic array operations"
	@echo "  make run-test-dynamic-array-edge       - Run test for edge cases in dynamic array operations"
	@echo "  make run-test-dynamic-array-resize     - Run test for resizing dynamic array operations"
	@echo "  make run-test-dynamic-array-performance - Run test for performance of dynamic array operations"
	@echo "  make run-test-dynamic-array-advanced   - Run test for advanced dynamic array operations"
	@echo "  make run-test-dynamic-array-errors     - Run test for error handling in dynamic array operations"

# Global test runner (summary output)
# Traditional approach (current - kept for compatibility)
.PHONY: test
test:
	@./run_tests.sh

# Fast unified approach (new)
.PHONY: test-fast
test-fast: run-test-unified
# =============================================================================
# UNIFIED TEST RUNNER (New Approach)
# =============================================================================

# Unified test runner that runs all tests in one binary
.PHONY: test-runner-unified
test-runner-unified: always $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_runner_unified \
		$(TEST_DIR)/unified/test_runner.c \
		$(TEST_DIR)/unified/all_tests.c \
		$(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o

.PHONY: run-test-unified
run-test-unified: test-runner-unified
	@./$(BIN_DIR)/test_runner_unified
