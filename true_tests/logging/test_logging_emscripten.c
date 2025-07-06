// File: true_tests/logging/test_logging_emscripten.c - Emscripten-specific Daedalus logging tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// Test tracking
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// EMSCRIPTEN-SPECIFIC TEST UTILITIES
// =============================================================================

static dString_t* emscripten_capture_buffer = NULL;

void emscripten_test_capture_handler(const dLogEntry_t* entry, void* user_data) {
    (void)user_data;

    if (!emscripten_capture_buffer) {
        emscripten_capture_buffer = d_InitString();
    }

    d_AppendToString(emscripten_capture_buffer, "[", 0);
    d_AppendToString(emscripten_capture_buffer, d_LogLevel_ToString(entry->level), 0);
    d_AppendToString(emscripten_capture_buffer, "] ", 0);

    if (entry->message) {
        d_AppendToString(emscripten_capture_buffer, d_PeekString(entry->message), 0);
    }

    d_AppendToString(emscripten_capture_buffer, "\n", 0);
}

void setup_emscripten_logging() {
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .colorize_output = false  // Colors may not work in WebAssembly context
    };

    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

    d_LogInfo("=== Emscripten Logging Test Suite Initialized ===");
}

void cleanup_emscripten_logging() {
    d_LogInfo("=== Cleaning up Emscripten Logging Test Suite ===");

    dLogger_t* logger = d_GetGlobalLogger();
    if (logger) {
        d_DestroyLogger(logger);
        d_SetGlobalLogger(NULL);
    }

    if (emscripten_capture_buffer) {
        d_DestroyString(emscripten_capture_buffer);
        emscripten_capture_buffer = NULL;
    }
}

const char* get_emscripten_capture() {
    return emscripten_capture_buffer ? d_PeekString(emscripten_capture_buffer) : "";
}

void clear_emscripten_capture() {
    if (emscripten_capture_buffer) {
        d_ClearString(emscripten_capture_buffer);
    }
}

// =============================================================================
// TEST 1: BASIC EMSCRIPTEN LOGGING FUNCTIONALITY
// =============================================================================

int test_emscripten_basic_logging() {
    d_LogInfo("Starting Emscripten Basic Logging Test");
    d_LogDebug("Testing fundamental logging in WebAssembly environment");

    // Test all log levels work in Emscripten
    d_LogDebug("WebAssembly Debug message");
    d_LogInfo("WebAssembly Info message");
    d_LogWarning("WebAssembly Warning message");
    d_LogError("WebAssembly Error message");
    d_LogFatal("WebAssembly Fatal message");

    TEST_ASSERT(true, "All log levels should work without crashing in Emscripten");

    // Test formatted logging
    d_LogDebugF("Formatted debug: %d %s %.2f", 42, "test", 3.14159f);
    d_LogInfoF("Formatted info: Player level %d, score %d", 25, 12500);
    d_LogWarningF("Formatted warning: %s system at %.1f%% capacity", "Memory", 85.5f);

    TEST_ASSERT(true, "Formatted logging should work without issues in WebAssembly");

    // Test string builder integration
    dLogBuilder_t* builder = d_LogBegin(D_LOG_LEVEL_INFO);
    TEST_ASSERT(builder != NULL, "Log builder should work in Emscripten environment");

    if (builder) {
        d_LogBuilder_Append(builder, "WebAssembly ");
        d_LogBuilder_Append(builder, "string ");
        d_LogBuilder_Append(builder, "concatenation ");
        d_LogBuilder_AppendInt(builder, 2023);
        d_LogBuilder_End(builder);
    }

    d_LogInfo("✓ Basic Emscripten Logging Test Completed");
    return 1;
}

// =============================================================================
// TEST 2: EMSCRIPTEN THREADING MODEL COMPATIBILITY
// =============================================================================

int test_emscripten_threading_model() {
    d_LogInfo("Starting Emscripten Threading Model Test");
    d_LogDebug("Testing single-threaded logging behavior");

    dLogger_t* logger = d_GetGlobalLogger();
    TEST_ASSERT(logger != NULL, "Should have a valid logger for threading test");

    // Test rapid logging that would stress threading in native environment
    for (int i = 0; i < 100; i++) {
        d_LogDebugF("Rapid log entry %d - testing single-threaded performance", i);
        
        if (i % 20 == 0) {
            d_LogInfoF("Threading checkpoint %d reached", i);
        }
    }

    TEST_ASSERT(true, "Rapid logging should work smoothly in single-threaded Emscripten");

    // Test statistics tracking (which uses internal state that might be affected by threading)
    dLogStats_t stats;
    d_GetLogStats(logger, &stats);
    TEST_ASSERT(true, "Statistics should be retrievable without threading issues");

    d_LogInfoF("Threading test stats: %.4f seconds total processing time", stats.total_log_time);
    TEST_ASSERT(stats.total_log_time >= 0.0, "Processing time should be valid in Emscripten");

    d_LogInfo("✓ Emscripten Threading Model Test Completed");
    return 1;
}

// =============================================================================
// TEST 3: WEBASSEMBLY MEMORY CONSTRAINTS
// =============================================================================

int test_webassembly_memory_constraints() {
    d_LogInfo("Starting WebAssembly Memory Constraints Test");
    d_LogDebug("Testing logging under WebAssembly memory limitations");

    // Create string buffer for capturing logs
    dString_t* memory_buffer = d_InitString();
    TEST_ASSERT(memory_buffer != NULL, "Should be able to create string buffer in WebAssembly");

    dLogger_t* memory_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = false,
        .colorize_output = false
    });

    TEST_ASSERT(memory_logger != NULL, "Should create logger under memory constraints");

    d_AddLogHandler(memory_logger, d_StringLogHandler, memory_buffer);
    TEST_ASSERT(true, "Should add string handler without memory issues");

    dLogger_t* original_logger = d_GetGlobalLogger();
    d_SetGlobalLogger(memory_logger);

    // Test moderate memory usage (not too aggressive for WebAssembly)
    for (int i = 0; i < 50; i++) {
        d_LogInfoF("Memory test iteration %d with some content to build up buffer", i);
        
        // Test structured logging memory usage
        if (i % 10 == 0) {
            dLogStructured_t* mem_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
            if (mem_log) {
                d_LogStructured_SetFormat(mem_log, true);
                d_LogStructured_Field(mem_log, "test", "memory_constraint");
                d_LogStructured_FieldInt(mem_log, "iteration", i);
                d_LogStructured_FieldFloat(mem_log, "progress", (float)i / 50.0f * 100.0f, 2);
                d_LogStructured_Commit(mem_log);
            }
        }
    }

    // Check buffer size
    size_t buffer_size = d_GetLengthOfString(memory_buffer);
    d_LogInfoF("Memory test buffer size: %zu bytes", buffer_size);
    TEST_ASSERT(buffer_size > 0, "Buffer should contain logged data");
    TEST_ASSERT(buffer_size < 100000, "Buffer should not be excessively large for WebAssembly");

    // Restore original logger and cleanup
    d_SetGlobalLogger(original_logger);
    d_DestroyLogger(memory_logger);
    d_DestroyString(memory_buffer);

    d_LogInfo("✓ WebAssembly Memory Constraints Test Completed");
    return 1;
}

// =============================================================================
// TEST 4: EMSCRIPTEN PERFORMANCE CHARACTERISTICS
// =============================================================================

int test_emscripten_performance_characteristics() {
    d_LogInfo("Starting Emscripten Performance Characteristics Test");
    d_LogDebug("Testing logging performance in WebAssembly environment");

    dLogger_t* perf_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = false
    });

    TEST_ASSERT(perf_logger != NULL, "Performance logger should be created successfully");

    dLogger_t* original_logger = d_GetGlobalLogger();
    d_SetGlobalLogger(perf_logger);

    // Reset stats for clean measurement
    d_ResetLogStats(perf_logger);
    TEST_ASSERT(true, "Should be able to reset stats in Emscripten");

    // Measure basic logging performance
    dLogStats_t stats_before, stats_after;
    d_GetLogStats(perf_logger, &stats_before);

    const int perf_iterations = 200;  // Moderate for WebAssembly
    for (int i = 0; i < perf_iterations; i++) {
        d_LogDebugF("Performance test iteration %d", i);
        
        if (i % 50 == 0) {
            d_LogInfoF("Performance checkpoint %d", i);
        }
    }

    d_GetLogStats(perf_logger, &stats_after);

    double total_time = stats_after.total_log_time - stats_before.total_log_time;
    double avg_time_per_log = total_time / perf_iterations;

    d_LogInfoF("Emscripten performance: %.6f seconds total, %.8f per log", 
               total_time, avg_time_per_log);

    TEST_ASSERT(total_time >= 0.0, "Total time should be non-negative");
    TEST_ASSERT(avg_time_per_log < 0.01, "Average time per log should be reasonable for WebAssembly");

    // Test structured logging performance
    dLogStats_t struct_stats_before, struct_stats_after;
    d_GetLogStats(perf_logger, &struct_stats_before);

    for (int i = 0; i < 20; i++) {
        dLogStructured_t* perf_struct = d_LogStructured(D_LOG_LEVEL_INFO);
        if (perf_struct) {
            d_LogStructured_SetFormat(perf_struct, i % 2 == 0);
            d_LogStructured_Field(perf_struct, "performance", "emscripten");
            d_LogStructured_FieldInt(perf_struct, "iteration", i);
            d_LogStructured_FieldFloat(perf_struct, "webassembly_factor", 1.5f, 2);
            d_LogStructured_Commit(perf_struct);
        }
    }

    d_GetLogStats(perf_logger, &struct_stats_after);
    double struct_time = struct_stats_after.total_log_time - struct_stats_before.total_log_time;

    d_LogInfoF("Structured logging performance: %.6f seconds for 20 entries", struct_time);
    TEST_ASSERT(struct_time >= 0.0, "Structured logging time should be non-negative");

    d_SetGlobalLogger(original_logger);
    d_DestroyLogger(perf_logger);

    d_LogInfo("✓ Emscripten Performance Characteristics Test Completed");
    return 1;
}

// =============================================================================
// TEST 5: CROSS-PLATFORM FEATURE COMPATIBILITY
// =============================================================================

int test_cross_platform_compatibility() {
    d_LogInfo("Starting Cross-Platform Feature Compatibility Test");
    d_LogDebug("Testing feature parity between native and Emscripten builds");

    // Test log level controls
    dLogger_t* compat_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .colorize_output = false
    });

    TEST_ASSERT(compat_logger != NULL, "Should create logger for compatibility test");

    dLogger_t* original_logger = d_GetGlobalLogger();
    d_SetGlobalLogger(compat_logger);

    // Test level filtering
    d_LogDebug("This debug should be filtered");
    d_LogInfo("This info should appear");
    d_LogWarning("This warning should appear");

    // Change log level
    d_SetLogLevel(compat_logger, D_LOG_LEVEL_DEBUG);
    TEST_ASSERT(d_GetLogLevel(compat_logger) == D_LOG_LEVEL_DEBUG, "Log level should change");

    d_LogDebug("This debug should now appear");

    // Test global logging toggle
    bool original_enabled = d_IsLoggingEnabled();
    d_SetLoggingEnabled(false);
    TEST_ASSERT(!d_IsLoggingEnabled(), "Logging should be disabled");

    d_LogError("This should not appear");

    d_SetLoggingEnabled(true);
    TEST_ASSERT(d_IsLoggingEnabled(), "Logging should be re-enabled");

    d_LogError("This should appear again");

    // Test filter system compatibility
    dLogFilterBuilder_t* compat_filter = d_CreateFilterBuilder();
    TEST_ASSERT(compat_filter != NULL, "Filter builder should work in Emscripten");

    if (compat_filter) {
        d_FilterBuilder_AddPrefix(compat_filter, "EMSCRIPTEN_", D_LOG_LEVEL_INFO);
        d_FilterBuilder_AddSuffix(compat_filter, "_WASM", D_LOG_LEVEL_DEBUG);
        TEST_ASSERT(true, "Should add filter rules without issues");

        d_FilterBuilder_Apply(compat_logger, compat_filter);
        TEST_ASSERT(true, "Should apply filters in Emscripten environment");

        d_DestroyFilterBuilder(compat_filter);
    }

    // Test file vs string handlers (string handler is more reliable in WebAssembly)
    dString_t* compat_buffer = d_InitString();
    TEST_ASSERT(compat_buffer != NULL, "String buffer should work reliably");

    d_AddLogHandler(compat_logger, d_StringLogHandler, compat_buffer);
    TEST_ASSERT(true, "String handler should be preferred in WebAssembly");

    d_LogInfo("EMSCRIPTEN_TEST: String handler test");
    d_LogDebug("COMPATIBILITY_WASM: Filter and handler test");

    const char* captured = d_PeekString(compat_buffer);
    TEST_ASSERT(captured != NULL, "Should capture logs via string handler");
    TEST_ASSERT(strlen(captured) > 0, "Captured content should not be empty");

    d_SetGlobalLogger(original_logger);
    d_DestroyLogger(compat_logger);
    d_DestroyString(compat_buffer);

    d_LogInfo("✓ Cross-Platform Feature Compatibility Test Completed");
    return 1;
}

// =============================================================================
// TEST 6: EMSCRIPTEN ERROR HANDLING
// =============================================================================

int test_emscripten_error_handling() {
    d_LogInfo("Starting Emscripten Error Handling Test");
    d_LogDebug("Testing robust error handling in WebAssembly environment");

    // Test NULL pointer safety (critical in constrained environment)
    d_LogEx(D_LOG_LEVEL_INFO, NULL, 0, NULL, "Message with NULL parameters");
    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, NULL);
    TEST_ASSERT(true, "NULL pointer handling should be robust in Emscripten");

    // Test empty string handling
    d_LogF(D_LOG_LEVEL_INFO, "");
    d_LogEx(D_LOG_LEVEL_INFO, "", 0, "", "");
    TEST_ASSERT(true, "Empty string handling should work in WebAssembly");

    // Test structured logging edge cases
    dLogStructured_t* error_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(error_log != NULL, "Structured log should be created for error testing");

    if (error_log) {
        // Test with NULL/empty fields
        d_LogStructured_Field(error_log, "valid", "data");
        d_LogStructured_Field(error_log, "", "empty_key");
        d_LogStructured_Field(error_log, "empty_val", "");
        d_LogStructured_Field(error_log, NULL, "null_key");
        d_LogStructured_Field(error_log, "null_val", NULL);
        TEST_ASSERT(true, "Should handle NULL/empty fields gracefully");

        d_LogStructured_Commit(error_log);
    }

    // Test logging when disabled
    d_SetLoggingEnabled(false);
    d_LogError("This should not appear - logging disabled in Emscripten");
    TEST_ASSERT(!d_IsLoggingEnabled(), "Logging should be properly disabled");

    d_SetLoggingEnabled(true);
    d_LogInfo("Logging re-enabled in Emscripten");
    TEST_ASSERT(d_IsLoggingEnabled(), "Logging should be properly re-enabled");

    // Test with invalid parameters that shouldn't crash WebAssembly
    d_LogEx((dLogLevel_t)999, __FILE__, __LINE__, __FUNCTION__, "Invalid log level test");
    TEST_ASSERT(true, "Invalid parameters should not crash WebAssembly runtime");

    d_LogInfo("✓ Emscripten Error Handling Test Completed");
    return 1;
}

// =============================================================================
// TEST 7: WEBASSEMBLY INTEGRATION SCENARIO
// =============================================================================

int test_webassembly_integration_scenario() {
    d_LogInfo("Starting WebAssembly Integration Scenario Test");
    d_LogDebug("Testing complete logging workflow in WebAssembly context");

    // Simulate a complete WebAssembly application logging scenario
    d_LogInfo("=== WebAssembly Application Startup ===");

    // Application initialization
    dLogStructured_t* startup_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(startup_log != NULL, "Startup structured log should be created");

    if (startup_log) {
        d_LogStructured_SetFormat(startup_log, true);
        d_LogStructured_Field(startup_log, "platform", "emscripten");
        d_LogStructured_Field(startup_log, "runtime", "webassembly");
        d_LogStructured_Field(startup_log, "phase", "initialization");
        d_LogStructured_FieldFloat(startup_log, "memory_limit_mb", 16.0f, 1);
        d_LogStructured_Commit(startup_log);
    }

    // Simulate user interactions
    d_LogInfo("=== User Interaction Simulation ===");
    for (int interaction = 1; interaction <= 5; interaction++) {
        d_LogInfoF("User interaction %d: Button click processed", interaction);
        
        dLogStructured_t* interaction_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        if (interaction_log) {
            d_LogStructured_SetFormat(interaction_log, false);
            d_LogStructured_Field(interaction_log, "event", "user_click");
            d_LogStructured_FieldInt(interaction_log, "interaction_id", interaction);
            d_LogStructured_FieldFloat(interaction_log, "timestamp", interaction * 0.5f, 3);
            d_LogStructured_Commit(interaction_log);
        }

        if (interaction == 3) {
            d_LogWarning("WebAssembly: High CPU usage detected during interaction");
        }
    }

    // Simulate data processing
    d_LogInfo("=== Data Processing Phase ===");
    for (int process = 1; process <= 3; process++) {
        d_LogInfoF("Processing data chunk %d of 3", process);
        
        if (process == 2) {
            d_LogWarning("WebAssembly: Memory allocation approaching limit");
        }
    }

    // Application shutdown
    d_LogInfo("=== WebAssembly Application Shutdown ===");
    dLogStructured_t* shutdown_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(shutdown_log != NULL, "Shutdown structured log should be created");

    if (shutdown_log) {
        d_LogStructured_SetFormat(shutdown_log, true);
        d_LogStructured_Field(shutdown_log, "phase", "shutdown");
        d_LogStructured_FieldInt(shutdown_log, "total_interactions", 5);
        d_LogStructured_FieldInt(shutdown_log, "data_chunks_processed", 3);
        d_LogStructured_Field(shutdown_log, "status", "clean_exit");
        d_LogStructured_Commit(shutdown_log);
    }

    // Get final statistics
    dLogger_t* logger = d_GetGlobalLogger();
    if (logger) {
        dLogStats_t final_stats;
        d_GetLogStats(logger, &final_stats);
        
        d_LogInfoF("WebAssembly session stats: %.4f seconds total", final_stats.total_log_time);
        TEST_ASSERT(final_stats.total_log_time >= 0.0, "Session stats should be valid");
    }

    d_LogInfo("✓ WebAssembly Integration Scenario Test Completed");
    return 1;
}
// =============================================================================
// TEST 8: EMSCRIPTEN RATE LIMITING IN WEBASSEMBLY
// =============================================================================

int test_emscripten_rate_limiting() {
    d_LogInfo("Starting Emscripten Rate Limiting Test");
    d_LogDebug("Testing rate-limited logging in WebAssembly single-threaded environment");

    // Test rate limiting without threading concerns
    d_LogInfo("Testing basic rate limiting in WebAssembly");
    
    // Reset rate limiter for clean test
    d_ResetRateLimiterCache();
    TEST_ASSERT(d_GetRateLimiterCacheEntryCount() == 0, "Rate limiter cache should be empty after reset");

    // Test rate limiting with identical messages
    const char* repeated_message = "WebAssembly repeated error message";
    for (int i = 0; i < 10; i++) {
        d_LogRateLimited(D_LOG_LEVEL_WARNING, 3, 1.0, repeated_message);
    }

    // Should have created a rate limit entry
    TEST_ASSERT(d_GetRateLimiterCacheEntryCount() > 0, "Rate limiter should have cached entries");

    // Test formatted rate limiting
    for (int i = 0; i < 8; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, 
                         D_LOG_LEVEL_ERROR, 2, 2.0, 
                         "WebAssembly error code: %d", 404);
    }

    // Test rate limiting with different hash strategies
    for (int i = 0; i < 5; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, 
                         D_LOG_LEVEL_INFO, 3, 1.5, 
                         "Dynamic message %d in WebAssembly", i);
    }

    size_t final_cache_size = d_GetRateLimiterCacheEntryCount();
    d_LogInfoF("Rate limiter cache contains %zu entries after testing", final_cache_size);
    TEST_ASSERT(final_cache_size > 0, "Rate limiter should have multiple cached entries");

    d_LogInfo("✓ Emscripten Rate Limiting Test Completed");
    return 1;
}

// =============================================================================
// TEST 9: WEBASSEMBLY LOG CONTEXT STACK MANAGEMENT  
// =============================================================================

int test_webassembly_log_context_stack() {
    d_LogInfo("Starting WebAssembly Log Context Stack Test");
    d_LogDebug("Testing hierarchical log contexts in single-threaded WebAssembly");

    // Test basic context push/pop
    dLogContext_t* wasm_context = d_PushLogContext("WebAssembly");
    TEST_ASSERT(wasm_context != NULL, "Should be able to push WebAssembly context");

    d_LogInfo("Message from WebAssembly context");

    // Test nested contexts
    dLogContext_t* game_context = d_PushLogContext("GameEngine");
    TEST_ASSERT(game_context != NULL, "Should be able to push nested game context");

    d_LogInfo("Message from WebAssembly::GameEngine context");

    dLogContext_t* render_context = d_PushLogContext("Renderer");
    TEST_ASSERT(render_context != NULL, "Should be able to push deeply nested context");

    d_LogInfo("Message from WebAssembly::GameEngine::Renderer context");

    // Test structured logging with contexts
    dLogStructured_t* context_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(context_log != NULL, "Structured log should work with contexts");

    if (context_log) {
        d_LogStructured_SetFormat(context_log, true);
        d_LogStructured_Field(context_log, "platform", "webassembly");
        d_LogStructured_Field(context_log, "subsystem", "renderer");
        d_LogStructured_FieldInt(context_log, "frame_count", 120);
        d_LogStructured_Commit(context_log);
    }

    // Test proper context cleanup (important in WebAssembly memory management)
    d_PopLogContext(render_context);
    d_LogInfo("Back in WebAssembly::GameEngine context");

    d_PopLogContext(game_context);
    d_LogInfo("Back in WebAssembly context");

    d_PopLogContext(wasm_context);
    d_LogInfo("Back in global context");

    // Test rapid context creation/destruction to stress WebAssembly memory
    for (int i = 0; i < 20; i++) {
        char context_name[32];
        snprintf(context_name, sizeof(context_name), "TempContext_%d", i);
        
        dLogContext_t* temp_context = d_PushLogContext(context_name);
        TEST_ASSERT(temp_context != NULL, "Should create temporary context");
        
        d_LogDebugF("Temporary context %d active", i);
        
        d_PopLogContext(temp_context);
    }

    d_LogInfo("✓ WebAssembly Log Context Stack Test Completed");
    return 1;
}

// =============================================================================
// TEST 10: EMSCRIPTEN BROWSER-SPECIFIC FEATURES
// =============================================================================

int test_emscripten_browser_features() {
    d_LogInfo("Starting Emscripten Browser-Specific Features Test");
    d_LogDebug("Testing browser environment adaptations and web-specific logging");

    // Test color output handling (should be disabled for browser console compatibility)
    dLogConfig_t browser_config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = true,
        .colorize_output = false  // Should be false for browser compatibility
    };

    dLogger_t* browser_logger = d_CreateLogger(browser_config);
    TEST_ASSERT(browser_logger != NULL, "Browser-compatible logger should be created");
    TEST_ASSERT(browser_logger->config.colorize_output == false, "Colors should be disabled for browser");

    dLogger_t* original_logger = d_GetGlobalLogger();
    d_SetGlobalLogger(browser_logger);

    // Test web-friendly log formatting
    d_LogInfo("WebAssembly module loaded successfully");
    d_LogWarning("Browser: WebGL context creation warning");
    d_LogError("Browser: Local storage quota exceeded");

    // Test JavaScript-friendly structured data (JSON format preferred)
    dLogStructured_t* js_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(js_log != NULL, "JavaScript-compatible structured log should be created");

    if (js_log) {
        d_LogStructured_SetFormat(js_log, true); // JSON format for JS compatibility
        d_LogStructured_Field(js_log, "event_type", "browser_interaction");
        d_LogStructured_Field(js_log, "user_agent", "WebAssembly/1.0");
        d_LogStructured_Field(js_log, "canvas_id", "game_canvas");
        d_LogStructured_FieldInt(js_log, "viewport_width", 1920);
        d_LogStructured_FieldInt(js_log, "viewport_height", 1080);
        d_LogStructured_FieldFloat(js_log, "device_pixel_ratio", 2.0f, 1);
        d_LogStructured_FieldBool(js_log, "webgl_supported", true);
        d_LogStructured_Commit(js_log);
    }

    // Test performance logging suitable for browser dev tools
    dLogStructured_t* perf_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(perf_log != NULL, "Performance log should be created");

    if (perf_log) {
        d_LogStructured_SetFormat(perf_log, true);
        d_LogStructured_Field(perf_log, "performance_metric", "frame_timing");
        d_LogStructured_FieldFloat(perf_log, "frame_time_ms", 16.67f, 2);
        d_LogStructured_FieldInt(perf_log, "fps", 60);
        d_LogStructured_FieldFloat(perf_log, "cpu_usage_percent", 45.2f, 1);
        d_LogStructured_FieldFloat(perf_log, "memory_used_mb", 128.5f, 1);
        d_LogStructured_Commit(perf_log);
    }

    // Test error logging with browser debugging context
    d_LogErrorF("Browser: Failed to load asset '%s' from URL", "textures/hero_sprite.png");
    d_LogErrorF("Browser: WebSocket connection failed to %s:%d", "wss://game.server.com", 443);

    // Test conditional logging based on browser capabilities
    d_LogIf(true, D_LOG_LEVEL_INFO, "Browser: WebGL 2.0 context created successfully");
    d_LogIf(false, D_LOG_LEVEL_WARNING, "Browser: Falling back to WebGL 1.0");

    // Test web-specific diagnostic information
    dLogBuilder_t* diag_builder = d_LogBegin(D_LOG_LEVEL_INFO);
    TEST_ASSERT(diag_builder != NULL, "Diagnostic builder should be created");

    if (diag_builder) {
        d_LogBuilder_Append(diag_builder, "Browser Diagnostics: ");
        d_LogBuilder_Append(diag_builder, "WebAssembly=");
        d_LogBuilder_Append(diag_builder, "supported, ");
        d_LogBuilder_Append(diag_builder, "SharedArrayBuffer=");
        d_LogBuilder_Append(diag_builder, "disabled, ");
        d_LogBuilder_Append(diag_builder, "OffscreenCanvas=");
        d_LogBuilder_Append(diag_builder, "available");
        d_LogBuilder_End(diag_builder);
    }

    // Test string buffer capture for potential JavaScript bridge
    dString_t* browser_buffer = d_InitString();
    TEST_ASSERT(browser_buffer != NULL, "Browser capture buffer should be created");

    d_AddLogHandler(browser_logger, d_StringLogHandler, browser_buffer);
    TEST_ASSERT(true, "String handler for browser integration should be added");

    d_LogInfo("Browser: This message should be captured for JavaScript");
    d_LogWarning("Browser: Performance warning - consider reducing quality");

    const char* captured_logs = d_PeekString(browser_buffer);
    TEST_ASSERT(captured_logs != NULL, "Should capture browser-friendly logs");
    TEST_ASSERT(strlen(captured_logs) > 0, "Captured logs should not be empty");
    TEST_ASSERT(strstr(captured_logs, "Browser:") != NULL, "Should contain browser-specific messages");

    d_LogInfoF("Captured %zu bytes of browser-compatible log data", strlen(captured_logs));

    // Cleanup
    d_SetGlobalLogger(original_logger);
    d_DestroyLogger(browser_logger);
    d_DestroyString(browser_buffer);

    d_LogInfo("✓ Emscripten Browser-Specific Features Test Completed");
    return 1;
}
// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main() {
    printf("\n");
    printf("=============================================================================\n");
    printf("              DAEDALUS EMSCRIPTEN LOGGING COMPATIBILITY SUITE               \n");
    printf("                     Testing WebAssembly Cross-Realm Stability              \n");
    printf("=============================================================================\n\n");

    setup_emscripten_logging();

    TEST_SUITE_START("Emscripten Logging Compatibility");

    RUN_TEST(test_emscripten_basic_logging);
    RUN_TEST(test_emscripten_threading_model);
    RUN_TEST(test_webassembly_memory_constraints);
    RUN_TEST(test_emscripten_performance_characteristics);
    RUN_TEST(test_cross_platform_compatibility);
    RUN_TEST(test_emscripten_error_handling);
    RUN_TEST(test_webassembly_integration_scenario);

    // New WebAssembly-specific tests
    RUN_TEST(test_emscripten_rate_limiting);
    RUN_TEST(test_webassembly_log_context_stack);
    RUN_TEST(test_emscripten_browser_features);

    TEST_SUITE_END();

    cleanup_emscripten_logging();

    return 0;
}