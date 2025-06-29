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
							$(OBJ_DIR)/dKinematicBody.o\
							$(OBJ_DIR)/dLinkedList.o\
							$(OBJ_DIR)/dLogs.o\
							$(OBJ_DIR)/dMatrixMath.o\
							$(OBJ_DIR)/dStrings-dArrays.o\
							$(OBJ_DIR)/dStrings.o\
							$(OBJ_DIR)/dVectorMath.o\

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ -ggdb $(CINC) $(CFLAGS)

$(BIN_DIR)/debug: $(NATIVE_OBJS) | $(BIN_DIR)
	$(CC) $^ -ggdb $(CINC) $(CFLAGS) -o $@


.PHONY: shared
shared: $(BIN_DIR)/libDaedalus

SHARED_OBJS = \
							$(SHA_DIR)/dArrays.o\
							$(SHA_DIR)/dKinematicBody.o\
							$(SHA_DIR)/dLinkedList.o\
							$(SHA_DIR)/dLogs.o\
							$(SHA_DIR)/dMatrixMath.o\
							$(SHA_DIR)/dStrings-dArrays.o\
							$(SHA_DIR)/dStrings.o\
							$(SHA_DIR)/dVectorMath.o\

$(SHA_DIR)/%.o: $(SRC_DIR)/%.c | $(SHA_DIR)
	$(CC) -c $< $(CINC) -o $@ $(CFLAGS) -fPIC -pedantic

$(BIN_DIR)/libDaedalus: $(SHARED_OBJS)  | $(BIN_DIR)
	$(CC) $^ $(CINC) -shared -fPIC -pedantic  $(CFLAGS) -o $@.so


.PHONY: EM
EM: $(BIN_DIR)/libDaedalus.a

EMS_OBJS = \
							$(EMS_DIR)/dArrays.o\
							$(EMS_DIR)/dKinematicBody.o\
							$(EMS_DIR)/dLinkedList.o\
							$(EMS_DIR)/dLogs.o\
							$(EMS_DIR)/dMatrixMath.o\
							$(EMS_DIR)/dStrings-dArrays.o\
							$(EMS_DIR)/dStrings.o\
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
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(EMS_DIR)
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
all: shared

# =============================================================================
# TESTING
# =============================================================================
TEST_DIR=true_tests
TEST_CFLAGS = -Wall -Wextra -ggdb $(CINC)

# Individual test targets

.PHONY: test-logging-emscripten
test-logging-emscripten: always $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_emscripten $(TEST_DIR)/logging/test_logging_emscripten.c $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o

.PHONY: run-test-logging-emscripten
run-test-logging-emscripten: test-logging-emscripten
	@./$(BIN_DIR)/test_logging_emscripten

.PHONY: test-create-string-from-file
test-create-string-from-file: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_create_string_from_file $(TEST_DIR)/strings/test_create_string_from_file.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o

.PHONY: run-test-create-string-from-file
run-test-create-string-from-file: test-create-string-from-file
	@./$(BIN_DIR)/test_create_string_from_file

.PHONY: test-logging-rate-limited
test-logging-rate-limited: always $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_rate_limited \
		$(TEST_DIR)/logging/test_logging_rate_limited.c \
		$(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o

.PHONY: run-test-logging-rate-limited
run-test-logging-rate-limited: test-logging-rate-limited
	@./$(BIN_DIR)/test_logging_rate_limited

.PHONY: run
run: run-test-create-string-from-file run-test-logging-rate-limited run-test-string-builder run-test-string-builder-edge

.PHONY: test-string-builder
test-string-builder: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_builder $(TEST_DIR)/strings/test_string_builder.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o

.PHONY: run-test-string-builder
run-test-string-builder: test-string-builder
	@./$(BIN_DIR)/test_string_builder

.PHONY: test-string-builder-edge
test-string-builder-edge: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_builder_edge $(TEST_DIR)/strings/test_string_builder_edge.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o

.PHONY: run-test-string-builder-edge
run-test-string-builder-edge: test-string-builder-edge
	@./$(BIN_DIR)/test_string_builder_edge

.PHONE: test-string-advanced
test-string-advanced: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_advanced $(TEST_DIR)/strings/test_string_advanced.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o $(OBJ_DIR)/dArrays.o

.PHONY: run-test-string-advanced
run-test-string-advanced: test-string-advanced
	@./$(BIN_DIR)/test_string_advanced

.PHONY: test-logging-basic
test-logging-basic: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_basic $(TEST_DIR)/logging/test_logging_basic.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o

.PHONY: run-test-logging-basic
run-test-logging-basic: test-logging-basic
	@./$(BIN_DIR)/test_logging_basic

.PHONY: test-logging-advanced
test-logging-advanced: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_advanced $(TEST_DIR)/logging/test_logging_advanced.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o

.PHONY: test-logging-extreme
test-logging-extreme: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_extreme $(TEST_DIR)/logging/test_logging_extreme.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o

.PHONY: test-logging-showcase
test-logging-showcase: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_showcase $(TEST_DIR)/logging/test_logging_showcase.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o

.PHONY: run-test-logging-showcase
run-test-logging-showcase: test-logging-showcase
	@./$(BIN_DIR)/test_logging_showcase

.PHONY: test-logging-structured
test-logging-structured: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_logging_structured $(TEST_DIR)/logging/test_logging_structured.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dLogs.o

.PHONY: run-test-logging-structured
run-test-logging-structured: test-logging-structured
	@./$(BIN_DIR)/test_logging_structured

.PHONY: run-test-logging-advanced
run-test-logging-advanced: test-logging-advanced
	@./$(BIN_DIR)/test_logging_advanced

.PHONY: run-test-logging-extreme
run-test-logging-extreme: test-logging-extreme
	@./$(BIN_DIR)/test_logging_extreme

.PHONY: test-string-padding
test-string-padding: always $(OBJ_DIR)/dStrings.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_padding $(TEST_DIR)/strings/test_string_padding.c $(OBJ_DIR)/dStrings.o

.PHONY: run-test-string-padding
run-test-string-padding: test-string-padding
	@./$(BIN_DIR)/test_string_padding

.PHONY: test-string-pythonic
test-string-pythonic: always $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings-dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_string_pythonic $(TEST_DIR)/strings/test_string_pythonic.c $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings-dArrays.o

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
test-dynamic-array-edge: always $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_edge $(TEST_DIR)/dynamicarrays/test_dynamic_array_edge.c $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o

.PHONY: run-test-dynamic-array-edge
run-test-dynamic-array-edge: test-dynamic-array-edge
	@./$(BIN_DIR)/test_dynamic_array_edge

.PHONY: test-dynamic-array-resize
test-dynamic-array-resize: always $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_resize $(TEST_DIR)/dynamicarrays/test_dynamic_array_resize.c $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o

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

.PHONY: test-dynamic-array-debug-hunting
test-dynamic-array-debug-hunting: always $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_debug_hunting $(TEST_DIR)/dynamicarrays/test_dynamic_array_debug_hunting.c $(OBJ_DIR)/dArrays.o $(OBJ_DIR)/dStrings.o $(OBJ_DIR)/dLogs.o

.PHONY: run-test-dynamic-array-debug-hunting
run-test-dynamic-array-debug-hunting: test-dynamic-array-debug-hunting
	@./$(BIN_DIR)/test_dynamic_array_debug_hunting

# Global test runner (summary output)
.PHONY: test-dynamic-array-errors
test-dynamic-array-errors: always $(OBJ_DIR)/dArrays.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_dynamic_array_errors $(TEST_DIR)/dynamicarrays/test_dynamic_array_errors.c $(OBJ_DIR)/dArrays.o

# Emscripten test targets using the static library
.PHONY: test-logging-emscripten-web
test-logging-emscripten-web: EM $(INDEX_DIR)
	$(ECC) $(CINC) -O2 \
		-s WASM=1 \
		-s EXPORTED_FUNCTIONS='["_main"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s INITIAL_MEMORY=16MB \
		-s STACK_SIZE=1MB \
		--preload-file . \
		true_tests/logging/test_logging_emscripten.c \
		$(BIN_DIR)/libDaedalus.a \
		-o $(INDEX_DIR)/test_logging_emscripten.html

.PHONY: test-logging-advanced-emscripten-web  
test-logging-advanced-emscripten-web: EM $(INDEX_DIR)
	$(ECC) $(CINC) -O2 \
		-s WASM=1 \
		-s EXPORTED_FUNCTIONS='["_main"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s INITIAL_MEMORY=32MB \
		-s STACK_SIZE=2MB \
		--preload-file . \
		true_tests/logging/test_logging_advanced.c \
		$(BIN_DIR)/libDaedalus.a \
		-o $(INDEX_DIR)/test_logging_advanced_emscripten.html

# Clean web artifacts
.PHONY: clean-web
clean-web:
	rm -rf $(INDEX_DIR)

# Serve tests locally for browser testing
.PHONY: serve-emscripten-tests
serve-emscripten-tests: test-logging-emscripten-web
	@echo "🌐 Starting local web server for Emscripten tests..."
	@echo "📍 Navigate to: http://localhost:8000/"
	@cd $(INDEX_DIR) && python3 -m http.server 8000
# Global test runner (summary output)
# Traditional approach (current - kept for compatibility)
.PHONY: test
test:
	@./run_tests.sh
