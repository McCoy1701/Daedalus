#include "../../include/tests.h"
#include "../../include/Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper Functions
void create_test_file(const char* filename, const char* content)
{
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "%s", content);
        fclose(file);
    }
}

void cleanup_test_file(const char* filename)
{
    remove(filename);
}

// Test Functions
int test_create_string_from_valid_file(void)
{
    const char* test_filename = "test_valid_file.txt";
    const char* test_content = "Hello, World!\nThis is a test file.\nLine 3.";

    // Create test file
    create_test_file(test_filename, test_content);

    // Test reading the file
    char* result = d_CreateStringFromFile(test_filename);

    TEST_ASSERT(result != NULL, "Should successfully read valid file");
    TEST_ASSERT(strcmp(result, test_content) == 0, "File content should match expected content");
    TEST_ASSERT(strlen(result) == strlen(test_content), "String length should match content length");

    // Clean up
    free(result);
    cleanup_test_file(test_filename);

    return 1;
}

int test_create_string_from_empty_file(void)
{
    const char* test_filename = "test_empty_file.txt";
    const char* test_content = "";

    // Create empty test file
    create_test_file(test_filename, test_content);

    // Test reading empty file
    char* result = d_CreateStringFromFile(test_filename);

    TEST_ASSERT(result == NULL, "Should not read empty file");

    // Clean up
    free(result);
    cleanup_test_file(test_filename);

    return 1;
}

int test_create_string_from_nonexistent_file(void)
{
    const char* nonexistent_filename = "this_file_does_not_exist.txt";

    // Ensure file doesn't exist
    cleanup_test_file(nonexistent_filename);

    // Test reading non-existent file
    char* result = d_CreateStringFromFile(nonexistent_filename);

    TEST_ASSERT(result == NULL, "Should return NULL for non-existent file");

    return 1;
}

int test_create_string_with_null_filename(void)
{
    // Test with NULL filename
    char* result = d_CreateStringFromFile(NULL);

    TEST_ASSERT(result == NULL, "Should return NULL for NULL filename");

    return 1;
}

// Main test runner
int main(void)
{
    TEST_SUITE_START("d_CreateStringFromFile Tests");

    RUN_TEST(test_create_string_from_valid_file);
    RUN_TEST(test_create_string_from_empty_file);
    RUN_TEST(test_create_string_from_nonexistent_file);
    RUN_TEST(test_create_string_with_null_filename);

    TEST_SUITE_END();
}
