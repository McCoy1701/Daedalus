// File: true_tests/staticarrays/test_static_array_save_load.c - Static array save/load functionality tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS FOR TESTING
// =============================================================================

// Test iterator callback to count elements
static void count_elements_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element; (void)element_size; // Unused parameters
    int* count = (int*)user_data;
    (*count)++;
}

// Test iterator callback to verify data integrity
typedef struct {
    int* expected_values;
    int num_expected;
    int matches_found;
} VerifyData_t;

static void verify_data_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size; // Unused parameter
    VerifyData_t* verify = (VerifyData_t*)user_data;
    
    // Skip verification if expected_values is NULL (used for simple counting)
    if (!verify->expected_values) {
        verify->matches_found++;
        return;
    }
    
    int value = *(int*)element;
    
    if (index < (size_t)verify->num_expected && verify->expected_values[index] == value) {
        verify->matches_found++;
    }
}

// Helper to clean up test files
static void cleanup_test_file(const char* filename) {
    if (access(filename, F_OK) == 0) {
        remove(filename);
    }
}

// =============================================================================
// SUCCESS TESTS
// =============================================================================

int test_save_load_basic_success(void)
{
    const char* test_file = "test_static_array_basic.dat";
    cleanup_test_file(test_file);
    
    // Create static array with known data
    dStaticArray_t* original = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(original != NULL, "Should create original array successfully");
    
    // Add known values
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(original, &values[i]) == 0, "Should append values successfully");
    }
    
    // Save to file
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, original) == 0, "Should save array to file successfully");
    
    // Load from file
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load array from file successfully");
    
    // Verify loaded array has same properties
    TEST_ASSERT(loaded->capacity == 10, "Loaded array should have capacity 10");
    TEST_ASSERT(loaded->count == 5, "Loaded array should have count 5");
    TEST_ASSERT(loaded->element_size == sizeof(int), "Loaded array should have correct element size");
    
    // Verify all original data is present
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_IndexDataFromStaticArray(loaded, i);
        TEST_ASSERT(value != NULL && *value == values[i], "All original values should be preserved in loaded array");
    }
    
    // Test iterator on loaded array
    int element_count = 0;
    TEST_ASSERT(d_IterateStaticArray(loaded, count_elements_callback, &element_count) == 0, "Should iterate over loaded array successfully");
    TEST_ASSERT(element_count == 5, "Should iterate over exactly 5 elements");
    
    d_DestroyStaticArray(original);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_save_load_different_types_success(void)
{
    const char* test_file = "test_static_array_types.dat";
    cleanup_test_file(test_file);
    
    // Test with float array
    dStaticArray_t* float_array = d_InitStaticArray(8, sizeof(float));
    TEST_ASSERT(float_array != NULL, "Should create float array successfully");
    
    float float_values[] = {1.5f, 2.7f, 3.14f, 4.0f};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(float_array, &float_values[i]) == 0, "Should append float values successfully");
    }
    
    // Save to file
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, float_array) == 0, "Should save float array to file successfully");
    
    // Load from file
    dStaticArray_t* loaded_float = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded_float != NULL, "Should load float array from file successfully");
    
    // Verify loaded array has same properties
    TEST_ASSERT(loaded_float->capacity == 8, "Loaded float array should have capacity 8");
    TEST_ASSERT(loaded_float->count == 4, "Loaded float array should have count 4");
    TEST_ASSERT(loaded_float->element_size == sizeof(float), "Loaded float array should have correct element size");
    
    // Verify all original float data is present
    for (int i = 0; i < 4; i++) {
        float* value = (float*)d_IndexDataFromStaticArray(loaded_float, i);
        TEST_ASSERT(value != NULL && *value == float_values[i], "All original float values should be preserved in loaded array");
    }
    
    // Test iterator with data verification (just count elements for float test)
    VerifyData_t verify_data = {NULL, 4, 0};
    TEST_ASSERT(d_IterateStaticArray(loaded_float, verify_data_callback, &verify_data) == 0, "Should iterate over loaded float array successfully");
    TEST_ASSERT(verify_data.matches_found == 4, "Should find all 4 elements in float array");
    
    d_DestroyStaticArray(float_array);
    d_DestroyStaticArray(loaded_float);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// BAD PATH TESTS
// =============================================================================

int test_save_load_invalid_parameters(void)
{
    const char* test_file = "test_static_array_invalid.dat";
    cleanup_test_file(test_file);
    
    // Create a valid array for testing
    dStaticArray_t* array = d_InitStaticArray(5, sizeof(int));
    int value = 42;
    d_AppendDataToStaticArray(array, &value);
    
    // Test save with invalid parameters
    TEST_ASSERT(d_SaveStaticArrayToFile(NULL, array) == 1, "Save should fail with NULL filename");
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, NULL) == 1, "Save should fail with NULL array");
    TEST_ASSERT(d_SaveStaticArrayToFile("", array) == 1, "Save should fail with empty filename");
    
    // Test load with invalid parameters
    TEST_ASSERT(d_LoadStaticArrayFromFile(NULL) == NULL, "Load should fail with NULL filename");
    TEST_ASSERT(d_LoadStaticArrayFromFile("nonexistent_file.dat") == NULL, "Load should fail with nonexistent file");
    
    // Test iterator with invalid parameters
    TEST_ASSERT(d_IterateStaticArray(NULL, count_elements_callback, NULL) == 1, "Iterate should fail with NULL array");
    TEST_ASSERT(d_IterateStaticArray(array, NULL, NULL) == 1, "Iterate should fail with NULL callback");
    
    d_DestroyStaticArray(array);
    cleanup_test_file(test_file);
    return 1;
}

int test_save_load_corrupted_file(void)
{
    const char* test_file = "test_static_array_corrupted.dat";
    cleanup_test_file(test_file);
    
    // Create a file with invalid content
    FILE* corrupt_file = fopen(test_file, "wb");
    TEST_ASSERT(corrupt_file != NULL, "Should create corrupt test file");
    
    // Write wrong magic number
    uint32_t wrong_magic = 0xDEADBEEF;
    fwrite(&wrong_magic, sizeof(uint32_t), 1, corrupt_file);
    fclose(corrupt_file);
    
    // Try to load corrupted file
    dStaticArray_t* loaded1 = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded1 == NULL, "Should fail to load file with wrong magic number");
    
    // Create file with correct magic but wrong version
    corrupt_file = fopen(test_file, "wb");
    uint32_t correct_magic = 0xDAEDDDCA;
    uint32_t wrong_version = 999;
    fwrite(&correct_magic, sizeof(uint32_t), 1, corrupt_file);
    fwrite(&wrong_version, sizeof(uint32_t), 1, corrupt_file);
    fclose(corrupt_file);
    
    dStaticArray_t* loaded2 = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded2 == NULL, "Should fail to load file with wrong version");
    
    // Create file with truncated data
    corrupt_file = fopen(test_file, "wb");
    uint32_t correct_version = 1;
    fwrite(&correct_magic, sizeof(uint32_t), 1, corrupt_file);
    fwrite(&correct_version, sizeof(uint32_t), 1, corrupt_file);
    // Don't write complete metadata - file will be truncated
    fclose(corrupt_file);
    
    dStaticArray_t* loaded3 = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded3 == NULL, "Should fail to load truncated file");
    
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// MULTIPLE SAVE/LOAD TESTS WITH DATA ALTERATIONS
// =============================================================================

int test_save_load_multiple_cycles_with_updates(void)
{
    const char* test_file = "test_static_array_cycles.dat";
    cleanup_test_file(test_file);
    
    // Create initial array
    dStaticArray_t* array1 = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(array1 != NULL, "Should create initial array successfully");
    
    int original_values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(array1, &original_values[i]) == 0, "Should append original values");
    }
    
    // === CYCLE 1: Save original data ===
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, array1) == 0, "Should save original array successfully");
    
    dStaticArray_t* loaded1 = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded1 != NULL, "Should load array from cycle 1 successfully");
    
    // Add more values to loaded array
    int updated_values_cycle1[] = {100, 200};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(loaded1, &updated_values_cycle1[i]) == 0, "Should append values in cycle 1");
    }
    
    // === CYCLE 2: Save updated data ===
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, loaded1) == 0, "Should save updated array from cycle 1");
    
    dStaticArray_t* loaded2 = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded2 != NULL, "Should load array from cycle 2 successfully");
    TEST_ASSERT(loaded2->count == 7, "Should have 7 elements after cycle 1 updates");
    
    // Verify cycle 1 updates are preserved
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_IndexDataFromStaticArray(loaded2, i);
        TEST_ASSERT(value != NULL && *value == original_values[i], "Should preserve original values");
    }
    for (int i = 0; i < 2; i++) {
        int* value = (int*)d_IndexDataFromStaticArray(loaded2, i + 5);
        TEST_ASSERT(value != NULL && *value == updated_values_cycle1[i], "Should preserve cycle 1 updates");
    }
    
    // Add more values again
    int updated_values_cycle2[] = {1000, 2000};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(loaded2, &updated_values_cycle2[i]) == 0, "Should append values in cycle 2");
    }
    
    // === CYCLE 3: Save and verify final data ===
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, loaded2) == 0, "Should save updated array from cycle 2");
    
    dStaticArray_t* loaded3 = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded3 != NULL, "Should load array from cycle 3 successfully");
    TEST_ASSERT(loaded3->count == 9, "Should have 9 elements after all updates");
    
    // Verify final updates are preserved
    for (int i = 0; i < 2; i++) {
        int* value = (int*)d_IndexDataFromStaticArray(loaded3, i + 7);
        TEST_ASSERT(value != NULL && *value == updated_values_cycle2[i], "Should preserve cycle 2 updates");
    }
    
    // Test iterator on final array
    int final_element_count = 0;
    TEST_ASSERT(d_IterateStaticArray(loaded3, count_elements_callback, &final_element_count) == 0, "Should iterate over final array successfully");
    TEST_ASSERT(final_element_count == 9, "Should iterate over all 9 elements in final array");
    
    d_DestroyStaticArray(array1);
    d_DestroyStaticArray(loaded1);
    d_DestroyStaticArray(loaded2);
    d_DestroyStaticArray(loaded3);
    cleanup_test_file(test_file);
    return 1;
}

int test_save_load_large_dataset_consistency(void)
{
    const char* test_file = "test_static_array_large.dat";
    cleanup_test_file(test_file);
    
    const size_t num_elements = 100;
    
    // Create large array
    dStaticArray_t* large_array = d_InitStaticArray(num_elements + 10, sizeof(int));
    TEST_ASSERT(large_array != NULL, "Should create large array successfully");
    
    // Fill with pattern data
    LOOP_TEST_START();
    for (size_t i = 0; i < num_elements; i++) {
        int value = (int)i * 3; // Pattern: 0, 3, 6, 9, ...
        TEST_ASSERT(d_AppendDataToStaticArray(large_array, &value) == 0, "Should append large dataset values");
    }
    LOOP_TEST_END();
    
    // === PHASE 1: Save and load original data ===
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, large_array) == 0, "Should save large array successfully");
    
    dStaticArray_t* loaded_large = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded_large != NULL, "Should load large array successfully");
    TEST_ASSERT(loaded_large->count == num_elements, "Should load correct number of elements");
    
    // Verify all original data
    LOOP_TEST_START();
    for (size_t i = 0; i < num_elements; i++) {
        int* value = (int*)d_IndexDataFromStaticArray(loaded_large, i);
        int expected = (int)i * 3;
        TEST_ASSERT(value != NULL && *value == expected, "Should preserve all original values in large dataset");
    }
    LOOP_TEST_END();
    
    // === PHASE 2: Update every other value ===
    for (size_t i = 0; i < num_elements; i += 2) {
        int new_value = (int)i * 10; // Update even indices with different pattern
        int* element_ptr = (int*)d_IndexDataFromStaticArray(loaded_large, i);
        *element_ptr = new_value;
    }
    
    // === PHASE 3: Save and load updated data ===
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, loaded_large) == 0, "Should save updated large array successfully");
    
    dStaticArray_t* final_large = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(final_large != NULL, "Should load final large array successfully");
    
    // Verify selective updates
    LOOP_TEST_START();
    for (size_t i = 0; i < num_elements; i++) {
        int* value = (int*)d_IndexDataFromStaticArray(final_large, i);
        int expected = (i % 2 == 0) ? (int)i * 10 : (int)i * 3; // Even indices updated, odd preserved
        TEST_ASSERT(value != NULL && *value == expected, "Should preserve selective updates in large dataset");
    }
    LOOP_TEST_END();
    
    // === PHASE 4: Test iterator on large dataset ===
    int large_element_count = 0;
    TEST_ASSERT(d_IterateStaticArray(final_large, count_elements_callback, &large_element_count) == 0, "Should iterate over large dataset successfully");
    TEST_ASSERT(large_element_count == (int)num_elements, "Should iterate over all elements in large dataset");
    
    // Cleanup
    d_DestroyStaticArray(large_array);
    d_DestroyStaticArray(loaded_large);
    d_DestroyStaticArray(final_large);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// DEBUG TESTS - ADVANCED INTEGRATION AND CALLBACK EXPLORATION
// =============================================================================

// Advanced callback that logs all elements and calculates statistics
typedef struct {
    int total_elements;
    int sum_values;
    int min_value;
    int max_value;
    bool first_element;
} StatsData_t;

static void stats_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element_size; // Unused parameters
    StatsData_t* stats = (StatsData_t*)user_data;
    int value = *(int*)element;
    
    stats->total_elements++;
    stats->sum_values += value;
    
    if (stats->first_element) {
        stats->min_value = stats->max_value = value;
        stats->first_element = false;
    } else {
        if (value < stats->min_value) stats->min_value = value;
        if (value > stats->max_value) stats->max_value = value;
    }
}

// Callback that collects all values into a dynamically allocated array
typedef struct {
    int* collected_values;
    int capacity;
    int count;
} ValueCollector_t;

static void value_collector_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element_size; // Unused parameters
    ValueCollector_t* collector = (ValueCollector_t*)user_data;
    int value = *(int*)element;
    
    if (collector->count >= collector->capacity) {
        // Expand array if needed
        collector->capacity *= 2;
        collector->collected_values = realloc(collector->collected_values, collector->capacity * sizeof(int));
    }
    
    collector->collected_values[collector->count++] = value;
}

// Callback that searches for specific values
typedef struct {
    int target_value;
    bool found_match;
    size_t found_index;
    int search_count;
} SearchData_t;

static void search_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size; // Unused parameter
    SearchData_t* search = (SearchData_t*)user_data;
    int value = *(int*)element;
    
    search->search_count++;
    if (value == search->target_value && !search->found_match) {
        search->found_match = true;
        search->found_index = index;
    }
}

int test_debug_stats_with_save_load(void)
{
    const char* test_file = "test_debug_stats.dat";
    cleanup_test_file(test_file);
    
    // Create array with known statistical properties
    dStaticArray_t* original = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(original != NULL, "Should create original array for stats test");
    
    int values[] = {5, 15, 25, 35, 45, 55, 65, 75};
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(original, &values[i]) == 0, "Should append stats test values");
    }
    
    // Get original statistics
    StatsData_t original_stats = {0, 0, 0, 0, true};
    TEST_ASSERT(d_IterateStaticArray(original, stats_callback, &original_stats) == 0, "Should collect original stats");
    TEST_ASSERT(original_stats.total_elements == 8, "Should count 8 elements in original");
    TEST_ASSERT(original_stats.min_value == 5 && original_stats.max_value == 75, "Should find correct value range");
    
    // Save array
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, original) == 0, "Should save stats array");
    
    // Load and verify statistics are preserved
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load stats array");
    
    StatsData_t loaded_stats = {0, 0, 0, 0, true};
    TEST_ASSERT(d_IterateStaticArray(loaded, stats_callback, &loaded_stats) == 0, "Should collect loaded stats");
    TEST_ASSERT(loaded_stats.total_elements == original_stats.total_elements, "Should preserve element count through save/load");
    TEST_ASSERT(loaded_stats.sum_values == original_stats.sum_values, "Should preserve value sum through save/load");
    
    d_DestroyStaticArray(original);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_debug_value_collection_with_save_load(void)
{
    const char* test_file = "test_debug_values.dat";
    cleanup_test_file(test_file);
    
    // Create array with specific values for collection testing
    dStaticArray_t* original = d_InitStaticArray(8, sizeof(int));
    TEST_ASSERT(original != NULL, "Should create original array for value collection test");
    
    int values[] = {100, 200, 300, 400, 500, 600};
    for (int i = 0; i < 6; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(original, &values[i]) == 0, "Should append collection test values");
    }
    
    // Collect original values using callback
    ValueCollector_t original_collector = {malloc(10 * sizeof(int)), 10, 0};
    TEST_ASSERT(d_IterateStaticArray(original, value_collector_callback, &original_collector) == 0, "Should collect original values");
    TEST_ASSERT(original_collector.count == 6, "Should collect all 6 values");
    
    // Save array
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, original) == 0, "Should save collection array");
    
    // Load and collect values again
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load collection array");
    
    ValueCollector_t loaded_collector = {malloc(10 * sizeof(int)), 10, 0};
    TEST_ASSERT(d_IterateStaticArray(loaded, value_collector_callback, &loaded_collector) == 0, "Should collect loaded values");
    TEST_ASSERT(loaded_collector.count == 6, "Should collect all 6 values after save/load");
    
    // Verify all original values are present in correct order
    for (int i = 0; i < 6; i++) {
        TEST_ASSERT(loaded_collector.collected_values[i] == values[i], "Should preserve value order after save/load");
    }
    
    free(original_collector.collected_values);
    free(loaded_collector.collected_values);
    d_DestroyStaticArray(original);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_debug_search_callback_with_save_load(void)
{
    const char* test_file = "test_debug_search.dat";
    cleanup_test_file(test_file);
    
    // Create array for search testing
    dStaticArray_t* array = d_InitStaticArray(8, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create array for search test");
    
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(array, &values[i]) == 0, "Should append search test values");
    }
    
    // Search for existing value
    SearchData_t search1 = {30, false, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(array, search_callback, &search1) == 0, "Should search in original array");
    TEST_ASSERT(search1.found_match == true, "Should find existing value");
    TEST_ASSERT(search1.found_index == 2, "Should find value at correct index");
    TEST_ASSERT(search1.search_count == 5, "Should check all elements during search");
    
    // Update the value we just found
    int* element_ptr = (int*)d_IndexDataFromStaticArray(array, 2);
    *element_ptr = 3000;
    
    // Search for old value (should not find)
    SearchData_t search2 = {30, false, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(array, search_callback, &search2) == 0, "Should search after update");
    TEST_ASSERT(search2.found_match == false, "Should not find old value after update");
    
    // Search for new value (should find)
    SearchData_t search3 = {3000, false, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(array, search_callback, &search3) == 0, "Should search for new value");
    TEST_ASSERT(search3.found_match == true, "Should find new value after update");
    
    // Save updated array
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, array) == 0, "Should save updated array");
    
    // Load and verify search still works
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load updated array");
    
    SearchData_t search4 = {3000, false, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(loaded, search_callback, &search4) == 0, "Should search in loaded array");
    TEST_ASSERT(search4.found_match == true, "Should find updated value in loaded array");
    
    // Search for non-existent value
    SearchData_t search5 = {999, false, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(loaded, search_callback, &search5) == 0, "Should search for non-existent value");
    TEST_ASSERT(search5.found_match == false, "Should not find non-existent value");
    
    d_DestroyStaticArray(array);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// DEBUG-HUNTING TESTS - ADVANCED BUG DETECTION
// =============================================================================

// Memory boundary violation detector
typedef struct {
    size_t elements_checked;
    size_t boundary_violations;
    size_t null_access_attempts;
    size_t size_mismatches;
    size_t index_violations;
} MemoryBoundaryContext_t;

static void memory_boundary_detector(size_t index, const void* element, size_t element_size, void* user_data) {
    MemoryBoundaryContext_t* ctx = (MemoryBoundaryContext_t*)user_data;
    ctx->elements_checked++;
    
    // Check for NULL element pointer
    if (!element) {
        ctx->null_access_attempts++;
        printf("🚨 DEBUG: NULL element pointer at index %zu\n", index);
        return;
    }
    
    // Check for suspicious element sizes
    if (element_size == 0 || element_size > 1024) {
        ctx->size_mismatches++;
        printf("🚨 DEBUG: Suspicious element size %zu at index %zu\n", element_size, index);
    }
    
    // Check for unreasonable index values
    if (index > 10000) {
        ctx->index_violations++;
        printf("🚨 DEBUG: Unreasonable index %zu detected\n", index);
    }
    
    // Test basic memory accessibility
    int test_value = *(int*)element;
    
    // Look for patterns that might indicate corruption
    if (test_value == 0xDEADBEEF || test_value == (int)0xDEADDEAD) {
        ctx->boundary_violations++;
        printf("🚨 DEBUG: Corruption pattern 0x%X found at index %zu\n", test_value, index);
    }
    
    printf("🔍 Boundary Check %zu: Index=%zu, Value=%d, Size=%zu\n", 
           ctx->elements_checked, index, test_value, element_size);
}

int test_debug_memory_boundary_detection(void)
{
    const char* test_file = "test_debug_boundary.dat";
    cleanup_test_file(test_file);
    
    // Create array that might reveal boundary issues
    dStaticArray_t* array = d_InitStaticArray(20, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create array for boundary test");
    
    // Fill with test pattern including edge case values
    int test_values[] = {0, -1, 2147483647, -2147483648, 42, 0xDEADBEEF, 100, 1000};
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(array, &test_values[i]) == 0, "Should append boundary test values");
    }
    
    printf("\n=== 🔍 MEMORY BOUNDARY DETECTION TEST ===\n");
    
    MemoryBoundaryContext_t boundary_ctx = {0, 0, 0, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(array, memory_boundary_detector, &boundary_ctx) == 0, 
               "Should complete boundary detection");
    
    TEST_ASSERT(boundary_ctx.elements_checked == 8, "Should check all 8 elements");
    TEST_ASSERT(boundary_ctx.null_access_attempts == 0, "Should find no NULL pointers");
    TEST_ASSERT(boundary_ctx.boundary_violations >= 1, "Should detect corruption patterns");
    
    // Save and test persistence of boundary data
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, array) == 0, "Should save boundary test array");
    
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load boundary test array");
    
    MemoryBoundaryContext_t loaded_boundary_ctx = {0, 0, 0, 0, 0};
    d_IterateStaticArray(loaded, memory_boundary_detector, &loaded_boundary_ctx);
    
    TEST_ASSERT(loaded_boundary_ctx.boundary_violations == boundary_ctx.boundary_violations, 
               "Should preserve boundary violations through save/load");
    
    printf("\n📊 BOUNDARY SUMMARY:\n");
    printf("Elements checked: %zu\n", boundary_ctx.elements_checked);
    printf("NULL access attempts: %zu\n", boundary_ctx.null_access_attempts);
    printf("Size mismatches: %zu\n", boundary_ctx.size_mismatches);
    printf("Index violations: %zu\n", boundary_ctx.index_violations);
    printf("Boundary violations: %zu\n", boundary_ctx.boundary_violations);
    
    d_DestroyStaticArray(array);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

// Capacity overflow and underflow detector
typedef struct {
    size_t capacity_seen;
    size_t count_seen;
    size_t overflow_attempts;
    size_t underflow_attempts;
    size_t capacity_violations;
} CapacityTestContext_t;

static void capacity_stress_detector(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element; (void)element_size; // Unused parameters
    CapacityTestContext_t* ctx = (CapacityTestContext_t*)user_data;
    
    // Track the highest index we've seen
    if (index >= ctx->capacity_seen) {
        ctx->capacity_seen = index + 1;
    }
    
    ctx->count_seen++;
    
    // Check for suspicious index patterns
    if (index > 1000) {
        ctx->overflow_attempts++;
        printf("🚨 DEBUG: Potential overflow - index %zu is very high\n", index);
    }
    
    // Check for impossible index values
    if (index == SIZE_MAX || index > SIZE_MAX - 100) {
        ctx->capacity_violations++;
        printf("🚨 DEBUG: Impossible index value %zu detected\n", index);
    }
    
    printf("⚗️ Capacity Test: Index=%zu, Count=%zu, Capacity=%zu\n", 
           index, ctx->count_seen, ctx->capacity_seen);
}

int test_debug_capacity_overflow_detection(void)
{
    const char* test_file = "test_debug_capacity.dat";
    cleanup_test_file(test_file);
    
    printf("\n=== ⚗️ CAPACITY OVERFLOW DETECTION TEST ===\n");
    
    // Test with minimal capacity array
    dStaticArray_t* small_array = d_InitStaticArray(3, sizeof(int));
    TEST_ASSERT(small_array != NULL, "Should create small capacity array");
    
    // Fill to capacity
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(small_array, &values[i]) == 0, "Should fill to capacity");
    }
    
    // Try to overflow (should fail)
    int overflow_value = 999;
    TEST_ASSERT(d_AppendDataToStaticArray(small_array, &overflow_value) == 1, "Should fail to append beyond capacity");
    
    // Test capacity detection
    CapacityTestContext_t capacity_ctx = {0, 0, 0, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(small_array, capacity_stress_detector, &capacity_ctx) == 0, 
               "Should complete capacity detection");
    
    TEST_ASSERT(capacity_ctx.count_seen == 3, "Should see exactly 3 elements");
    TEST_ASSERT(capacity_ctx.capacity_seen <= 3, "Should not exceed stated capacity");
    TEST_ASSERT(capacity_ctx.overflow_attempts == 0, "Should find no overflow attempts in small array");
    
    // Save and test capacity preservation
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, small_array) == 0, "Should save capacity test array");
    
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load capacity test array");
    TEST_ASSERT(loaded->capacity == 3, "Should preserve original capacity");
    TEST_ASSERT(loaded->count == 3, "Should preserve original count");
    
    // Test edge case: empty array
    dStaticArray_t* empty_array = d_InitStaticArray(5, sizeof(int));
    CapacityTestContext_t empty_ctx = {0, 0, 0, 0, 0};
    TEST_ASSERT(d_IterateStaticArray(empty_array, capacity_stress_detector, &empty_ctx) == 0, 
               "Should handle empty array iteration");
    TEST_ASSERT(empty_ctx.count_seen == 0, "Should see no elements in empty array");
    
    printf("\n📊 CAPACITY SUMMARY:\n");
    printf("Max capacity seen: %zu\n", capacity_ctx.capacity_seen);
    printf("Elements counted: %zu\n", capacity_ctx.count_seen);
    printf("Overflow attempts: %zu\n", capacity_ctx.overflow_attempts);
    printf("Capacity violations: %zu\n", capacity_ctx.capacity_violations);
    
    d_DestroyStaticArray(small_array);
    d_DestroyStaticArray(loaded);
    d_DestroyStaticArray(empty_array);
    cleanup_test_file(test_file);
    return 1;
}

// Data integrity and corruption detector
typedef struct {
    int last_value;
    size_t sequence_breaks;
    size_t value_corruptions;
    size_t unexpected_jumps;
    bool first_element;
    uint32_t running_checksum;
} DataIntegrityContext_t;

static void data_integrity_detector(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size; // Unused parameter
    DataIntegrityContext_t* ctx = (DataIntegrityContext_t*)user_data;
    int current_value = *(int*)element;
    
    // Expected pattern: values should be index * 5
    int expected_value = (int)index * 5;
    
    if (current_value != expected_value) {
        ctx->value_corruptions++;
        printf("🚨 DEBUG: Value corruption at index %zu! Expected=%d, Got=%d\n", 
               index, expected_value, current_value);
    }
    
    if (!ctx->first_element) {
        // Check for unexpected value jumps
        int value_diff = abs(current_value - ctx->last_value);
        if (value_diff > 100) {
            ctx->unexpected_jumps++;
            printf("🚨 DEBUG: Large value jump detected! %d -> %d (diff=%d)\n", 
                   ctx->last_value, current_value, value_diff);
        }
        
        // Check for sequence breaks in expected pattern
        int expected_diff = 5; // Should increment by 5 each time
        int actual_diff = current_value - ctx->last_value;
        if (abs(actual_diff - expected_diff) > 1) {
            ctx->sequence_breaks++;
            printf("🚨 DEBUG: Sequence break! Expected diff=%d, Got diff=%d\n", 
                   expected_diff, actual_diff);
        }
    } else {
        ctx->first_element = false;
    }
    
    ctx->last_value = current_value;
    ctx->running_checksum ^= (uint32_t)(current_value + index);
    
    printf("✅ Integrity: Index=%zu, Value=%d, Checksum=0x%X\n", 
           index, current_value, ctx->running_checksum);
}

int test_debug_data_integrity_detection(void)
{
    const char* test_file = "test_debug_integrity.dat";
    cleanup_test_file(test_file);
    
    printf("\n=== ✅ DATA INTEGRITY DETECTION TEST ===\n");
    
    // Create array with expected pattern
    dStaticArray_t* array = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create array for integrity test");
    
    // Fill with expected pattern: index * 5
    for (int i = 0; i < 7; i++) {
        int value = i * 5;
        TEST_ASSERT(d_AppendDataToStaticArray(array, &value) == 0, "Should append integrity test values");
    }
    
    // Test clean data first
    DataIntegrityContext_t clean_ctx = {0, 0, 0, 0, true, 0};
    TEST_ASSERT(d_IterateStaticArray(array, data_integrity_detector, &clean_ctx) == 0, 
               "Should complete clean integrity check");
    TEST_ASSERT(clean_ctx.value_corruptions == 0, "Should find no corruptions in clean data");
    TEST_ASSERT(clean_ctx.sequence_breaks == 0, "Should find no sequence breaks in clean data");
    
    // Introduce corruption
    int* corrupt_element = (int*)d_IndexDataFromStaticArray(array, 3);
    *corrupt_element = 999; // Should be 15, now corrupted to 999
    
    // Add more corruption
    corrupt_element = (int*)d_IndexDataFromStaticArray(array, 5);
    *corrupt_element = -100; // Should be 25, now corrupted to -100
    
    printf("\n=== 🚨 TESTING WITH CORRUPTED DATA ===\n");
    
    DataIntegrityContext_t corrupt_ctx = {0, 0, 0, 0, true, 0};
    d_IterateStaticArray(array, data_integrity_detector, &corrupt_ctx);
    
    TEST_ASSERT(corrupt_ctx.value_corruptions >= 2, "Should detect value corruptions");
    TEST_ASSERT(corrupt_ctx.unexpected_jumps >= 1, "Should detect unexpected jumps");
    
    // Save corrupted data and verify persistence
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, array) == 0, "Should save corrupted array");
    
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load corrupted array");
    
    DataIntegrityContext_t loaded_ctx = {0, 0, 0, 0, true, 0};
    d_IterateStaticArray(loaded, data_integrity_detector, &loaded_ctx);
    
    TEST_ASSERT(loaded_ctx.value_corruptions == corrupt_ctx.value_corruptions, 
               "Should preserve corruptions through save/load");
    
    printf("\n📊 INTEGRITY SUMMARY:\n");
    printf("Value corruptions: %zu\n", corrupt_ctx.value_corruptions);
    printf("Sequence breaks: %zu\n", corrupt_ctx.sequence_breaks);
    printf("Unexpected jumps: %zu\n", corrupt_ctx.unexpected_jumps);
    printf("Final checksum: 0x%X\n", corrupt_ctx.running_checksum);
    
    d_DestroyStaticArray(array);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

// Performance and timing anomaly detector
typedef struct {
    size_t iterations;
    size_t slow_iterations;
    size_t memory_pressure_tests;
    double max_iteration_time;
    double total_time;
    clock_t start_time;
} PerformanceAnomalyContext_t;

static void performance_anomaly_detector(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size; // Unused parameter
    PerformanceAnomalyContext_t* ctx = (PerformanceAnomalyContext_t*)user_data;
    
    clock_t iter_start = clock();
    
    int value = *(int*)element;
    
    // Simulate some work that might reveal performance issues
    volatile int computation_result = 0;
    for (int i = 0; i < 500; i++) {
        computation_result += (value * i) ^ (int)(index * i);
    }
    
    // Simulate memory pressure
    char* temp_buffer = malloc(512);
    if (temp_buffer) {
        memset(temp_buffer, (char)(value % 256), 512);
        ctx->memory_pressure_tests++;
        free(temp_buffer);
    }
    
    clock_t iter_end = clock();
    double iteration_time = ((double)(iter_end - iter_start)) / CLOCKS_PER_SEC;
    
    if (iteration_time > ctx->max_iteration_time) {
        ctx->max_iteration_time = iteration_time;
    }
    
    if (iteration_time > 0.001) { // More than 1ms is slow for this simple operation
        ctx->slow_iterations++;
        printf("🚨 DEBUG: Slow iteration detected! Time=%.6fs for index=%zu, value=%d\n", 
               iteration_time, index, value);
    }
    
    ctx->total_time += iteration_time;
    ctx->iterations++;
    
    printf("⚡ Perf: Iter=%zu, Index=%zu, Value=%d, Time=%.6fs\n", 
           ctx->iterations, index, value, iteration_time);
}

int test_debug_performance_anomaly_detection(void)
{
    const char* test_file = "test_debug_performance.dat";
    cleanup_test_file(test_file);
    
    printf("\n=== ⚡ PERFORMANCE ANOMALY DETECTION TEST ===\n");
    
    // Create larger array for performance testing
    const int num_elements = 15;
    dStaticArray_t* array = d_InitStaticArray(20, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create array for performance test");
    
    // Fill with diverse values that might cause performance variations
    for (int i = 0; i < num_elements; i++) {
        int value = (i * 137 + 1000) % 10000; // Non-sequential pattern
        TEST_ASSERT(d_AppendDataToStaticArray(array, &value) == 0, "Should append performance test values");
    }
    
    // Run performance test
    PerformanceAnomalyContext_t perf_ctx = {0, 0, 0, 0.0, 0.0, 0};
    perf_ctx.start_time = clock();
    
    TEST_ASSERT(d_IterateStaticArray(array, performance_anomaly_detector, &perf_ctx) == 0, 
               "Should complete performance anomaly detection");
    
    TEST_ASSERT(perf_ctx.iterations == num_elements, "Should process all elements");
    TEST_ASSERT(perf_ctx.memory_pressure_tests > 0, "Should perform memory pressure tests");
    
    // Save and test persistence doesn't affect performance characteristics
    TEST_ASSERT(d_SaveStaticArrayToFile(test_file, array) == 0, "Should save performance test array");
    
    dStaticArray_t* loaded = d_LoadStaticArrayFromFile(test_file);
    TEST_ASSERT(loaded != NULL, "Should load performance test array");
    
    PerformanceAnomalyContext_t loaded_perf_ctx = {0, 0, 0, 0.0, 0.0, 0};
    loaded_perf_ctx.start_time = clock();
    
    d_IterateStaticArray(loaded, performance_anomaly_detector, &loaded_perf_ctx);
    
    // Performance should be similar (within reasonable bounds)
    double original_avg = perf_ctx.total_time / perf_ctx.iterations;
    double loaded_avg = loaded_perf_ctx.total_time / loaded_perf_ctx.iterations;
    double performance_ratio = loaded_avg / (original_avg > 0 ? original_avg : 0.000001);
    
    TEST_ASSERT(performance_ratio < 5.0, "Loaded array performance should not be drastically worse");
    
    printf("\n📊 PERFORMANCE SUMMARY:\n");
    printf("Total iterations: %zu\n", perf_ctx.iterations);
    printf("Slow iterations: %zu\n", perf_ctx.slow_iterations);
    printf("Memory pressure tests: %zu\n", perf_ctx.memory_pressure_tests);
    printf("Max iteration time: %.6fs\n", perf_ctx.max_iteration_time);
    printf("Average time per element: %.6fs\n", original_avg);
    printf("Performance ratio (loaded/original): %.2f\n", performance_ratio);
    
    d_DestroyStaticArray(array);
    d_DestroyStaticArray(loaded);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

// Main test runner
int main(void)
{
    // Initialize Daedalus logging system
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .include_thread_id = false,
        .colorize_output = true,
        .timestamp_format = NULL,
        .context_separator = "::"
    };
    
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    
    d_LogInfo("Starting Static Array Save/Load Tests");

    TEST_SUITE_START("Static Array Save/Load Tests");

    // Run success tests
    RUN_TEST(test_save_load_basic_success);
    RUN_TEST(test_save_load_different_types_success);
    
    // Run bad path tests
    RUN_TEST(test_save_load_invalid_parameters);
    RUN_TEST(test_save_load_corrupted_file);
    
    // Run multiple cycle tests
    RUN_TEST(test_save_load_multiple_cycles_with_updates);
    RUN_TEST(test_save_load_large_dataset_consistency);
    
    // Run debug tests
    RUN_TEST(test_debug_stats_with_save_load);
    RUN_TEST(test_debug_value_collection_with_save_load);
    RUN_TEST(test_debug_search_callback_with_save_load);
    
    // Run debug-hunting tests
    RUN_TEST(test_debug_memory_boundary_detection);
    RUN_TEST(test_debug_capacity_overflow_detection);
    RUN_TEST(test_debug_data_integrity_detection);
    RUN_TEST(test_debug_performance_anomaly_detection);

    TEST_SUITE_END();

    d_LogInfo("Static Array Save/Load Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}