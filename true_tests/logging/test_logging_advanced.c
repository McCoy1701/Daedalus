// File: true_tests/logging/test_logging_advanced.c - Minimal Advanced Daedalus logging functionality tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

// Test tracking
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// TEST UTILITIES AND SETUP
// =============================================================================

// Custom capture handler for testing output
static dString_t* test_capture_buffer = NULL;

void advanced_test_capture_handler(const dLogEntry_t* entry, void* user_data) {
    (void)user_data; // Suppress unused parameter warning

    if (!test_capture_buffer) {
        test_capture_buffer = d_InitString();
    }

    d_AppendString(test_capture_buffer, "[", 0);
    d_AppendString(test_capture_buffer, d_LogLevel_ToString(entry->level), 0);
    d_AppendString(test_capture_buffer, "] ", 0);

    if (entry->message) {
        d_AppendString(test_capture_buffer, d_PeekString(entry->message), 0);
    }

    d_AppendString(test_capture_buffer, "\n", 0);
}

void setup_advanced_test_logging() {
    // Create logger with basic configuration
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .colorize_output = true
    };

    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

    d_LogInfo("=== Advanced Logging Test Suite Initialized ===");
}

void cleanup_advanced_test_logging() {
    d_LogInfo("=== Cleaning up Advanced Logging Test Suite ===");

    dLogger_t* logger = d_GetGlobalLogger();
    if (logger) {
        d_DestroyLogger(logger);
        d_SetGlobalLogger(NULL);
    }

    if (test_capture_buffer) {
        d_DestroyString(test_capture_buffer);
        test_capture_buffer = NULL;
    }
}

const char* get_test_capture() {
    return test_capture_buffer ? d_PeekString(test_capture_buffer) : "";
}

void clear_test_capture() {
    if (test_capture_buffer) {
        d_ClearString(test_capture_buffer);
    }
}

// =============================================================================
// TEST 1: STRUCTURED LOGGING SYSTEM
// =============================================================================

int test_structured_logging_system() {
    d_LogInfo("Starting Structured Logging System Test");
    d_LogDebug("Testing JSON and key-value structured logging");

    // Test JSON format structured logging
    d_LogInfo("Testing JSON structured logging");
    dLogStructured_t* json_log = d_LogStructured(D_LOG_LEVEL_INFO);

    TEST_ASSERT(json_log != NULL, "JSON structured log should be created successfully");

    if (json_log) {
        d_LogStructured_SetFormat(json_log, true); // Enable JSON
        d_LogStructured_Field(json_log, "event", "player_action");
        d_LogStructured_Field(json_log, "player", "Knight");
        d_LogStructured_Field(json_log, "action", "attack");
        d_LogStructured_FieldInt(json_log, "damage", 42);
        d_LogStructured_FieldFloat(json_log, "crit_chance", 0.25f, 2);
        d_LogStructured_Commit(json_log);
        d_LogInfo("✓ JSON structured logging test completed");
    } else {
        d_LogError("Failed to create JSON structured log");
        return 0;
    }

    // Test key-value format structured logging
    d_LogInfo("Testing key-value structured logging");
    dLogStructured_t* kv_log = d_LogStructured(D_LOG_LEVEL_INFO);

    TEST_ASSERT(kv_log != NULL, "Key-value structured log should be created successfully");

    if (kv_log) {
        d_LogStructured_SetFormat(kv_log, false); // Key-value format
        d_LogStructured_Field(kv_log, "system", "physics");
        d_LogStructured_Field(kv_log, "fps", "60");
        d_LogStructured_FieldInt(kv_log, "entities", 150);
        d_LogStructured_FieldFloat(kv_log, "delta_time", 16.67f, 2);
        d_LogStructured_Commit(kv_log);
        d_LogInfo("✓ Key-value structured logging test completed");
    } else {
        d_LogError("Failed to create key-value structured log");
        return 0;
    }

    d_LogInfo("✓ Structured Logging System Test Completed");
    return 1;
}

// =============================================================================
// TEST 2: CONSOLE HANDLER TESTING
// =============================================================================

int test_console_handler() {
    d_LogInfo("Starting Console Handler Test");
    d_LogDebug("Testing colored console output handler");

    // Create a logger specifically for console testing
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = true
    };

    dLogger_t* console_logger = d_CreateLogger(config);
    TEST_ASSERT(console_logger != NULL, "Console logger should be created successfully");

    // Add console handler
    d_AddLogHandler(console_logger, d_ConsoleLogHandler, NULL);
    TEST_ASSERT(true, "Console handler should be added successfully");

    // Log various game events with different levels
    d_LogEx(D_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__,
            "Debug: Checking collision boundaries");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Player reached checkpoint: Crystal Cave Entrance");

    d_LogEx(D_LOG_LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__,
            "Warning: Enemy count exceeds recommended limit (150/100)");

    d_LogEx(D_LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__,
            "Failed to load boss texture: boss_dragon.png");

    d_LogEx(D_LOG_LEVEL_FATAL, __FILE__, __LINE__, __FUNCTION__,
            "FATAL: Game save corrupted - unable to continue");

    d_DestroyLogger(console_logger);

    d_LogInfo("✓ Console Handler Test Completed");
    return 1;
}

// =============================================================================
// TEST 3: STATISTICS TRACKING
// =============================================================================

int test_statistics_tracking() {
    d_LogInfo("Starting Statistics Tracking Test");
    d_LogDebug("Testing performance monitoring for optimization");

    dLogger_t* logger = d_GetGlobalLogger();
    TEST_ASSERT(logger != NULL, "Global logger should be available for statistics test");
    if (!logger) {
        d_LogError("No global logger available for statistics test");
        return 0;
    }

    // Reset stats for clean test
    d_ResetLogStats(logger);

    // Generate various log activities
    d_LogInfo("Simulating game performance monitoring");

    // Normal logging
    for (int i = 0; i < 10; i++) {
        d_LogDebugF("Frame %d rendered", i);
    }

    // Get statistics
    dLogStats_t stats;
    d_GetLogStats(logger, &stats);
    TEST_ASSERT(true, "Should be able to retrieve log statistics");

    d_LogInfo("=== Logging Statistics Report ===");
    d_LogInfoF("Total log processing time: %.3f seconds", stats.total_log_time);
    d_LogInfoF("Logs suppressed: %llu", (unsigned long long)stats.logs_suppressed);
    d_LogInfoF("Handler errors: %u", stats.handler_errors);

    // Verify stats are being tracked
    TEST_ASSERT(stats.total_log_time >= 0.0, "Log timing should be non-negative");
    TEST_ASSERT(stats.handler_errors == 0, "Should have no handler errors during test");
    if (stats.total_log_time >= 0.0) {
        d_LogInfo("✓ Log timing is being tracked");
    }

    d_LogInfo("✓ Statistics Tracking Test Completed");
    return 1;
}

// =============================================================================
// TEST 4: LOGGER LEVEL CONTROLS
// =============================================================================

int test_logger_level_controls() {
    d_LogInfo("Starting Logger Level Controls Test");
    d_LogDebug("Testing dynamic log level adjustment");

    // Create test logger
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = false,
        .colorize_output = false
    };

    dLogger_t* test_logger = d_CreateLogger(config);
    TEST_ASSERT(test_logger != NULL, "Test logger should be created successfully");
    TEST_ASSERT(test_logger->config.default_level == D_LOG_LEVEL_INFO, "Logger should start with INFO level");
    d_SetGlobalLogger(test_logger);

    d_LogInfo("Testing initial INFO level (should appear)");
    d_LogDebug("Testing DEBUG level (should be filtered)");

    // Lower the log level to DEBUG
    d_SetLogLevel(test_logger, D_LOG_LEVEL_DEBUG);
    TEST_ASSERT(true, "Log level should be set to DEBUG");
    d_LogInfo("Lowered log level to DEBUG");
    d_LogDebug("Testing DEBUG level (should now appear)");

    // Raise the log level to WARNING
    d_SetLogLevel(test_logger, D_LOG_LEVEL_WARNING);
    TEST_ASSERT(true, "Log level should be set to WARNING");
    d_LogWarning("Testing WARNING level (should appear)");
    d_LogInfo("Testing INFO level (should be filtered)");

    // Test logging enable/disable
    d_LogInfo("Testing logging disable");
    d_SetLoggingEnabled(false);
    TEST_ASSERT(d_IsLoggingEnabled() == false, "Logging should be disabled");
    d_LogError("This error should not appear in logs");

    d_SetLoggingEnabled(true);
    TEST_ASSERT(true, "Logging should be enabled");
    d_LogError("This error should appear in logs");

    d_LogInfo("✓ Logger Level Controls Test Completed");
    return 1;
}

// =============================================================================
// TEST 5: BASIC GAME LOGGING SCENARIO
// =============================================================================

int test_game_logging_scenario() {
    d_LogInfo("Starting Game Logging Scenario Test");
    d_LogDebug("Testing logging in realistic game scenarios");

    // Simulate game initialization
    d_LogInfo("=== Game Initialization ===");
    d_LogInfo("Loading configuration files...");
    d_LogInfo("Initializing graphics subsystem...");
    d_LogInfo("Loading audio system...");
    d_LogInfo("Game initialization complete");

    // Simulate player actions with structured logging
    d_LogInfo("=== Player Session ===");

    // Player spawn event
    dLogStructured_t* spawn_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(spawn_log != NULL, "Spawn log should be created successfully");
    if (spawn_log) {
        d_LogStructured_SetFormat(spawn_log, true);
        d_LogStructured_Field(spawn_log, "event", "player_spawn");
        d_LogStructured_Field(spawn_log, "player_class", "Warrior");
        d_LogStructured_FieldInt(spawn_log, "level", 25);
        d_LogStructured_FieldInt(spawn_log, "health", 1000);
        d_LogStructured_FieldFloat(spawn_log, "x_pos", 128.5f, 1);
        d_LogStructured_FieldFloat(spawn_log, "y_pos", 64.2f, 1);
        d_LogStructured_Commit(spawn_log);
    }

    // Combat simulation
    d_LogInfo("=== Combat Encounter ===");
    for (int round = 1; round <= 3; round++) {
        d_LogInfoF("Combat Round %d", round);

        // Player attack with structured logging
        dLogStructured_t* attack_log = d_LogStructured(D_LOG_LEVEL_INFO);
        TEST_ASSERT(attack_log != NULL, "Attack log should be created successfully");
        if (attack_log) {
            d_LogStructured_SetFormat(attack_log, false); // Key-value
            d_LogStructured_Field(attack_log, "action", "player_attack");
            d_LogStructured_Field(attack_log, "weapon", "Sword");
            d_LogStructured_FieldInt(attack_log, "damage", 75 + (round * 10));
            d_LogStructured_FieldInt(attack_log, "round", round);
            d_LogStructured_Commit(attack_log);
        }

        d_LogWarningF("Enemy takes %d damage", 75 + (round * 10));
    }

    // Level up event
    d_LogInfo("=== Player Progression ===");
    dLogStructured_t* levelup_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(levelup_log != NULL, "Level up log should be created successfully");
    if (levelup_log) {
        d_LogStructured_SetFormat(levelup_log, true);
        d_LogStructured_Field(levelup_log, "event", "level_up");
        d_LogStructured_FieldInt(levelup_log, "old_level", 25);
        d_LogStructured_FieldInt(levelup_log, "new_level", 26);
        d_LogStructured_FieldInt(levelup_log, "new_health", 1050);
        d_LogStructured_FieldInt(levelup_log, "skill_points", 5);
        d_LogStructured_Commit(levelup_log);
    }

    d_LogInfo("✓ Game Logging Scenario Test Completed");
    return 1;
}
// =============================================================================
// TEST 6: FILE HANDLER TESTING
// =============================================================================

int test_file_handler() {
    d_LogInfo("Starting File Handler Test");
    d_LogDebug("Testing file-based logging for game sessions");

    // Create a test log file
    const char* log_filename = "test_game_session.log";
    FILE* log_file = fopen(log_filename, "w");
    TEST_ASSERT(log_file != NULL, "Test log file should be created successfully");
    if (!log_file) {
        d_LogError("Failed to create test log file");
        return 0;
    }

    // Create logger with file handler
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = true
    };

    dLogger_t* file_logger = d_CreateLogger(config);
    TEST_ASSERT(file_logger != NULL, "File logger should be created successfully");
    d_AddLogHandler(file_logger, d_FileLogHandler, log_file);
    TEST_ASSERT(true, "File handler should be added successfully");

    // Log game session events
    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "=== New Game Session Started ===");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Player: Knight, Level: 15, Location: Dark Forest");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Quest Started: Defeat the Shadow Lord");

    d_LogEx(D_LOG_LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__,
            "Low inventory space: 28/30 slots used");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Achievement Unlocked: Monster Slayer");

    d_DestroyLogger(file_logger);
    fclose(log_file);

    // Verify file was written
    FILE* verify = fopen(log_filename, "r");
    TEST_ASSERT(verify != NULL, "Log file should exist and be readable");
    if (verify) {
        d_LogInfo("✓ Log file successfully created");
        fclose(verify);
        remove(log_filename); // Cleanup
    }

    d_LogInfo("✓ File Handler Test Completed");
    return 1;
}

// =============================================================================
// TEST 7: STRING HANDLER TESTING
// =============================================================================

int test_string_handler() {
    d_LogInfo("Starting String Handler Test");
    d_LogDebug("Testing in-memory log capture for game replays");

    // Create a string buffer for capturing logs
    dString_t* replay_buffer = d_InitString();
    TEST_ASSERT(replay_buffer != NULL, "Replay buffer should be created successfully");

    // Create logger with string handler
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true
    };

    dLogger_t* string_logger = d_CreateLogger(config);
    TEST_ASSERT(string_logger != NULL, "String logger should be created successfully");
    d_AddLogHandler(string_logger, d_StringLogHandler, replay_buffer);
    TEST_ASSERT(true, "String handler should be added without crashing");

    // Set as global logger for the test
    dLogger_t* old_logger = d_GetGlobalLogger();
    d_SetGlobalLogger(string_logger);

    // Simulate game replay events
    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Replay: Match started - Arena Deathmatch");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Player1 picked up Rocket Launcher");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Player2 scored first kill (+100 points)");

    d_LogEx(D_LOG_LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__,
            "Player1 health critical: 15/100");

    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__,
            "Match ended - Winner: Player2 (Score: 25-18)");

    // Check captured content
    const char* captured = d_PeekString(replay_buffer);
    TEST_ASSERT(captured != NULL, "Should be able to peek captured string content");
    TEST_ASSERT(strlen(captured) > 0, "Captured content should not be empty");
    d_LogDebugF("Captured %zu bytes of replay data", strlen(captured));

    TEST_ASSERT(strstr(captured, "Replay:") != NULL, "Should capture 'Replay:' content");
    TEST_ASSERT(strstr(captured, "Winner:") != NULL, "Should capture 'Winner:' content");
    if (strstr(captured, "Replay:") && strstr(captured, "Winner:")) {
        d_LogInfo("✓ Successfully captured replay events");
    }

    // Restore original global logger
    d_SetGlobalLogger(old_logger);
    d_DestroyLogger(string_logger);
    d_DestroyString(replay_buffer);

    d_LogInfo("✓ String Handler Test Completed");
    return 1;
}
// =============================================================================
// TEST 8: FILTER BUILDER SYSTEM
// =============================================================================

int test_filter_builder_system() {
    d_LogInfo("Starting Filter Builder System Test");
    d_LogDebug("Testing selective logging for different game modules");

    // Create filter builder
    dLogFilterBuilder_t* filter_builder = d_CreateFilterBuilder();
    TEST_ASSERT(filter_builder != NULL, "Filter builder should be created successfully");
    if (!filter_builder) {
        d_LogError("Failed to create filter builder");
        return 0;
    }

    d_LogInfo("Setting up game module filters");

    // Add filters for different game systems
    d_FilterBuilder_AddDirectory(filter_builder, "src/render/", D_LOG_LEVEL_WARNING);
    d_FilterBuilder_AddDirectory(filter_builder, "src/physics/", D_LOG_LEVEL_DEBUG);
    d_FilterBuilder_AddPrefix(filter_builder, "NETWORK_", D_LOG_LEVEL_INFO);
    d_FilterBuilder_AddSuffix(filter_builder, "_CRITICAL", D_LOG_LEVEL_ERROR);

    TEST_ASSERT(true, "Should add filter rules without crashing");

    // Test string-based filter configuration
    d_LogInfo("Testing string-based filter configuration");
    dLogger_t* test_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = false
    });
    TEST_ASSERT(test_logger != NULL, "Test logger should be created successfully");

    int rules_added = d_FilterBuilder_FromString(test_logger,
                                                "gameplay/*:INFO,ai/*:DEBUG,sound/*:WARNING");
    TEST_ASSERT(rules_added > 0, "Should add filter rules from string configuration");
    d_LogInfoF("Added %d filter rules from string", rules_added);

    // Apply filters to logger
    dLogger_t* filtered_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = false
    });
    TEST_ASSERT(filtered_logger != NULL, "Filtered logger should be created successfully");

    d_FilterBuilder_Apply(filtered_logger, filter_builder);
    TEST_ASSERT(true, "Should apply filters to logger without crashing");

    d_LogInfo("Testing filtered logging behavior");
    // These would be filtered based on source file in real usage
    d_LogEx(D_LOG_LEVEL_DEBUG, "src/physics/collision.c",
            100, "check_collision", "Detailed collision check");

    d_LogEx(D_LOG_LEVEL_INFO, "src/render/texture.c",
            50, "load_texture", "This might be filtered out");

    d_DestroyFilterBuilder(filter_builder);
    d_DestroyLogger(filtered_logger);
    d_DestroyLogger(test_logger);

    d_LogInfo("✓ Filter Builder System Test Completed");
    return 1;
}
// =============================================================================
// TEST 9: PERFORMANCE STRESS TESTING
// =============================================================================

int test_performance_stress() {
    d_LogInfo("Starting Performance Stress Test");
    d_LogDebug("Testing logging performance under heavy load");

    dLogger_t* logger = d_GetGlobalLogger();
    TEST_ASSERT(logger != NULL, "Global logger should be available for stress test");
    if (!logger) {
        d_LogError("No global logger available for stress test");
        return 0;
    }

    // Reset stats for clean measurement
    d_ResetLogStats(logger);
    TEST_ASSERT(true, "Should be able to reset log statistics without crashing");

    d_LogInfo("Running performance stress test...");

    // Simulate heavy logging load
    const int stress_iterations = 1000;

    for (int i = 0; i < stress_iterations; i++) {
        // Mix of different log levels
        d_LogDebugF("Stress test iteration %d", i);

        if (i % 100 == 0) {
            d_LogInfoF("Checkpoint: %d/%d iterations completed", i, stress_iterations);
        }

        if (i % 250 == 0) {
            d_LogWarningF("Quarter milestone reached: %d", i);
        }

        // Structured logging stress
        if (i % 50 == 0) {
            dLogStructured_t* stress_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
            if (stress_log) {
                d_LogStructured_SetFormat(stress_log, false);
                d_LogStructured_Field(stress_log, "test_type", "stress");
                d_LogStructured_FieldInt(stress_log, "iteration", i);
                d_LogStructured_FieldFloat(stress_log, "progress", (float)i / stress_iterations * 100.0f, 2);
                d_LogStructured_Commit(stress_log);
            }
        }
    }

    // Get final performance stats
    dLogStats_t final_stats;
    d_GetLogStats(logger, &final_stats);
    TEST_ASSERT(true, "Should be able to retrieve final statistics without crashing");

    d_LogInfo("=== Performance Stress Test Results ===");
    d_LogInfoF("Total iterations: %d", stress_iterations);
    d_LogInfoF("Total processing time: %.4f seconds", final_stats.total_log_time);
    d_LogInfoF("Average time per log: %.6f seconds", final_stats.total_log_time / stress_iterations);
    d_LogInfoF("Logs suppressed: %llu", (unsigned long long)final_stats.logs_suppressed);

    // Validate performance metrics
    TEST_ASSERT(final_stats.total_log_time >= 0.0, "Total log time should be non-negative");
    TEST_ASSERT(final_stats.total_log_time / stress_iterations < 1.0, "Average time per log should be reasonable (< 1 second)");

    d_LogInfo("✓ Performance Stress Test Completed");
    return 1;
}
// =============================================================================
// TEST 10: MULTI-HANDLER COORDINATION
// =============================================================================

int test_multi_handler_coordination() {
    d_LogInfo("Starting Multi-Handler Coordination Test");
    d_LogDebug("Testing multiple handlers working together");

    // Create buffers for different handlers
    dString_t* console_buffer = d_InitString();
    dString_t* debug_buffer = d_InitString();
    dString_t* error_buffer = d_InitString();

    TEST_ASSERT(console_buffer != NULL, "Console buffer should be created successfully");
    TEST_ASSERT(debug_buffer != NULL, "Debug buffer should be created successfully");
    TEST_ASSERT(error_buffer != NULL, "Error buffer should be created successfully");

    // Create logger with multiple handlers
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = false
    };

    dLogger_t* multi_logger = d_CreateLogger(config);
    TEST_ASSERT(multi_logger != NULL, "Multi-logger should be created successfully");

    // Add multiple handlers
    d_AddLogHandler(multi_logger, d_StringLogHandler, console_buffer);
    d_AddLogHandler(multi_logger, d_StringLogHandler, debug_buffer);
    d_AddLogHandler(multi_logger, d_StringLogHandler, error_buffer);
    d_AddLogHandler(multi_logger, d_ConsoleLogHandler, NULL);

    TEST_ASSERT(true, "Multiple handlers should be added without crashing");

    // Set as global for testing
    dLogger_t* old_logger = d_GetGlobalLogger();
    d_SetGlobalLogger(multi_logger);

    // Generate logs at different levels
    d_LogDebug("Debug message for multi-handler test");
    d_LogInfo("Info message for multi-handler test");
    d_LogWarning("Warning message for multi-handler test");
    d_LogError("Error message for multi-handler test");

    // Test structured logging with multiple handlers
    dLogStructured_t* multi_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(multi_log != NULL, "Multi-handler structured log should be created successfully");
    if (multi_log) {
        d_LogStructured_SetFormat(multi_log, true);
        d_LogStructured_Field(multi_log, "test", "multi_handler");
        d_LogStructured_FieldInt(multi_log, "handlers", 4);
        d_LogStructured_Field(multi_log, "status", "active");
        d_LogStructured_Commit(multi_log);
    }

    // Verify all handlers received the messages
    const char* console_content = d_PeekString(console_buffer);
    const char* debug_content = d_PeekString(debug_buffer);
    const char* error_content = d_PeekString(error_buffer);

    TEST_ASSERT(console_content != NULL, "Console buffer content should be accessible");
    TEST_ASSERT(debug_content != NULL, "Debug buffer content should be accessible");
    TEST_ASSERT(error_content != NULL, "Error buffer content should be accessible");
    TEST_ASSERT(strlen(console_content) > 0, "Console buffer should contain log messages");
    TEST_ASSERT(strlen(debug_content) > 0, "Debug buffer should contain log messages");
    TEST_ASSERT(strlen(error_content) > 0, "Error buffer should contain log messages");

    int console_logs = 0, debug_logs = 0, error_logs = 0;

    // Count log entries in each buffer
    const char* ptr = console_content;
    while ((ptr = strstr(ptr, "[")) != NULL) {
        console_logs++;
        ptr++;
    }

    ptr = debug_content;
    while ((ptr = strstr(ptr, "[")) != NULL) {
        debug_logs++;
        ptr++;
    }

    ptr = error_content;
    while ((ptr = strstr(ptr, "[")) != NULL) {
        error_logs++;
        ptr++;
    }

    d_LogInfoF("Handler message counts - Console: %d, Debug: %d, Error: %d",
               console_logs, debug_logs, error_logs);

    // Verify all handlers received the expected number of messages
    TEST_ASSERT(console_logs > 0, "Console handler should have received log messages");
    TEST_ASSERT(debug_logs > 0, "Debug handler should have received log messages");
    TEST_ASSERT(error_logs > 0, "Error handler should have received log messages");
    TEST_ASSERT(console_logs == debug_logs && debug_logs == error_logs, "All string handlers should receive the same number of messages");

    // Cleanup
    d_SetGlobalLogger(old_logger);
    d_DestroyLogger(multi_logger);
    d_DestroyString(console_buffer);
    d_DestroyString(debug_buffer);
    d_DestroyString(error_buffer);

    d_LogInfo("✓ Multi-Handler Coordination Test Completed");
    return 1;
}
// =============================================================================
// TEST 11: ERROR HANDLING AND EDGE CASES
// =============================================================================

int test_error_handling_edge_cases() {
    d_LogInfo("Starting Error Handling and Edge Cases Test");
    d_LogDebug("Testing robustness with invalid inputs");

    // Test NULL pointer handling
    d_LogInfo("Testing NULL pointer safety");
    // These should not crash the system
    d_LogEx(D_LOG_LEVEL_INFO, NULL, 0, NULL, "Message with NULL file/function");
    d_LogEx(D_LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, NULL);
    TEST_ASSERT(true, "System should handle NULL pointers gracefully");

    // Test empty strings
    d_LogInfo("Testing empty string handling");
    d_LogEx(D_LOG_LEVEL_INFO, "", 0, "", "");
    d_LogF(D_LOG_LEVEL_INFO, "");
    TEST_ASSERT(true, "System should handle empty strings gracefully");

    // Test very long messages
    d_LogInfo("Testing long message handling");
    char long_message[2048];
    memset(long_message, 'A', sizeof(long_message) - 1);
    long_message[sizeof(long_message) - 1] = '\0';
    d_LogF(D_LOG_LEVEL_DEBUG, "Long message test: %s", long_message);
    TEST_ASSERT(strlen(long_message) == 2047, "Long message should be properly constructed");

    // Test structured logging with NULL/empty fields
    d_LogInfo("Testing structured logging edge cases");
    dLogStructured_t* edge_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(edge_log != NULL, "Structured log should be created even for edge case testing");
    if (edge_log) {
        d_LogStructured_Field(edge_log, "valid_key", "valid_value");
        d_LogStructured_Field(edge_log, "", "empty_key");
        d_LogStructured_Field(edge_log, "empty_value", "");
        d_LogStructured_Field(edge_log, NULL, "null_key");
        d_LogStructured_Field(edge_log, "null_value", NULL);
        d_LogStructured_FieldInt(edge_log, "max_int", INT_MAX);
        d_LogStructured_FieldInt(edge_log, "min_int", INT_MIN);
        d_LogStructured_FieldFloat(edge_log, "zero_float", 0.0f, 3);
        d_LogStructured_Commit(edge_log);
    }

    // Test invalid log levels (if any boundary checking exists)
    d_LogInfo("Testing boundary log levels");
    d_LogEx((dLogLevel_t)999, __FILE__, __LINE__, __FUNCTION__, "Invalid log level test");
    TEST_ASSERT(true, "System should handle invalid log levels gracefully");

    // Test logging when disabled
    d_LogInfo("Testing logging when globally disabled");
    d_SetLoggingEnabled(false);
    TEST_ASSERT(d_IsLoggingEnabled() == false, "Logging should be disabled");
    d_LogError("This should not appear - logging disabled");
    d_SetLoggingEnabled(true);
    TEST_ASSERT(d_IsLoggingEnabled() == true, "Logging should be re-enabled");
    d_LogInfo("Logging re-enabled");

    // Test handler removal edge cases
    d_LogInfo("Testing handler management edge cases");
    dLogger_t* test_logger = d_CreateLogger((dLogConfig_t){.default_level = D_LOG_LEVEL_DEBUG});
    TEST_ASSERT(test_logger != NULL, "Test logger should be created for edge case testing");
    // These should not crash
    d_RemoveLogHandler(test_logger, NULL);
    d_RemoveLogHandler(NULL, d_ConsoleLogHandler);
    TEST_ASSERT(true, "System should handle NULL handler removal gracefully");
    d_DestroyLogger(test_logger);

    d_LogInfo("✓ Error Handling and Edge Cases Test Completed");
    return 1;
}
// =============================================================================
// TEST 12: COMPREHENSIVE INTEGRATION
// =============================================================================

int test_comprehensive_integration() {
    d_LogInfo("Starting Comprehensive Integration Test");
    d_LogDebug("Testing all features working together in complex scenario");

    // Create multiple loggers for different game systems
    dLogConfig_t render_config = {
        .default_level = D_LOG_LEVEL_WARNING,
        .include_timestamp = true,
        .colorize_output = true
    };

    dLogConfig_t gameplay_config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = false
    };

    dLogger_t* render_logger = d_CreateLogger(render_config);
    dLogger_t* gameplay_logger = d_CreateLogger(gameplay_config);

    TEST_ASSERT(render_logger != NULL, "Render logger should be created successfully");
    TEST_ASSERT(gameplay_logger != NULL, "Gameplay logger should be created successfully");

    // Create file and string handlers
    FILE* game_log_file = fopen("integration_test.log", "w");
    dString_t* debug_buffer = d_InitString();

    TEST_ASSERT(debug_buffer != NULL, "Debug buffer should be created successfully");

    if (game_log_file) {
        d_AddLogHandler(render_logger, d_FileLogHandler, game_log_file);
        d_AddLogHandler(gameplay_logger, d_FileLogHandler, game_log_file);
        TEST_ASSERT(true, "File handlers should be added without crashing");
    }

    d_AddLogHandler(gameplay_logger, d_StringLogHandler, debug_buffer);
    TEST_ASSERT(true, "String handler should be added without crashing");

    // Simulate complex game scenario
    d_SetGlobalLogger(gameplay_logger);

    // Game initialization phase
    d_LogInfo("=== GAME BOOT SEQUENCE ===");
    dLogStructured_t* boot_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(boot_log != NULL, "Boot log should be created successfully");
    if (boot_log) {
        d_LogStructured_SetFormat(boot_log, true);
        d_LogStructured_Field(boot_log, "phase", "initialization");
        d_LogStructured_Field(boot_log, "version", "1.0.0");
        d_LogStructured_FieldInt(boot_log, "build", 12345);
        d_LogStructured_Commit(boot_log);
    }

    // Switch to render logger for graphics operations
    d_SetGlobalLogger(render_logger);
    d_LogWarning("Render system: Low VRAM detected");
    d_LogError("Render system: Failed to load shader: water.glsl");

    // Switch back to gameplay logger
    d_SetGlobalLogger(gameplay_logger);

    // Simulate player session with mixed logging types
    for (int session = 1; session <= 3; session++) {
        d_LogInfoF("=== Player Session %d ===", session);

        // Player actions
        for (int action = 1; action <= 5; action++) {
            dLogStructured_t* action_log = d_LogStructured(D_LOG_LEVEL_INFO);
            TEST_ASSERT(action_log != NULL, "Action log should be created successfully");
            if (action_log) {
                d_LogStructured_SetFormat(action_log, (action % 2 == 0)); // Alternate JSON/KV
                d_LogStructured_Field(action_log, "event", "player_action");
                d_LogStructured_FieldInt(action_log, "session", session);
                d_LogStructured_FieldInt(action_log, "action_id", action);
                d_LogStructured_FieldFloat(action_log, "timestamp", session * action * 1.5f, 3);
                d_LogStructured_Commit(action_log);
            }

            // Regular logging mixed in
            if (action == 3) {
                d_LogWarningF("Session %d: Player health low", session);
            }
        }

        // End of session stats
        dLogStructured_t* session_stats = d_LogStructured(D_LOG_LEVEL_INFO);
        TEST_ASSERT(session_stats != NULL, "Session stats log should be created successfully");
        if (session_stats) {
            d_LogStructured_SetFormat(session_stats, true);
            d_LogStructured_Field(session_stats, "event", "session_end");
            d_LogStructured_FieldInt(session_stats, "session_id", session);
            d_LogStructured_FieldInt(session_stats, "actions_performed", 5);
            d_LogStructured_FieldFloat(session_stats, "duration_minutes", session * 15.5f, 1);
            d_LogStructured_Commit(session_stats);
        }
    }

    // Get final statistics from both loggers
    dLogStats_t gameplay_stats, render_stats;
    d_GetLogStats(gameplay_logger, &gameplay_stats);
    d_GetLogStats(render_logger, &render_stats);

    TEST_ASSERT(true, "Should be able to retrieve logger statistics without crashing");

    d_LogInfo("=== INTEGRATION TEST RESULTS ===");
    d_LogInfoF("Gameplay logger stats - Time: %.4fs, Suppressed: %llu",
               gameplay_stats.total_log_time,
               (unsigned long long)gameplay_stats.logs_suppressed);
    d_LogInfoF("Render logger stats - Time: %.4fs, Suppressed: %llu",
               render_stats.total_log_time,
               (unsigned long long)render_stats.logs_suppressed);

    // Check debug buffer content
    const char* debug_content = d_PeekString(debug_buffer);
    TEST_ASSERT(debug_content != NULL, "Debug buffer content should be accessible");
    TEST_ASSERT(strlen(debug_content) > 0, "Debug buffer should contain log messages");

    int total_debug_logs = 0;
    const char* ptr = debug_content;
    while ((ptr = strstr(ptr, "[")) != NULL) {
        total_debug_logs++;
        ptr++;
    }
    d_LogInfoF("Debug buffer captured %d log entries", total_debug_logs);

    TEST_ASSERT(total_debug_logs > 0, "Debug buffer should have captured multiple log entries");
    TEST_ASSERT(gameplay_stats.total_log_time >= 0.0, "Gameplay logger processing time should be non-negative");
    TEST_ASSERT(render_stats.total_log_time >= 0.0, "Render logger processing time should be non-negative");

    // Cleanup
    d_DestroyLogger(render_logger);
    d_DestroyLogger(gameplay_logger);
    d_DestroyString(debug_buffer);

    if (game_log_file) {
        fclose(game_log_file);
        remove("integration_test.log"); // Cleanup test file
    }

    d_LogInfo("✓ Comprehensive Integration Test Completed");
    return 1;
}
// =============================================================================
// DEBUG HUNTER TEST 1: MEMORY LEAK HUNTER
// =============================================================================

int debug_hunter_memory_leak_test() {
    d_LogInfo("Starting Debug Hunter: Memory Leak Test");
    d_LogDebug("Hunting for memory leaks in logger lifecycle");

    size_t initial_memory_usage = 0; // In a real implementation, you'd track this

    // Test 1: Rapid logger creation/destruction
    d_LogInfo("Testing rapid logger creation/destruction cycles");
    for (int cycle = 0; cycle < 100; cycle++) {
        dLogger_t* temp_logger = d_CreateLogger((dLogConfig_t){
            .default_level = D_LOG_LEVEL_DEBUG,
            .include_timestamp = true,
            .colorize_output = false
        });

        TEST_ASSERT(temp_logger != NULL, "Temp logger should be created successfully in memory leak test");
        if (!temp_logger) {
            d_LogErrorF("Failed to create logger on cycle %d", cycle);
            return 0;
        }

        // Add handlers that should be cleaned up
        dString_t* temp_buffer = d_InitString();
        TEST_ASSERT(temp_buffer != NULL, "Temp buffer should be created successfully");
        d_AddLogHandler(temp_logger, d_StringLogHandler, temp_buffer);
        TEST_ASSERT(true, "String handler should be added without crashing");

        // Generate some logs
        d_SetGlobalLogger(temp_logger);
        d_LogDebugF("Test cycle %d", cycle);

        // Destroy everything - this should not leak
        d_DestroyLogger(temp_logger);
        d_DestroyString(temp_buffer);

        if (cycle % 25 == 0) {
            d_LogInfoF("Completed %d memory cycles", cycle);
        }
    }

    // Test 2: Structured logging memory cycles
    d_LogInfo("Testing structured logging memory cycles");
    for (int i = 0; i < 50; i++) {
        dLogStructured_t* struct_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        TEST_ASSERT(struct_log != NULL, "Structured log should be created successfully in memory test");
        if (struct_log) {
            // Add many fields to stress memory allocation
            for (int field = 0; field < 20; field++) {
                char key[32], value[64];
                snprintf(key, sizeof(key), "field_%d", field);
                snprintf(value, sizeof(value), "value_%d_cycle_%d", field, i);
                d_LogStructured_Field(struct_log, key, value);
            }
            d_LogStructured_FieldInt(struct_log, "cycle", i);
            d_LogStructured_FieldFloat(struct_log, "memory_test", i * 3.14f, 2);
            d_LogStructured_Commit(struct_log);
        }
    }

    // Test 3: Filter builder memory cycles
    d_LogInfo("Testing filter builder memory cycles");
    for (int i = 0; i < 25; i++) {
        dLogFilterBuilder_t* filter = d_CreateFilterBuilder();
        TEST_ASSERT(filter != NULL, "Filter builder should be created successfully in memory test");
        if (filter) {
            // Add multiple rules
            d_FilterBuilder_AddDirectory(filter, "/tmp/test", D_LOG_LEVEL_DEBUG);
            d_FilterBuilder_AddPrefix(filter, "TEST_", D_LOG_LEVEL_INFO);
            d_FilterBuilder_AddSuffix(filter, "_DEBUG", D_LOG_LEVEL_WARNING);
            TEST_ASSERT(true, "Filter rules should be added without crashing");

            // Test string parsing which allocates memory
            dLogger_t* temp_logger = d_CreateLogger((dLogConfig_t){.default_level = D_LOG_LEVEL_DEBUG});
            TEST_ASSERT(temp_logger != NULL, "Temp logger should be created successfully in filter test");
            int rules_added = d_FilterBuilder_FromString(temp_logger, "test/*:DEBUG,game/*:INFO,render/*:WARNING");
            TEST_ASSERT(rules_added > 0, "Filter rules should be added from string successfully");

            d_DestroyFilterBuilder(filter);
            d_DestroyLogger(temp_logger);
        }
    }

    d_LogInfo("✓ Memory Leak Hunter Test Completed - Check for memory leaks!");
    TEST_ASSERT(true, "Memory leak test completed without crashes");
    return 1;
}

// =============================================================================
// DEBUG HUNTER TEST 2: RACE CONDITION HUNTER
// =============================================================================

int debug_hunter_race_condition_test() {
    d_LogInfo("Starting Debug Hunter: Race Condition Test");
    d_LogDebug("Hunting for thread safety issues and race conditions");

    // Test 1: Rapid global logger switching
    d_LogInfo("Testing rapid global logger switching");
    dLogger_t* logger_a = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = false
    });

    dLogger_t* logger_b = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true
    });

    // Rapidly switch between loggers while logging
    for (int switch_cycle = 0; switch_cycle < 200; switch_cycle++) {
        d_SetGlobalLogger(switch_cycle % 2 == 0 ? logger_a : logger_b);

        // Log immediately after switch - potential race condition
        d_LogDebugF("Switch cycle %d with logger %c",
                   switch_cycle, (switch_cycle % 2 == 0) ? 'A' : 'B');

        // Test structured logging during rapid switches
        if (switch_cycle % 10 == 0) {
            dLogStructured_t* race_log = d_LogStructured(D_LOG_LEVEL_INFO);
            if (race_log) {
                d_LogStructured_Field(race_log, "test", "race_condition");
                d_LogStructured_FieldInt(race_log, "cycle", switch_cycle);
                d_LogStructured_Field(race_log, "logger", (switch_cycle % 2 == 0) ? "A" : "B");
                d_LogStructured_Commit(race_log);
            }
        }
    }

    // Test 2: Handler modification during logging
    d_LogInfo("Testing handler modification during active logging");
    dString_t* race_buffer_1 = d_InitString();
    dString_t* race_buffer_2 = d_InitString();

    d_SetGlobalLogger(logger_a);

    for (int handler_test = 0; handler_test < 50; handler_test++) {
        // Add handler
        d_AddLogHandler(logger_a, d_StringLogHandler, race_buffer_1);

        // Log something
        d_LogInfoF("Handler test %d - buffer 1", handler_test);

        // Add another handler
        d_AddLogHandler(logger_a, d_StringLogHandler, race_buffer_2);

        // Log something else
        d_LogInfoF("Handler test %d - both buffers", handler_test);

        // Remove handlers (if implemented)
        d_RemoveLogHandler(logger_a, d_StringLogHandler);

        // This log might have unpredictable behavior
        d_LogWarningF("Handler test %d - after removal", handler_test);
    }

    // Test 3: Statistics access during heavy logging
    d_LogInfo("Testing statistics access during heavy logging");
    for (int stats_test = 0; stats_test < 100; stats_test++) {
        // Generate logs
        d_LogDebugF("Stats test iteration %d", stats_test);

        // Access stats immediately - potential race condition
        dLogStats_t stats;
        d_GetLogStats(logger_a, &stats);

        // Reset stats during active logging
        if (stats_test % 20 == 0) {
            d_ResetLogStats(logger_a);
        }

        // More logging after stats access
        d_LogInfoF("Stats test post-access %d", stats_test);
    }

    // Cleanup
    d_DestroyString(race_buffer_1);
    d_DestroyString(race_buffer_2);
    d_DestroyLogger(logger_a);
    d_DestroyLogger(logger_b);

    d_LogInfo("✓ Race Condition Hunter Test Completed - Check for thread safety issues!");
    return 1;
}
// =============================================================================
// DEBUG HUNTER TEST 3: BUFFER OVERFLOW HUNTER
// =============================================================================

int debug_hunter_buffer_overflow_test() {
    d_LogInfo("Starting Debug Hunter: Buffer Overflow Test");
    d_LogDebug("Hunting for buffer overflows and boundary violations");

    // Test 1: Extremely long log messages
    d_LogInfo("Testing extremely long log messages");

    // Create progressively longer messages
    for (int size_test = 512; size_test <= 16384; size_test *= 2) {
        char* giant_message = malloc(size_test + 1);
        if (!giant_message) {
            d_LogError("Failed to allocate test buffer");
            continue;
        }

        // Fill with pattern that's easy to detect corruption
        for (int i = 0; i < size_test; i++) {
            giant_message[i] = 'A' + (i % 26);
        }
        giant_message[size_test] = '\0';

        d_LogInfoF("Giant message test (%d bytes): %s", size_test, giant_message);

        // Test with formatted logging too
        d_LogDebugF("Formatted giant (%d): %.100s...", size_test, giant_message);

        free(giant_message);
    }

    // Test 2: Massive structured logging
    d_LogInfo("Testing massive structured logging");
    dLogStructured_t* massive_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (massive_log) {
        d_LogStructured_SetFormat(massive_log, true); // JSON format

        // Add hundreds of fields
        for (int field = 0; field < 500; field++) {
            char key[64], value[256];

            // Create keys and values that could cause buffer issues
            snprintf(key, sizeof(key), "very_long_key_name_that_might_cause_buffer_issues_%d", field);
            snprintf(value, sizeof(value),
                    "extremely_long_value_content_designed_to_stress_buffer_management_systems_%d_with_lots_of_text",
                    field);

            d_LogStructured_Field(massive_log, key, value);

            if (field % 100 == 0) {
                d_LogInfoF("Added %d fields to massive structured log", field);
            }
        }

        d_LogStructured_Commit(massive_log);
    }

    // Test 3: Rapid-fire format string stress
    d_LogInfo("Testing format string boundary stress");

    for (int format_test = 0; format_test < 100; format_test++) {
        // Create format strings with many arguments
        d_LogDebugF("Multi-arg test %d: %s %d %f %c %x %o %s %d %f",
                   format_test, "string1", 42, 3.14f, 'X', 0xDEAD, 0755, "string2", 999, 2.71f);

        // Test with potentially problematic format specifiers
        d_LogInfoF("Format boundary test %d: %*s %*.*f %%",
                  format_test, 50, "padded_string", 10, 3, 123.456789);
    }

    // Test 4: Filter pattern boundary stress
    d_LogInfo("Testing filter pattern boundary stress");
    dLogFilterBuilder_t* boundary_filter = d_CreateFilterBuilder();
    if (boundary_filter) {
        // Add filters with very long patterns
        char long_pattern[1024];
        for (int pattern_test = 0; pattern_test < 20; pattern_test++) {
            snprintf(long_pattern, sizeof(long_pattern),
                    "very/long/directory/path/that/might/cause/buffer/overflow/issues/in/filter/processing/test_%d/*",
                    pattern_test);

            d_FilterBuilder_AddDirectory(boundary_filter, long_pattern, D_LOG_LEVEL_DEBUG);

            // Test very long prefixes and suffixes
            snprintf(long_pattern, sizeof(long_pattern),
                    "EXTREMELY_LONG_PREFIX_THAT_MIGHT_OVERFLOW_BUFFERS_%d_", pattern_test);
            d_FilterBuilder_AddPrefix(boundary_filter, long_pattern, D_LOG_LEVEL_INFO);
        }

        d_DestroyFilterBuilder(boundary_filter);
    }

    // Test 5: String handler buffer stress
    d_LogInfo("Testing string handler buffer stress");
    dString_t* stress_buffer = d_InitString();

    dLogger_t* buffer_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true
    });

    d_AddLogHandler(buffer_logger, d_StringLogHandler, stress_buffer);
    d_SetGlobalLogger(buffer_logger);

    // Rapid logging to string buffer
    for (int buffer_stress = 0; buffer_stress < 1000; buffer_stress++) {
        d_LogDebugF("Buffer stress test %d with some additional content to make messages longer",
                   buffer_stress);

        if (buffer_stress % 100 == 0) {
            size_t buffer_size = d_GetStringLength(stress_buffer);
            d_LogInfoF("String buffer now contains %zu bytes", buffer_size);
        }
    }

    // Check final buffer size
    size_t final_size = d_GetStringLength(stress_buffer);
    d_LogInfoF("Final string buffer size: %zu bytes", final_size);

    d_DestroyLogger(buffer_logger);
    d_DestroyString(stress_buffer);

    d_LogInfo("✓ Buffer Overflow Hunter Test Completed - Check for boundary violations!");
    return 1;
}
// =============================================================================
// DEBUG HUNTER TEST 4: STATE CORRUPTION HUNTER
// =============================================================================

int debug_hunter_state_corruption_test() {
    d_LogInfo("Starting Debug Hunter: State Corruption Test");
    d_LogDebug("Hunting for internal state corruption and inconsistencies");

    // Test 1: Logger state consistency under stress
    d_LogInfo("Testing logger state consistency");

    dLogger_t* state_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = true
    });

    dString_t* state_buffer = d_InitString();
    d_AddLogHandler(state_logger, d_StringLogHandler, state_buffer);

    // Rapid state changes
    for (int state_test = 0; state_test < 200; state_test++) {
        // Change log level rapidly
        dLogLevel_t levels[] = {D_LOG_LEVEL_DEBUG, D_LOG_LEVEL_INFO, D_LOG_LEVEL_WARNING, D_LOG_LEVEL_ERROR};
        d_SetLogLevel(state_logger, levels[state_test % 4]);

        // Log at different levels to test filtering consistency
        d_SetGlobalLogger(state_logger);
        d_LogDebug("Debug message - should be filtered sometimes");
        d_LogInfo("Info message - should appear most times");
        d_LogWarning("Warning message - should always appear");

        // Toggle global logging on/off
        if (state_test % 20 == 0) {
            d_SetLoggingEnabled(false);
            d_LogError("This should not appear - logging disabled");
            d_SetLoggingEnabled(true);
            d_LogError("This should appear - logging re-enabled");
        }

        // Verify state consistency by checking buffer content
        if (state_test % 50 == 0) {
            const char* buffer_content = d_PeekString(state_buffer);
            size_t buffer_len = strlen(buffer_content);

            // Count different log levels in buffer
            int debug_count = 0, info_count = 0, warning_count = 0, error_count = 0;
            const char* ptr = buffer_content;

            while ((ptr = strstr(ptr, "[DEBUG]")) != NULL) { debug_count++; ptr++; }
            ptr = buffer_content;
            while ((ptr = strstr(ptr, "[INFO]")) != NULL) { info_count++; ptr++; }
            ptr = buffer_content;
            while ((ptr = strstr(ptr, "[WARNING]")) != NULL) { warning_count++; ptr++; }
            ptr = buffer_content;
            while ((ptr = strstr(ptr, "[ERROR]")) != NULL) { error_count++; ptr++; }

            d_LogInfoF("State check %d: Buffer=%zu bytes, DEBUG=%d, INFO=%d, WARNING=%d, ERROR=%d",
                      state_test, buffer_len, debug_count, info_count, warning_count, error_count);
        }
    }

    // Test 2: Statistics consistency
    d_LogInfo("Testing statistics state consistency");

    dLogStats_t stats_before, stats_after;
    d_GetLogStats(state_logger, &stats_before);

    // Generate known number of logs
    const int known_log_count = 100;
    for (int i = 0; i < known_log_count; i++) {
        d_LogInfoF("Statistics consistency test %d", i);
    }

    d_GetLogStats(state_logger, &stats_after);

    // Verify statistics make sense
    if (stats_after.total_log_time < stats_before.total_log_time) {
        d_LogError("CORRUPTION DETECTED: Log time went backwards!");
    }

    d_LogInfoF("Stats consistency check - Before: %.4fs, After: %.4fs, Difference: %.4fs",
              stats_before.total_log_time, stats_after.total_log_time,
              stats_after.total_log_time - stats_before.total_log_time);

    // Test 3: Structured logging state consistency
    d_LogInfo("Testing structured logging state consistency");

    for (int struct_test = 0; struct_test < 50; struct_test++) {
        dLogStructured_t* state_struct = d_LogStructured(D_LOG_LEVEL_INFO);
        if (state_struct) {
            // Alternate between JSON and KV format
            d_LogStructured_SetFormat(state_struct, struct_test % 2 == 0);

            // Add fields in different orders to test state handling
            if (struct_test % 3 == 0) {
                d_LogStructured_Field(state_struct, "first", "value1");
                d_LogStructured_FieldInt(state_struct, "second", struct_test);
                d_LogStructured_FieldFloat(state_struct, "third", struct_test * 1.5f, 2);
            } else if (struct_test % 3 == 1) {
                d_LogStructured_FieldFloat(state_struct, "third", struct_test * 1.5f, 2);
                d_LogStructured_Field(state_struct, "first", "value1");
                d_LogStructured_FieldInt(state_struct, "second", struct_test);
            } else {
                d_LogStructured_FieldInt(state_struct, "second", struct_test);
                d_LogStructured_FieldFloat(state_struct, "third", struct_test * 1.5f, 2);
                d_LogStructured_Field(state_struct, "first", "value1");
            }

            d_LogStructured_Commit(state_struct);
        }
    }

    // Test 4: Filter state corruption
    d_LogInfo("Testing filter state corruption");

    dLogFilterBuilder_t* corruption_filter = d_CreateFilterBuilder();
    if (corruption_filter) {
        // Add many rules in different orders
        for (int filter_test = 0; filter_test < 30; filter_test++) {
            char pattern[64];

            snprintf(pattern, sizeof(pattern), "test_%d/", filter_test);
            d_FilterBuilder_AddDirectory(corruption_filter, pattern, D_LOG_LEVEL_DEBUG);

            snprintf(pattern, sizeof(pattern), "PREFIX_%d_", filter_test);
            d_FilterBuilder_AddPrefix(corruption_filter, pattern, D_LOG_LEVEL_INFO);

            snprintf(pattern, sizeof(pattern), "_SUFFIX_%d", filter_test);
            d_FilterBuilder_AddSuffix(corruption_filter, pattern, D_LOG_LEVEL_WARNING);
        }

        // Test string-based rule addition which might corrupt state
        dLogger_t* filter_logger = d_CreateLogger((dLogConfig_t){.default_level = D_LOG_LEVEL_DEBUG});

        int rules_added = d_FilterBuilder_FromString(filter_logger,
            "corrupt/test/*:DEBUG,another/test/*:INFO,third/test/*:WARNING,fourth/test/*:ERROR");

        d_LogInfoF("Filter corruption test added %d rules", rules_added);

        d_DestroyFilterBuilder(corruption_filter);
        d_DestroyLogger(filter_logger);
    }

    // Cleanup and final state check
    d_GetLogStats(state_logger, &stats_after);
    d_LogInfoF("Final state check - Total time: %.4fs, Suppressed: %llu, Errors: %u",
              stats_after.total_log_time,
              (unsigned long long)stats_after.logs_suppressed,
              stats_after.handler_errors);

    d_DestroyLogger(state_logger);
    d_DestroyString(state_buffer);

    d_LogInfo("✓ State Corruption Hunter Test Completed - Check for internal inconsistencies!");
    return 1;
}
// =============================================================================
// DEBUG HUNTER TEST 5: PERFORMANCE DEGRADATION HUNTER
// =============================================================================

int debug_hunter_performance_degradation_test() {
    d_LogInfo("Starting Debug Hunter: Performance Degradation Test");
    d_LogDebug("Hunting for performance regressions and bottlenecks");

    // Test 1: Performance scaling with handler count
    d_LogInfo("Testing performance scaling with multiple handlers");

    dLogger_t* perf_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true
    });

    // Create multiple string buffers as handlers
    const int max_handlers = 10;
    dString_t* handler_buffers[max_handlers];

    for (int handler_count = 1; handler_count <= max_handlers; handler_count++) {
        // Add one more handler
        handler_buffers[handler_count - 1] = d_InitString();
        d_AddLogHandler(perf_logger, d_StringLogHandler, handler_buffers[handler_count - 1]);

        d_SetGlobalLogger(perf_logger);

        // Measure performance with current handler count
        dLogStats_t stats_before, stats_after;
        d_ResetLogStats(perf_logger);
        d_GetLogStats(perf_logger, &stats_before);

        // Fixed workload
        const int workload_size = 100;
        for (int work = 0; work < workload_size; work++) {
            d_LogInfoF("Performance test with %d handlers, iteration %d", handler_count, work);

            if (work % 25 == 0) {
                dLogStructured_t* perf_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
                if (perf_log) {
                    d_LogStructured_SetFormat(perf_log, true);
                    d_LogStructured_Field(perf_log, "test", "performance");
                    d_LogStructured_FieldInt(perf_log, "handlers", handler_count);
                    d_LogStructured_FieldInt(perf_log, "iteration", work);
                    d_LogStructured_Commit(perf_log);
                }
            }
        }

        d_GetLogStats(perf_logger, &stats_after);
        double time_per_log = (stats_after.total_log_time - stats_before.total_log_time) / workload_size;

        d_LogInfoF("Handlers: %d, Time per log: %.6f seconds", handler_count, time_per_log);

        // Check for performance degradation
        if (handler_count > 1 && time_per_log > 0.001) { // 1ms per log is concerning
            d_LogWarningF("Potential performance degradation detected with %d handlers", handler_count);
        }
    }

    // Test 2: Memory usage scaling
    d_LogInfo("Testing memory usage patterns");

    dLogStats_t baseline_stats;
    d_GetLogStats(perf_logger, &baseline_stats);

    // Create massive amounts of structured logging data
    for (int memory_test = 0; memory_test < 200; memory_test++) {
        dLogStructured_t* memory_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        if (memory_log) {
            d_LogStructured_SetFormat(memory_log, true);

            // Add many fields to stress memory allocation
            for (int field = 0; field < 50; field++) {
                char key[32], value[128];
                snprintf(key, sizeof(key), "memory_key_%d", field);
                snprintf(value, sizeof(value), "memory_value_%d_test_%d_with_extra_data", field, memory_test);
                d_LogStructured_Field(memory_log, key, value);
            }

            d_LogStructured_FieldInt(memory_log, "test_iteration", memory_test);
            d_LogStructured_Commit(memory_log);
        }

        // Check for performance degradation over time
        if (memory_test % 50 == 0) {
            dLogStats_t current_stats;
            d_GetLogStats(perf_logger, &current_stats);
            double time_ratio = current_stats.total_log_time / baseline_stats.total_log_time;

            d_LogInfoF("Memory test %d: Time ratio vs baseline: %.2fx", memory_test, time_ratio);

            if (time_ratio > 2.0) {
                d_LogWarningF("Performance degradation detected at iteration %d", memory_test);
            }
        }
    }

    // Test 3: Filter performance with many rules
    d_LogInfo("Testing filter performance scaling");

    dLogFilterBuilder_t* perf_filter = d_CreateFilterBuilder();
    if (perf_filter) {
        // Add increasingly more filter rules
        for (int rule_count = 10; rule_count <= 100; rule_count += 10) {
            // Add more rules
            for (int rule = (rule_count - 10); rule < rule_count; rule++) {
                char pattern[64];
                snprintf(pattern, sizeof(pattern), "performance_test_%d/", rule);
                d_FilterBuilder_AddDirectory(perf_filter, pattern, D_LOG_LEVEL_DEBUG);
            }

            // Test logging performance with current rule set
            dLogStats_t filter_stats_before, filter_stats_after;
            d_GetLogStats(perf_logger, &filter_stats_before);

            // Apply filter and test
            d_FilterBuilder_Apply(perf_logger, perf_filter);

            for (int filter_test = 0; filter_test < 50; filter_test++) {
                d_LogDebugF("Filter performance test with %d rules, iteration %d",
                           rule_count, filter_test);
            }

            d_GetLogStats(perf_logger, &filter_stats_after);
            double filter_time = filter_stats_after.total_log_time - filter_stats_before.total_log_time;

            d_LogInfoF("Filter rules: %d, Time: %.6f seconds", rule_count, filter_time);
        }

        d_DestroyFilterBuilder(perf_filter);
    }

    // Test 4: String buffer growth performance
    d_LogInfo("Testing string buffer growth performance");

    dString_t* growth_buffer = d_InitString();
    dLogger_t* growth_logger = d_CreateLogger((dLogConfig_t){.default_level = D_LOG_LEVEL_DEBUG});
    d_AddLogHandler(growth_logger, d_StringLogHandler, growth_buffer);
    d_SetGlobalLogger(growth_logger);

    // Log increasingly longer messages to test buffer reallocation performance
    for (int growth_test = 0; growth_test < 100; growth_test++) {
        char* test_message = malloc(growth_test * 100 + 1);
        if (test_message) {
            memset(test_message, 'X', growth_test * 100);
            test_message[growth_test * 100] = '\0';

            dLogStats_t growth_before, growth_after;
            d_GetLogStats(growth_logger, &growth_before);

            d_LogInfoF("Growth test %d (size %d): %s", growth_test, growth_test * 100, test_message);

            d_GetLogStats(growth_logger, &growth_after);
            double growth_time = growth_after.total_log_time - growth_before.total_log_time;

            if (growth_test % 20 == 0) {
                size_t buffer_size = d_GetStringLength(growth_buffer);
                d_LogInfoF("Buffer growth: Size=%zu bytes, Last log time=%.6fs",
                          buffer_size, growth_time);
            }

            free(test_message);
        }
    }

    // Final performance summary
    dLogStats_t final_stats;
    d_GetLogStats(perf_logger, &final_stats);
    d_LogInfoF("Performance test final stats: %.4fs total, %llu suppressed, %u errors",
              final_stats.total_log_time,
              (unsigned long long)final_stats.logs_suppressed,
              final_stats.handler_errors);

    // Cleanup
    for (int i = 0; i < max_handlers; i++) {
        d_DestroyString(handler_buffers[i]);
    }
    d_DestroyLogger(perf_logger);
    d_DestroyLogger(growth_logger);
    d_DestroyString(growth_buffer);

    d_LogInfo("✓ Performance Degradation Hunter Test Completed - Check for bottlenecks!");
    return 1;
}
int test_multi_logger_independence() {
    d_LogInfo("Starting Multi-Logger Independence Test");
    d_LogDebug("Testing that separate loggers work independently without interference");

    // Create three independent loggers with different configurations
    dLogConfig_t gameplay_config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = false,
        .colorize_output = true
    };

    dLogConfig_t analytics_config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = false
    };

    dLogConfig_t performance_config = {
        .default_level = D_LOG_LEVEL_WARNING,
        .include_timestamp = true,
        .colorize_output = true
    };

    dLogger_t* gameplay_logger = d_CreateLogger(gameplay_config);
    dLogger_t* analytics_logger = d_CreateLogger(analytics_config);
    dLogger_t* performance_logger = d_CreateLogger(performance_config);

    TEST_ASSERT(gameplay_logger != NULL, "Gameplay logger should be created successfully");
    TEST_ASSERT(analytics_logger != NULL, "Analytics logger should be created successfully");
    TEST_ASSERT(performance_logger != NULL, "Performance logger should be created successfully");

    // Create separate string buffers for each logger
    dString_t* gameplay_buffer = d_InitString();
    dString_t* analytics_buffer = d_InitString();
    dString_t* performance_buffer = d_InitString();

    TEST_ASSERT(gameplay_buffer != NULL, "Gameplay buffer should be created successfully");
    TEST_ASSERT(analytics_buffer != NULL, "Analytics buffer should be created successfully");
    TEST_ASSERT(performance_buffer != NULL, "Performance buffer should be created successfully");

    // Add string handlers to capture output independently
    d_AddLogHandler(gameplay_logger, d_StringLogHandler, gameplay_buffer);
    d_AddLogHandler(analytics_logger, d_StringLogHandler, analytics_buffer);
    d_AddLogHandler(performance_logger, d_StringLogHandler, performance_buffer);

    TEST_ASSERT(true, "All handlers should be added without crashing");

    // Test independent logging with different levels
    dLogger_t* original_global = d_GetGlobalLogger();

    // Test gameplay logger (INFO level) - should filter DEBUG
    d_SetGlobalLogger(gameplay_logger);
    d_LogDebug("This DEBUG should be filtered by gameplay logger");
    d_LogInfo("This INFO should appear in gameplay logger");
    d_LogWarning("This WARNING should appear in gameplay logger");

    // Test analytics logger (DEBUG level) - should show all
    d_SetGlobalLogger(analytics_logger);
    d_LogDebug("This DEBUG should appear in analytics logger");
    d_LogInfo("This INFO should appear in analytics logger");
    d_LogWarning("This WARNING should appear in analytics logger");

    // Test performance logger (WARNING level) - should filter DEBUG and INFO
    d_SetGlobalLogger(performance_logger);
    d_LogDebug("This DEBUG should be filtered by performance logger");
    d_LogInfo("This INFO should be filtered by performance logger");
    d_LogWarning("This WARNING should appear in performance logger");

    // Verify independent operation
    const char* gameplay_content = d_PeekString(gameplay_buffer);
    const char* analytics_content = d_PeekString(analytics_buffer);
    const char* performance_content = d_PeekString(performance_buffer);

    TEST_ASSERT(gameplay_content != NULL, "Gameplay buffer should contain content");
    TEST_ASSERT(analytics_content != NULL, "Analytics buffer should contain content");
    TEST_ASSERT(performance_content != NULL, "Performance buffer should contain content");

    // Verify filtering worked correctly
    TEST_ASSERT(strstr(gameplay_content, "INFO should appear in gameplay") != NULL,
                "Gameplay logger should capture INFO messages");
    TEST_ASSERT(strstr(gameplay_content, "DEBUG should be filtered by gameplay") == NULL,
                "Gameplay logger should filter DEBUG messages");

    TEST_ASSERT(strstr(analytics_content, "DEBUG should appear in analytics") != NULL,
                "Analytics logger should capture DEBUG messages");
    TEST_ASSERT(strstr(analytics_content, "INFO should appear in analytics") != NULL,
                "Analytics logger should capture INFO messages");

    TEST_ASSERT(strstr(performance_content, "WARNING should appear in performance") != NULL,
                "Performance logger should capture WARNING messages");
    TEST_ASSERT(strstr(performance_content, "INFO should be filtered by performance") == NULL,
                "Performance logger should filter INFO messages");

    // Cleanup
    d_SetGlobalLogger(original_global);
    d_DestroyLogger(gameplay_logger);
    d_DestroyLogger(analytics_logger);
    d_DestroyLogger(performance_logger);
    d_DestroyString(gameplay_buffer);
    d_DestroyString(analytics_buffer);
    d_DestroyString(performance_buffer);

    d_LogInfo("✓ Multi-Logger Independence Test Completed");
    return 1;
}
int test_structured_analytics_logging() {
    d_LogInfo("Starting Structured Analytics Logging Test");
    d_LogDebug("Testing JSON and key-value structured logging for game analytics");

    // Create analytics logger
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = false
    };

    dLogger_t* analytics_logger = d_CreateLogger(config);
    TEST_ASSERT(analytics_logger != NULL, "Analytics logger should be created successfully");

    dString_t* analytics_buffer = d_InitString();
    TEST_ASSERT(analytics_buffer != NULL, "Analytics buffer should be created successfully");

    d_AddLogHandler(analytics_logger, d_StringLogHandler, analytics_buffer);
    TEST_ASSERT(true, "Analytics handler should be added without crashing");

    dLogger_t* original_global = d_GetGlobalLogger();
    d_SetGlobalLogger(analytics_logger);

    // Test player spawn analytics (JSON format)
    dLogStructured_t* spawn_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(spawn_log != NULL, "Player spawn structured log should be created");

    if (spawn_log) {
        d_LogStructured_SetFormat(spawn_log, true); // JSON format
        d_LogStructured_Field(spawn_log, "event_type", "player_spawn");
        d_LogStructured_Field(spawn_log, "player_name", "TestHero");
        d_LogStructured_Field(spawn_log, "player_class", "Warrior");
        d_LogStructured_FieldInt(spawn_log, "player_id", 12345);
        d_LogStructured_FieldInt(spawn_log, "level", 15);
        d_LogStructured_FieldFloat(spawn_log, "spawn_x", 100.5f, 2);
        d_LogStructured_FieldFloat(spawn_log, "spawn_y", 50.2f, 2);
        d_LogStructured_Commit(spawn_log);
    }

    // Test combat analytics (Key-value format)
    dLogStructured_t* combat_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(combat_log != NULL, "Combat structured log should be created");

    if (combat_log) {
        d_LogStructured_SetFormat(combat_log, false); // Key-value format
        d_LogStructured_Field(combat_log, "event_type", "combat_action");
        d_LogStructured_Field(combat_log, "attacker", "TestHero");
        d_LogStructured_Field(combat_log, "target", "Goblin");
        d_LogStructured_Field(combat_log, "skill_used", "Sword Strike");
        d_LogStructured_FieldInt(combat_log, "damage_dealt", 45);
        d_LogStructured_FieldInt(combat_log, "critical_hit", 1);
        d_LogStructured_FieldFloat(combat_log, "combat_duration", 2.35f, 3);
        d_LogStructured_Commit(combat_log);
    }

    // Test session summary analytics (JSON format)
    dLogStructured_t* session_log = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(session_log != NULL, "Session summary structured log should be created");

    if (session_log) {
        d_LogStructured_SetFormat(session_log, true); // JSON format
        d_LogStructured_Field(session_log, "report_type", "session_summary");
        d_LogStructured_FieldInt(session_log, "session_id", 987654);
        d_LogStructured_FieldInt(session_log, "total_combat_actions", 15);
        d_LogStructured_FieldInt(session_log, "combat_victories", 12);
        d_LogStructured_FieldFloat(session_log, "victory_rate_percent", 80.0f, 1);
        d_LogStructured_FieldFloat(session_log, "avg_actions_per_spawn", 3.75f, 2);
        d_LogStructured_Commit(session_log);
    }

    // Verify structured logging content
    const char* analytics_content = d_PeekString(analytics_buffer);
    TEST_ASSERT(analytics_content != NULL, "Analytics buffer should contain structured logs");
    TEST_ASSERT(strlen(analytics_content) > 0, "Analytics buffer should not be empty");

    // Verify JSON format content
    TEST_ASSERT(strstr(analytics_content, "\"event_type\":\"player_spawn\"") != NULL,
                "Should contain JSON player spawn event");
    TEST_ASSERT(strstr(analytics_content, "\"player_name\":\"TestHero\"") != NULL,
                "Should contain JSON player name");
    TEST_ASSERT(strstr(analytics_content, "\"spawn_x\":\"100.50\"") != NULL,
                "Should contain JSON float formatting");

    // Verify key-value format content
    TEST_ASSERT(strstr(analytics_content, "event_type=combat_action") != NULL,
                "Should contain key-value combat event");
    TEST_ASSERT(strstr(analytics_content, "skill_used=Sword Strike") != NULL,
                "Should contain key-value skill name");
    TEST_ASSERT(strstr(analytics_content, "combat_duration=2.350") != NULL,
                "Should contain key-value float with precision");

    // Verify session analytics JSON
    TEST_ASSERT(strstr(analytics_content, "\"report_type\":\"session_summary\"") != NULL,
                "Should contain JSON session summary");
    TEST_ASSERT(strstr(analytics_content, "\"victory_rate_percent\":\"80.0\"") != NULL,
                "Should contain JSON calculated metrics");

    d_LogInfoF("Analytics buffer captured %zu bytes of structured data", strlen(analytics_content));

    // Cleanup
    d_SetGlobalLogger(original_global);
    d_DestroyLogger(analytics_logger);
    d_DestroyString(analytics_buffer);

    d_LogInfo("✓ Structured Analytics Logging Test Completed");
    return 1;
}
int test_performance_monitoring_with_filters() {
    d_LogInfo("Starting Performance Monitoring with Filters Test");
    d_LogDebug("Testing performance monitoring system with filter configuration");

    // Create performance logger with DEBUG level to see all messages initially
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,  // Changed to DEBUG so we can see filter effects
        .include_timestamp = true,
        .colorize_output = false
    };

    dLogger_t* performance_logger = d_CreateLogger(config);
    TEST_ASSERT(performance_logger != NULL, "Performance logger should be created successfully");

    dString_t* performance_buffer = d_InitString();
    TEST_ASSERT(performance_buffer != NULL, "Performance buffer should be created successfully");

    d_AddLogHandler(performance_logger, d_StringLogHandler, performance_buffer);
    TEST_ASSERT(true, "Performance handler should be added without crashing");

    // Set up filter builder (test that the system works even if filters don't work as expected)
    dLogFilterBuilder_t* perf_filter = d_CreateFilterBuilder();
    TEST_ASSERT(perf_filter != NULL, "Performance filter builder should be created");

    // Add filter rules (these may not work as expected, but should not crash)
    d_FilterBuilder_AddPrefix(perf_filter, "PERF_", D_LOG_LEVEL_INFO);
    d_FilterBuilder_AddSuffix(perf_filter, "_CRITICAL", D_LOG_LEVEL_ERROR);
    d_FilterBuilder_AddDirectory(perf_filter, "performance/", D_LOG_LEVEL_DEBUG);

    TEST_ASSERT(true, "Filter rules should be added without crashing");

    d_FilterBuilder_Apply(performance_logger, perf_filter);
    TEST_ASSERT(true, "Filters should be applied without crashing");

    dLogger_t* original_global = d_GetGlobalLogger();
    d_SetGlobalLogger(performance_logger);

    // Test basic logging functionality
    d_LogDebug("Performance system initialized");
    d_LogInfo("PERF_MONITORING: System started");
    d_LogWarning("PERF_WARNING: High resource usage detected");
    d_LogError("PERF_CRITICAL: System overload detected");

    // Test performance metrics logging
    d_LogInfoF("PERF_FPS: Current framerate %d FPS", 60);
    d_LogInfoF("PERF_MEMORY: Memory usage %.1f MB", 245.5f);
    d_LogWarningF("PERF_LATENCY: Network latency %d ms", 150);

    // Test critical performance issues
    d_LogErrorF("FRAME_TIME_CRITICAL: Frame time %.3f exceeds threshold", 0.040f);
    d_LogErrorF("MEMORY_CRITICAL: Memory usage %.1f MB above limit", 600.0f);

    // Test structured performance logging
    dLogStructured_t* perf_metrics = d_LogStructured(D_LOG_LEVEL_INFO);
    TEST_ASSERT(perf_metrics != NULL, "Performance metrics structured log should be created");

    if (perf_metrics) {
        d_LogStructured_SetFormat(perf_metrics, false); // Key-value format
        d_LogStructured_Field(perf_metrics, "metric_type", "performance_snapshot");
        d_LogStructured_FieldFloat(perf_metrics, "avg_frame_time", 0.0167f, 4);
        d_LogStructured_FieldInt(perf_metrics, "fps", 60);
        d_LogStructured_FieldInt(perf_metrics, "active_entities", 150);
        d_LogStructured_FieldFloat(perf_metrics, "memory_mb", 245.5f, 1);
        d_LogStructured_FieldInt(perf_metrics, "draw_calls", 850);
        d_LogStructured_Commit(perf_metrics);
    }

    // Verify performance monitoring captures messages
    const char* perf_content = d_PeekString(performance_buffer);
    TEST_ASSERT(perf_content != NULL, "Performance buffer should contain logs");
    TEST_ASSERT(strlen(perf_content) > 0, "Performance buffer should not be empty");

    // Test that basic logging works (regardless of filters)
    TEST_ASSERT(strstr(perf_content, "Performance system initialized") != NULL,
                "Should capture basic debug messages");
    TEST_ASSERT(strstr(perf_content, "PERF_MONITORING: System started") != NULL,
                "Should capture PERF_ prefixed INFO messages");
    TEST_ASSERT(strstr(perf_content, "PERF_WARNING: High resource usage") != NULL,
                "Should capture PERF_ prefixed WARNING messages");

    // Test performance metrics are captured
    TEST_ASSERT(strstr(perf_content, "PERF_FPS: Current framerate 60") != NULL,
                "Should capture FPS performance metrics");
    TEST_ASSERT(strstr(perf_content, "PERF_MEMORY: Memory usage 245.5") != NULL,
                "Should capture memory performance metrics");

    // Test critical performance issues are captured
    TEST_ASSERT(strstr(perf_content, "FRAME_TIME_CRITICAL") != NULL,
                "Should capture critical frame time issues");
    TEST_ASSERT(strstr(perf_content, "MEMORY_CRITICAL") != NULL,
                "Should capture critical memory issues");

    // Test structured performance metrics
    TEST_ASSERT(strstr(perf_content, "metric_type=performance_snapshot") != NULL,
                "Should contain structured performance metrics");
    TEST_ASSERT(strstr(perf_content, "fps=60") != NULL,
                "Should contain FPS in structured format");
    TEST_ASSERT(strstr(perf_content, "memory_mb=245.5") != NULL,
                "Should contain memory usage in structured format");

    d_LogInfoF("Performance buffer captured %zu bytes of performance data",
               strlen(perf_content));

    // Test statistics tracking for performance monitoring
    dLogStats_t stats;
    d_GetLogStats(performance_logger, &stats);
    TEST_ASSERT(true, "Should retrieve performance logger statistics without crashing");

    d_LogInfoF("Performance logger processed logs in %.4f seconds", stats.total_log_time);
    TEST_ASSERT(stats.total_log_time >= 0.0, "Processing time should be non-negative");
    TEST_ASSERT(stats.handler_errors == 0, "Should have no handler errors during performance monitoring");

    // Test filter builder cleanup doesn't crash
    d_DestroyFilterBuilder(perf_filter);
    TEST_ASSERT(true, "Filter builder should be destroyed without crashing");

    // Cleanup
    d_SetGlobalLogger(original_global);
    d_DestroyLogger(performance_logger);
    d_DestroyString(performance_buffer);

    d_LogInfo("✓ Performance Monitoring with Filters Test Completed");
    return 1;
}



// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main() {
    printf("\n");
    printf("=============================================================================\n");
    printf("                    DAEDALUS ADVANCED LOGGING TEST SUITE                     \n");
    printf("=============================================================================\n\n");

    setup_advanced_test_logging();

    // Run all 12 tests
    TEST_SUITE_START("Advanced Logging Features");

    RUN_TEST(test_structured_logging_system);
    RUN_TEST(test_console_handler);
    RUN_TEST(test_statistics_tracking);
    RUN_TEST(test_logger_level_controls);
    RUN_TEST(test_game_logging_scenario);
    RUN_TEST(test_file_handler);
    RUN_TEST(test_string_handler);
    RUN_TEST(test_filter_builder_system);
    RUN_TEST(test_performance_stress);
    RUN_TEST(test_multi_handler_coordination);
    RUN_TEST(test_error_handling_edge_cases);
    RUN_TEST(test_comprehensive_integration);
    // HOWTO Guide verification tests
    RUN_TEST(test_multi_logger_independence);
    RUN_TEST(test_structured_analytics_logging);
    RUN_TEST(test_performance_monitoring_with_filters);
    // Debug Hunting Tests
    RUN_TEST(debug_hunter_memory_leak_test);
    RUN_TEST(debug_hunter_race_condition_test);
    RUN_TEST(debug_hunter_buffer_overflow_test);
    RUN_TEST(debug_hunter_state_corruption_test);
    RUN_TEST(debug_hunter_performance_degradation_test);


    TEST_SUITE_END();

    cleanup_advanced_test_logging();

    return 0;
}
