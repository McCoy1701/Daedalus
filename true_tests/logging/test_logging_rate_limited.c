// File: true_tests/logging/test_logging_rate_limited.c
// The final, victorious tests for the Daedalus rate-limiter.

#include "tests.h"
#include "Daedalus.h"
#include <unistd.h> // For sleep()
#include <stdio.h>  // For sprintf in assert messages

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Test-specific Log Handler - Counts calls.
static size_t log_handler_call_count = 0;
void counting_log_handler(const dLogEntry_t* entry, void* user_data) {
    (void)entry;
    (void)user_data;
    log_handler_call_count++;
}


// TEST SETUP & TEARDOWN
void setup_logging_tests(void) {
    log_handler_call_count = 0;

    // THIS IS THE CRITICAL FIX that was not working before.
    // Now that the prototype is in the header, it will link and run correctly.
    d_ResetRateLimiterCache();

    dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG };
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

    d_RemoveLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler);
    d_AddLogHandler(d_GetGlobalLogger(), counting_log_handler, NULL);
}

void teardown_logging_tests(void) {
    d_DestroyLogger(d_GetGlobalLogger());
    d_SetGlobalLogger(NULL); // Important to avoid using a destroyed logger
}

// =============================================================================
// THE VICTORIOUS TESTS
// =============================================================================

int test_rate_limiter_blocks_rapid_formatted_logs(void) {
    setup_logging_tests();
    int max_logs = 5;
    char assert_msg[128];

    for (int i = 0; i < 100; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs, 2.0, "Log: %d", i);
    }

    sprintf(assert_msg, "Formatted logs should be limited. Expected %d, Got %zu", max_logs, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == max_logs, assert_msg);

    teardown_logging_tests();
    return 1;
}

int test_rate_limiter_resets_after_time_window(void) {
    setup_logging_tests();
    int max_logs = 3;
    char assert_msg[128];

    for (int i = 0; i < 10; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs, 1.5, "Time window log: %d", i);
    }
    sprintf(assert_msg, "First burst should be limited. Expected %d, Got %zu", max_logs, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == max_logs, assert_msg);

    sleep(2); // Wait for the 1.5s window to expire

    for (int i = 0; i < 10; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs, 1.5, "Time window log: %d", i);
    }

    sprintf(assert_msg, "Second burst should be limited. Expected %d, Got %zu", max_logs * 2, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == max_logs * 2, assert_msg);

    teardown_logging_tests();
    return 1;
}

int test_limiter_respects_hash_final_message_flag(void) {
    setup_logging_tests();
    int loop_count = 50;
    char assert_msg[128];

    for (int i = 0; i < loop_count; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, D_LOG_LEVEL_INFO, 5, 2.0, "HASH_FINAL_MESSAGE: %d", i);
    }

    sprintf(assert_msg, "Hashing final message should be unique. Expected %d, Got %zu", loop_count, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == loop_count, assert_msg);

    teardown_logging_tests();
    return 1;
}

int test_limiter_with_zero_max_logs(void) {
    setup_logging_tests();
    char assert_msg[128];

    for (int i = 0; i < 50; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, 0, 2.0, "This should never appear.", i);
    }

    sprintf(assert_msg, "No logs should be processed for max_count=0. Expected 0, Got %zu", log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == 0, assert_msg);

    teardown_logging_tests();
    return 1;
}

int test_limiter_with_zero_time_window(void) {
    setup_logging_tests();
    int loop_count = 50;
    char assert_msg[128];

    // With a time_window of 0.0, the limiter should reset on every call.
    for (int i = 0; i < loop_count; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, 1, 0.0, "Zero window test: %d", i);
    }

    // Every log should have been allowed through because the window expires instantly.
    sprintf(assert_msg, "A zero time window should not limit any logs. Expected %d, Got %zu", loop_count, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == loop_count, assert_msg);

    teardown_logging_tests();
    return 1;
}

#include <pthread.h> // Make sure to include this at the top of the file!

// This is the function our threads will execute
void* thread_hammer_function(void* arg) {
    int max_logs = *(int*)arg;
    for (int i = 0; i < 100; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs, 2.0, "Multi-thread log", i);
    }
    return NULL;
}

int test_limiter_is_thread_safe(void) {
    setup_logging_tests();
    pthread_t thread1, thread2;
    int max_logs = 5;
    char assert_msg[128];

    // Create two threads, both running the hammer function
    pthread_create(&thread1, NULL, thread_hammer_function, &max_logs);
    pthread_create(&thread2, NULL, thread_hammer_function, &max_logs);

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Even with two threads hammering, the total logs should be EXACTLY max_logs.
    // If it's higher (e.g., 6, 7, or 10), we have a race condition.
    sprintf(assert_msg, "Rate limiter should be thread-safe. Expected %d, Got %zu", max_logs, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == max_logs, assert_msg);

    teardown_logging_tests();
    return 1;
}

int test_limiter_with_many_unique_messages(void) {
    setup_logging_tests();
    int unique_message_count = 200;
    int max_logs_per_message = 1;
    char assert_msg[128];

    // In this loop, we will generate many unique format strings
    // and attempt to log each one more times than allowed.
    for (int i = 0; i < unique_message_count; i++) {
        char unique_format[64];
        sprintf(unique_format, "This is unique message number %d", i);

        // Try to log this unique message twice, even though only one is allowed.
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs_per_message, 2.0, unique_format);
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs_per_message, 2.0, unique_format);
    }

    // The handler should have been called exactly once for each unique message.
    sprintf(assert_msg, "Cache stress test failed. Expected %d, Got %zu", unique_message_count, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == unique_message_count, assert_msg);

    // We can also use our debug helper to ensure the cache grew to the correct size.
    TEST_ASSERT(d_GetRateLimiterCacheEntryCount() == unique_message_count, "The cache should contain one entry for each unique message.");

    teardown_logging_tests();
    return 1;
}
int test_limiter_with_many_unique_messages_step_by_step(void) {
    setup_logging_tests();
    int unique_message_count = 200;
    char assert_msg[128];

    // In this loop, we will check the cache size after every single new entry.
    for (int i = 0; i < unique_message_count; i++) {
        char unique_format[64];
        sprintf(unique_format, "This is unique message number %d", i);

        // Log a new unique message. This should create a new cache entry.
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, 1, 2.0, unique_format);

        // Assert that the cache size is EXACTLY i + 1.
        // This is our vivisection. We are checking the patient's vitals after every incision.
        size_t current_cache_size = d_GetRateLimiterCacheEntryCount();
        sprintf(assert_msg, "After item %d, cache size should be %d, but was %zu", i, i + 1, current_cache_size);
        TEST_ASSERT(current_cache_size == (size_t)(i + 1), assert_msg);

        // If the test fails, we stop immediately to preserve the evidence.
        if (current_cache_size != (size_t)(i + 1)) {
            teardown_logging_tests();
            return 0; // Exit early on failure
        }
    }

    // The final log count should still be correct.
    sprintf(assert_msg, "Final log count should be correct. Expected %d, Got %zu", unique_message_count, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == unique_message_count, assert_msg);

    teardown_logging_tests();
    return 1;
}
int test_limiter_handles_null_and_empty_strings(void) {
    setup_logging_tests();
    char assert_msg[128];

    // --- Part 1: NULL input ---
    // The function should not crash when given a NULL format string.
    for (int i = 0; i < 50; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, 5, 2.0, NULL);
    }

    // No logs should have been produced.
    sprintf(assert_msg, "NULL format string should produce no logs. Expected 0, Got %zu", log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == 0, assert_msg);


    // --- Part 2: Empty String input ---
    // An empty string is a valid string and should be rate-limited correctly.
    int max_logs = 3;
    for (int i = 0; i < 50; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO, max_logs, 2.0, "");
    }

    // Exactly `max_logs` should have been produced.
    sprintf(assert_msg, "Empty string should be rate-limited. Expected %d, Got %zu", max_logs, log_handler_call_count);
    TEST_ASSERT(log_handler_call_count == max_logs, assert_msg);

    teardown_logging_tests();
    return 1;
}
int main(void) {
    TEST_SUITE_START("Daedalus Logging Rate Limiter Tests - FINAL BATTLE");

    RUN_TEST(test_rate_limiter_blocks_rapid_formatted_logs);
    RUN_TEST(test_rate_limiter_resets_after_time_window);
    RUN_TEST(test_limiter_respects_hash_final_message_flag);
    RUN_TEST(test_limiter_with_zero_max_logs);

    RUN_TEST(test_limiter_with_zero_time_window);
    RUN_TEST(test_limiter_is_thread_safe);
    RUN_TEST(test_limiter_with_many_unique_messages);
    RUN_TEST(test_limiter_with_many_unique_messages_step_by_step);
    RUN_TEST(test_limiter_handles_null_and_empty_strings);

    TEST_SUITE_END();
}
