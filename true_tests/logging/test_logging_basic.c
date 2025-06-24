// File: true_tests/logging/test_logging_basic.c - Tests for basic Daedalus logging functionality

#include "tests.h"
#include <limits.h>
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// TEST SETUP AND UTILITIES
// =============================================================================

// Custom log handler that captures output to a string for testing
static dString_t* captured_logs = NULL;

void test_capture_handler(const dLogEntry_t* entry, void* user_data)
{
    if (!captured_logs) {
        captured_logs = d_InitString();
    }

    // Format: [LEVEL] message
    d_AppendString(captured_logs, "[", 0);
    d_AppendString(captured_logs, d_LogLevel_ToString(entry->level), 0);
    d_AppendString(captured_logs, "] ", 0);

    if (entry->message) {
        d_AppendString(captured_logs, d_PeekString(entry->message), 0);
    }

    d_AppendString(captured_logs, "\n", 0);
}

void setup_test_logging()
{
    // Create a test logger with console output
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = false,  // Skip timestamps for easier testing
        .include_file_info = true,
        .include_function = false,
        .colorize_output = false
    };

    dLogger_t* logger = d_CreateLogger(config);
    if (logger) {
        // Add our capture handler for testing
        d_AddLogHandler(logger, test_capture_handler, NULL);
        d_SetGlobalLogger(logger);

        printf("Test logging setup complete\n");
    } else {
        printf("WARNING: Failed to create test logger\n");
    }
}

void cleanup_test_logging()
{
    dLogger_t* logger = d_GetGlobalLogger();
    if (logger) {
        d_DestroyLogger(logger);
        d_SetGlobalLogger(NULL);
    }

    if (captured_logs) {
        d_DestroyString(captured_logs);
        captured_logs = NULL;
    }
}

const char* get_captured_logs()
{
    return captured_logs ? d_PeekString(captured_logs) : "";
}

void clear_captured_logs()
{
    if (captured_logs) {
        d_ClearString(captured_logs);
    }
}

// =============================================================================
// BASIC LOGGING TESTS
// =============================================================================

int test_logger_creation_and_destruction(void)
{
    printf("Starting test: logger_creation_and_destruction\n");

    // Clear any residual captured logs
    clear_captured_logs();

    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .include_file_info = true
    };

    dLogger_t* logger = d_CreateLogger(config);
    TEST_ASSERT(logger != NULL, "Logger should be created successfully");
    TEST_ASSERT(logger->config.default_level == D_LOG_LEVEL_INFO, "Config should be preserved");
    TEST_ASSERT(logger->handlers != NULL, "Handlers array should be initialized");
    TEST_ASSERT(logger->contexts != NULL, "Contexts array should be initialized");

    d_DestroyLogger(logger);

    printf("Test logger_creation_and_destruction: PASSED\n");
    return 1;
}

int test_global_logger_management(void)
{
    printf("Starting test: global_logger_management\n");

    // Clear any residual captured logs
    clear_captured_logs();

    // Ensure clean state
    d_SetGlobalLogger(NULL);
    TEST_ASSERT(d_GetGlobalLogger() == NULL, "Should start with no global logger");

    dLogConfig_t config = {.default_level = D_LOG_LEVEL_DEBUG};
    dLogger_t* logger = d_CreateLogger(config);

    d_SetGlobalLogger(logger);
    TEST_ASSERT(d_GetGlobalLogger() == logger, "Global logger should be set");
    TEST_ASSERT(logger->is_global == true, "Logger should know it's global");

    d_SetGlobalLogger(NULL);
    TEST_ASSERT(d_GetGlobalLogger() == NULL, "Global logger should be cleared");

    d_DestroyLogger(logger);

    printf("Test global_logger_management: PASSED\n");
    return 1;
}

int test_log_level_utilities(void)
{
    printf("Starting test: log_level_utilities\n");

    // Clear any residual captured logs
    clear_captured_logs();

    // Test level to string conversion
    TEST_ASSERT(strcmp(d_LogLevel_ToString(D_LOG_LEVEL_DEBUG), "DEBUG") == 0, "DEBUG level string");
    TEST_ASSERT(strcmp(d_LogLevel_ToString(D_LOG_LEVEL_INFO), "INFO") == 0, "INFO level string");
    TEST_ASSERT(strcmp(d_LogLevel_ToString(D_LOG_LEVEL_WARNING), "WARNING") == 0, "WARNING level string");
    TEST_ASSERT(strcmp(d_LogLevel_ToString(D_LOG_LEVEL_ERROR), "ERROR") == 0, "ERROR level string");
    TEST_ASSERT(strcmp(d_LogLevel_ToString(D_LOG_LEVEL_FATAL), "FATAL") == 0, "FATAL level string");

    // Test string to level conversion
    TEST_ASSERT(d_LogLevel_FromString("DEBUG") == D_LOG_LEVEL_DEBUG, "Parse DEBUG");
    TEST_ASSERT(d_LogLevel_FromString("info") == D_LOG_LEVEL_INFO, "Parse lowercase info");
    TEST_ASSERT(d_LogLevel_FromString("W") == D_LOG_LEVEL_WARNING, "Parse single char W");
    TEST_ASSERT(d_LogLevel_FromString("e") == D_LOG_LEVEL_ERROR, "Parse lowercase e");
    TEST_ASSERT(d_LogLevel_FromString("invalid") == D_LOG_LEVEL_INFO, "Invalid defaults to INFO");
    TEST_ASSERT(d_LogLevel_FromString(NULL) == D_LOG_LEVEL_INFO, "NULL defaults to INFO");

    // Test color codes (basic check that they're not NULL)
    const char* debug_color = d_LogLevel_GetColor(D_LOG_LEVEL_DEBUG);
    const char* error_color = d_LogLevel_GetColor(D_LOG_LEVEL_ERROR);
    TEST_ASSERT(debug_color != NULL, "Debug color should not be NULL");
    TEST_ASSERT(error_color != NULL, "Error color should not be NULL");
    TEST_ASSERT(strcmp(debug_color, error_color) != 0, "Different levels should have different colors");

    printf("Test log_level_utilities: PASSED\n");
    return 1;
}

int test_timestamp_functionality(void)
{
    printf("Starting test: timestamp_functionality\n");

    // Clear any residual captured logs
    clear_captured_logs();

    double ts1 = d_GetTimestamp();
    // Small delay
    for (volatile int i = 0; i < 10000; i++);
    double ts2 = d_GetTimestamp();

    TEST_ASSERT(ts1 > 0, "Timestamp should be positive");
    TEST_ASSERT(ts2 > ts1, "Second timestamp should be later");
    TEST_ASSERT((ts2 - ts1) < 1.0, "Timestamps should be close in time");

    // Test timestamp formatting
    char buffer[64];
    d_FormatTimestamp(buffer, sizeof(buffer), ts1, NULL);
    TEST_ASSERT(strlen(buffer) > 0, "Formatted timestamp should not be empty");
    TEST_ASSERT(strstr(buffer, ":") != NULL, "Timestamp should contain time separator");

    printf("Formatted timestamp: %s\n", buffer);

    printf("Test timestamp_functionality: PASSED\n");
    return 1;
}

int test_basic_logging_without_global_logger(void)
{
    printf("Starting test: basic_logging_without_global_logger\n");

    // Clear any residual captured logs
    clear_captured_logs();

    // Ensure no global logger
    d_SetGlobalLogger(NULL);

    // These should not crash
    d_Log(D_LOG_LEVEL_INFO, "Test message");
    d_LogF(D_LOG_LEVEL_ERROR, "Test %s %d", "formatted", 42);
    d_LogInfo("Info message");
    d_LogErrorF("Error %d", 123);

    printf("Test basic_logging_without_global_logger: PASSED - No crashes\n");
    return 1;
}

int test_basic_logging_with_global_logger(void)
{
    printf("Starting test: basic_logging_with_global_logger\n");

    setup_test_logging();
    clear_captured_logs();

    // Test simple logging
    d_LogInfo("Simple info message");
    d_LogWarning("Warning message");
    d_LogError("Error message");

    const char* logs = get_captured_logs();
    printf("Captured logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "[INFO] Simple info message") != NULL, "Info message should be captured");
    TEST_ASSERT(strstr(logs, "[WARNING] Warning message") != NULL, "Warning message should be captured");
    TEST_ASSERT(strstr(logs, "[ERROR] Error message") != NULL, "Error message should be captured");

    cleanup_test_logging();

    printf("Test basic_logging_with_global_logger: PASSED\n");
    return 1;
}

int test_formatted_logging(void)
{
    printf("Starting test: formatted_logging\n");

    setup_test_logging();
    clear_captured_logs();

    // Test formatted logging
    d_LogInfoF("Player %s has %d health", "Warrior", 85);
    d_LogErrorF("Failed to load file: %s (error %d)", "texture.png", 404);
    d_LogF(D_LOG_LEVEL_DEBUG, "Debug value: %f", 3.14159);

    const char* logs = get_captured_logs();
    printf("Captured formatted logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Player Warrior has 85 health") != NULL, "Player message should be formatted");
    TEST_ASSERT(strstr(logs, "texture.png (error 404)") != NULL, "Error message should be formatted");
    TEST_ASSERT(strstr(logs, "3.14159") != NULL, "Float should be formatted");

    cleanup_test_logging();

    printf("Test formatted_logging: PASSED\n");
    return 1;
}

int test_log_level_filtering(void)
{
    printf("Starting test: log_level_filtering\n");

    dLogConfig_t config = {.default_level = D_LOG_LEVEL_WARNING};
    dLogger_t* logger = d_CreateLogger(config);
    d_AddLogHandler(logger, test_capture_handler, NULL);
    d_SetGlobalLogger(logger);

    clear_captured_logs();

    // These should be filtered out (below WARNING level)
    d_LogDebug("Debug message - should be filtered");
    d_LogInfo("Info message - should be filtered");

    // These should get through
    d_LogWarning("Warning message - should appear");
    d_LogError("Error message - should appear");
    d_LogFatal("Fatal message - should appear");

    const char* logs = get_captured_logs();
    printf("Filtered logs (WARNING and above):\n%s", logs);

    TEST_ASSERT(strstr(logs, "Debug message") == NULL, "Debug should be filtered");
    TEST_ASSERT(strstr(logs, "Info message") == NULL, "Info should be filtered");
    TEST_ASSERT(strstr(logs, "Warning message") != NULL, "Warning should appear");
    TEST_ASSERT(strstr(logs, "Error message") != NULL, "Error should appear");
    TEST_ASSERT(strstr(logs, "Fatal message") != NULL, "Fatal should appear");

    cleanup_test_logging();

    printf("Test log_level_filtering: PASSED\n");
    return 1;
}

int test_logging_with_source_info(void)
{
    printf("Starting test: logging_with_source_info\n");

    setup_test_logging();
    clear_captured_logs();

    // Test logging with explicit source info
    d_LogEx(D_LOG_LEVEL_INFO, "test_file.c", 42, "test_function", "Message with source info");

    const char* logs = get_captured_logs();
    printf("Logs with source info:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Message with source info") != NULL, "Message should be captured");
    // Note: The current capture handler doesn't format source info, but it's passed to the entry

    cleanup_test_logging();

    printf("Test logging_with_source_info: PASSED\n");
    return 1;
}

int test_performance_when_disabled(void)
{
    printf("Starting test: performance_when_disabled\n");

    // Clear any residual captured logs
    clear_captured_logs();

    // Disable logging globally
    d_SetLoggingEnabled(false);

    // These should have minimal overhead
    for (int i = 0; i < 1000; i++) {
        d_LogInfo("This message should be efficiently ignored");
        d_LogErrorF("Error %d should be ignored", i);
    }

    // Re-enable for other tests
    d_SetLoggingEnabled(true);

    printf("Test performance_when_disabled: PASSED - No crashes, efficient execution\n");
    return 1;
}

// =============================================================================
// INTEGRATION TESTS (Like your RPG examples)
// =============================================================================

int test_rpg_combat_logging(void)
{
    printf("Starting test: rpg_combat_logging\n");

    setup_test_logging();
    clear_captured_logs();

    // Simulate RPG combat with cleaner logging
    const char* attacker = "Warrior";
    const char* defender = "Goblin";
    int damage = 25;
    int remaining_health = 15;

    d_LogInfoF("Combat started: %s vs %s", attacker, defender);
    d_LogInfoF("%s attacks %s for %d damage", attacker, defender, damage);
    d_LogWarningF("%s health critical: %d remaining", defender, remaining_health);

    if (remaining_health <= 0) {
        d_LogInfoF("%s defeated!", defender);
    }

    const char* logs = get_captured_logs();
    printf("RPG Combat Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Combat started: Warrior vs Goblin") != NULL, "Combat start logged");
    TEST_ASSERT(strstr(logs, "attacks Goblin for 25 damage") != NULL, "Attack logged");
    TEST_ASSERT(strstr(logs, "health critical: 15") != NULL, "Health warning logged");

    cleanup_test_logging();

    printf("Test rpg_combat_logging: PASSED\n");
    return 1;
}

int test_game_state_logging(void)
{
    printf("Starting test: game_state_logging\n");

    setup_test_logging();
    clear_captured_logs();

    // Instead of the old verbose way:
    // dString_t* log_message = d_InitString();
    // d_AppendString(log_message, "Player level: ", 0);
    // d_AppendInt(log_message, level);
    // d_AppendString(log_message, ", XP: ", 0);
    // d_AppendInt(log_message, xp);
    // LOG(log_message->str);
    // d_DestroyString(log_message);

    // Now we can do this:
    int level = 15;
    int xp = 2450;
    float health_percent = 75.5f;

    d_LogInfoF("Player level: %d, XP: %d", level, xp);
    d_LogDebugF("Health: %.1f%%", health_percent);

    // Test boolean logging too
    bool has_magic = true;
    bool is_poisoned = false;

    d_LogInfoF("Player state: Magic=%s, Poisoned=%s",
               has_magic ? "Yes" : "No",
               is_poisoned ? "Yes" : "No");

    const char* logs = get_captured_logs();
    printf("Game State Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Player level: 15, XP: 2450") != NULL, "Player stats logged");
    TEST_ASSERT(strstr(logs, "Health: 75.5%") != NULL, "Health logged");
    TEST_ASSERT(strstr(logs, "Magic=Yes, Poisoned=No") != NULL, "Boolean states logged");

    cleanup_test_logging();

    printf("Test game_state_logging: PASSED\n");
    return 1;
}
// =============================================================================
// ADVANCED BUG HUNTING TESTS
// =============================================================================

int test_null_and_empty_string_handling(void)
{
    printf("Starting test: null_and_empty_string_handling\n");

    setup_test_logging();
    clear_captured_logs();

    // Test NULL format string handling
    d_LogInfoF(NULL);  // Should not crash

    // Test empty format string
    d_LogInfoF("");

    // Test NULL arguments in format
    d_LogInfoF("Player: %s, Score: %d", NULL, 42);

    // Test empty string arguments
    d_LogInfoF("Empty string test: '%s'", "");

    // Test format string with no arguments needed
    d_LogInfoF("Simple message with no formatting");

    const char* logs = get_captured_logs();
    printf("Null/Empty String Logs:\n%s", logs);

    // Should have some logs (at least the ones that are valid)
    TEST_ASSERT(strlen(logs) > 0, "Should have captured some valid logs");
    TEST_ASSERT(strstr(logs, "Simple message with no formatting") != NULL,
                "Simple message should be logged");

    cleanup_test_logging();
    printf("Test null_and_empty_string_handling: PASSED\n");
    return 1;
}

int test_extreme_format_strings(void)
{
    printf("Starting test: extreme_format_strings\n");

    setup_test_logging();
    clear_captured_logs();

    // Test very long format strings
    char long_format[1000];
    for (int i = 0; i < 990; i++) {
        long_format[i] = 'A';
    }
    strcpy(long_format + 990, " END");
    d_LogInfoF("%s", long_format);

    // Test many format specifiers
    d_LogInfoF("Multi-format: %d %s %f %c %x %o",
               42, "test", 3.14f, 'X', 255, 64);

    // Test precision specifiers
    d_LogInfoF("Precision: %.2f %.0f %10s %-10s",
               3.14159f, 2.71828f, "right", "left");

    // Test edge case numbers
    d_LogInfoF("Edge numbers: %d %d %f %f",
               INT_MAX, INT_MIN, 0.0f, -0.0f);

    const char* logs = get_captured_logs();
    printf("Extreme Format Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Multi-format: 42 test") != NULL,
                "Multi-format should be logged");
    TEST_ASSERT(strstr(logs, "Precision: 3.14") != NULL,
                "Precision formatting should work");

    cleanup_test_logging();
    printf("Test extreme_format_strings: PASSED\n");
    return 1;
}

int test_concurrent_logging_stress(void)
{
    printf("Starting test: concurrent_logging_stress\n");

    setup_test_logging();
    clear_captured_logs();

    // Simulate rapid logging that might stress the TLS buffer system
    for (int i = 0; i < 100; i++) {
        d_LogInfoF("Rapid log %d", i);
        d_LogWarningF("Warning %d with data: %s", i, "test_data");
        d_LogErrorF("Error batch %d: code=%d, msg=%s", i, i*10, "error_msg");

        // Mix formatted and non-formatted
        d_LogInfo("Non-formatted info");
        d_LogDebugF("Debug %d", i);
    }

    const char* logs = get_captured_logs();
    printf("Concurrent Stress Test completed, log count: %zu chars\n", strlen(logs));

    // Should have many logs
    TEST_ASSERT(strlen(logs) > 1000, "Should have substantial log output");
    TEST_ASSERT(strstr(logs, "Rapid log 0") != NULL, "Should have first log");
    TEST_ASSERT(strstr(logs, "Rapid log 99") != NULL, "Should have last log");
    TEST_ASSERT(strstr(logs, "Error batch 50") != NULL, "Should have middle logs");

    cleanup_test_logging();
    printf("Test concurrent_logging_stress: PASSED\n");
    return 1;
}

int test_logging_with_special_characters(void)
{
    printf("Starting test: logging_with_special_characters\n");

    setup_test_logging();
    clear_captured_logs();

    // Test various special characters and escape sequences
    d_LogInfoF("Special chars: \\n \\t \\r \\\\ \\\"");
    d_LogWarningF("Unicode-ish: café naïve résumé");  // Extended ASCII
    d_LogErrorF("Symbols: !@#$%%^&*()_+-=[]{}|;':\",./<>?");
    d_LogDebugF("Numbers: %d %f %e %g", 12345, 3.14159, 1.23e-4, 9.87654);

    // Test newlines and tabs in format
    d_LogInfoF("Multi-line\nmessage\twith\ttabs");

    // Test percent signs (tricky for printf)
    d_LogInfoF("Progress: 50%% complete, efficiency: 95%%");

    const char* logs = get_captured_logs();
    printf("Special Characters Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Special chars:") != NULL, "Special chars should be logged");
    TEST_ASSERT(strstr(logs, "50% complete") != NULL, "Percent signs should be handled");
    TEST_ASSERT(strstr(logs, "Multi-line") != NULL, "Multi-line should be logged");

    cleanup_test_logging();
    printf("Test logging_with_special_characters: PASSED\n");
    return 1;
}

int test_memory_and_cleanup_edge_cases(void)
{
    printf("Starting test: memory_and_cleanup_edge_cases\n");

    // Clear any residual captured logs
    clear_captured_logs();

    // Test multiple logger creation/destruction cycles
    for (int cycle = 0; cycle < 5; cycle++) {
        dLogConfig_t config = {.default_level = D_LOG_LEVEL_DEBUG};
        dLogger_t* logger = d_CreateLogger(config);
        TEST_ASSERT(logger != NULL, "Logger should be created in each cycle");

        d_SetGlobalLogger(logger);

        // Do some logging
        d_LogInfoF("Cycle %d test message", cycle);
        d_LogWarningF("Cycle %d warning with data: %s %d", cycle, "test", cycle * 10);

        // Cleanup
        d_SetGlobalLogger(NULL);
        d_DestroyLogger(logger);
    }

    // Test rapid enable/disable cycles
    for (int i = 0; i < 20; i++) {
        d_SetLoggingEnabled(i % 2 == 0);  // Alternate enable/disable
        d_LogInfoF("Toggle test %d", i);
    }
    d_SetLoggingEnabled(true);  // Re-enable for other tests

    // Test logger with no handlers
    dLogConfig_t config = {.default_level = D_LOG_LEVEL_INFO};
    dLogger_t* empty_logger = d_CreateLogger(config);
    d_SetGlobalLogger(empty_logger);

    // Should not crash even with no handlers
    d_LogInfoF("Message to logger with no handlers");
    d_LogErrorF("Error with no handlers: %d", 404);

    d_DestroyLogger(empty_logger);
    d_SetGlobalLogger(NULL);

    printf("Test memory_and_cleanup_edge_cases: PASSED\n");
    return 1;
}

// =============================================================================
// HOW-TO GUIDE VALIDATION TESTS
// =============================================================================

int test_howto_player_action_logging(void)
{
    printf("Starting test: howto_player_action_logging\n");

    setup_test_logging();
    clear_captured_logs();

    // Simulate the Player struct and functions from the how-to guide
    typedef struct {
        char name[32];
        int level;
        int health;
        int max_health;
        float x, y;
    } TestPlayer;

    TestPlayer player = {"Hero", 5, 25, 100, 10.5f, 7.2f};

    // Test player position logging (from guide)
    d_LogInfoF("Player '%s' moved to position (%.2f, %.2f)",
               player.name, player.x, player.y);

    // Test critical health logging (from guide)
    float health_percent = (float)player.health / player.max_health * 100.0f;
    if (health_percent <= 25.0f) {
        d_LogWarningF("Player '%s' health critical: %d/%d (%.1f%%)",
                      player.name, player.health, player.max_health, health_percent);
    }

    // Test level progression logging (from guide)
    d_LogInfoF("Player '%s' is now level %d", player.name, player.level);

    const char* logs = get_captured_logs();
    printf("How-to Player Action Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Player 'Hero' moved to position (10.50, 7.20)") != NULL,
                "Player movement should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Player 'Hero' health critical: 25/100 (25.0%)") != NULL,
                "Critical health should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Player 'Hero' is now level 5") != NULL,
                "Level progression should be logged as shown in guide");

    cleanup_test_logging();
    printf("Test howto_player_action_logging: PASSED\n");
    return 1;
}

int test_howto_combat_event_logging(void)
{
    printf("Starting test: howto_combat_event_logging\n");

    setup_test_logging();
    clear_captured_logs();

    // Simulate the combat logging from the how-to guide
    typedef struct {
        char name[32];
        int health;
    } TestEntity;

    TestEntity player = {"Hero", 90};
    TestEntity enemy = {"Goblin", 0};  // Dead enemy for overkill test

    int damage_dealt = 45;
    int damage_received = 10;

    // Test combat logging exactly as shown in the guide
    d_LogInfoF("Combat initiated: %s vs %s", player.name, enemy.name);

    if (damage_dealt > 0) {
        d_LogInfoF("%s attacks %s for %d damage",
                   player.name, enemy.name, damage_dealt);

        if (damage_dealt > enemy.health) {
            d_LogInfoF("%s defeated! Overkill: %d damage",
                       enemy.name, damage_dealt - enemy.health);
        }
    }

    if (damage_received > 0) {
        d_LogWarningF("%s takes %d damage from %s",
                      player.name, damage_received, enemy.name);
    }

    if (enemy.health <= 0) {
        d_LogInfoF("Combat victory: %s defeated %s", player.name, enemy.name);
    }

    const char* logs = get_captured_logs();
    printf("How-to Combat Event Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Combat initiated: Hero vs Goblin") != NULL,
                "Combat initiation should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Hero attacks Goblin for 45 damage") != NULL,
                "Attack damage should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Goblin defeated! Overkill: 45 damage") != NULL,
                "Overkill damage should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Hero takes 10 damage from Goblin") != NULL,
                "Damage received should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Combat victory: Hero defeated Goblin") != NULL,
                "Combat victory should be logged as shown in guide");

    cleanup_test_logging();
    printf("Test howto_combat_event_logging: PASSED\n");
    return 1;
}

int test_howto_game_systems_logging(void)
{
    printf("Starting test: howto_game_systems_logging\n");

    setup_test_logging();
    clear_captured_logs();

    // Test the various system logging examples from the guide

    // 1. Performance logging
    float delta_time = 0.016f;
    int fps = 60;
    int active_enemies = 5;
    int items_collected = 12;

    d_LogDebugF("Performance: FPS=%d, Delta=%.3fs, Enemies=%d, Items=%d",
                fps, delta_time, active_enemies, items_collected);

    // 2. Performance warning conditions
    int low_fps = 25;
    if (low_fps < 30) {
        d_LogWarningF("Low FPS detected: %d (target: 60)", low_fps);
    }

    int high_enemy_count = 150;
    if (high_enemy_count > 100) {
        d_LogWarningF("High enemy count may impact performance: %d active", high_enemy_count);
    }

    // 3. Game state transitions
    d_LogInfoF("Game state transition: %s → %s (reason: %s)",
               "MENU", "PLAYING", "player_start");

    // 4. Resource loading
    d_LogInfoF("Loaded %s: %s (%.3fs)", "texture", "player_sprite.png", 0.045f);
    d_LogErrorF("Failed to load %s: %s", "audio", "missing_sound.wav");

    // 5. Player level up (from main example)
    d_LogInfoF("Player leveled up! %s is now level %d with %d max health",
               "Hero", 2, 120);

    const char* logs = get_captured_logs();
    printf("How-to Game Systems Logs:\n%s", logs);

    TEST_ASSERT(strstr(logs, "Performance: FPS=60, Delta=0.016s, Enemies=5, Items=12") != NULL,
                "Performance logging should match guide format");
    TEST_ASSERT(strstr(logs, "Low FPS detected: 25 (target: 60)") != NULL,
                "Low FPS warning should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "High enemy count may impact performance: 150 active") != NULL,
                "High enemy count warning should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Game state transition: MENU → PLAYING (reason: player_start)") != NULL,
                "State transition should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Loaded texture: player_sprite.png (0.045s)") != NULL,
                "Successful resource loading should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Failed to load audio: missing_sound.wav") != NULL,
                "Failed resource loading should be logged as shown in guide");
    TEST_ASSERT(strstr(logs, "Player leveled up! Hero is now level 2 with 120 max health") != NULL,
                "Level up should be logged as shown in guide");

    cleanup_test_logging();
    printf("Test howto_game_systems_logging: PASSED\n");
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Daedalus Logging System - Basic Tests");

    printf("Testing basic Daedalus logging functionality\n");
    printf("===========================================\n");

    // Initialize global test state for bulk mode compatibility
    captured_logs = NULL;
    d_SetGlobalLogger(NULL);
    d_SetLoggingEnabled(true);

    // Core functionality tests
    RUN_TEST(test_logger_creation_and_destruction);
    RUN_TEST(test_global_logger_management);
    RUN_TEST(test_log_level_utilities);
    RUN_TEST(test_timestamp_functionality);

    // Basic logging tests
    RUN_TEST(test_basic_logging_without_global_logger);
    RUN_TEST(test_basic_logging_with_global_logger);
    RUN_TEST(test_formatted_logging);
    RUN_TEST(test_log_level_filtering);
    RUN_TEST(test_logging_with_source_info);
    RUN_TEST(test_performance_when_disabled);

    // Integration/usage tests
    RUN_TEST(test_rpg_combat_logging);
    RUN_TEST(test_game_state_logging);

    // Advanced bug hunting tests
    RUN_TEST(test_null_and_empty_string_handling);
    RUN_TEST(test_extreme_format_strings);
    RUN_TEST(test_concurrent_logging_stress);
    RUN_TEST(test_logging_with_special_characters);
    RUN_TEST(test_memory_and_cleanup_edge_cases);

    // How-to guide validation tests
    RUN_TEST(test_howto_player_action_logging);
    RUN_TEST(test_howto_combat_event_logging);
    RUN_TEST(test_howto_game_systems_logging);

    // Final cleanup to prevent global state issues in bulk test mode
    cleanup_test_logging();

    TEST_SUITE_END();
}
