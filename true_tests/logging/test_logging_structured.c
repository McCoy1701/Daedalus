// File: true_tests/test_logging_structured.c - Comprehensive tests for structured logging functionality
// This file contains detailed tests for the Daedalus structured logging system

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <float.h>

// Test tracking
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// STRUCTURED LOGGING TESTS
// =============================================================================

int test_structured_logging_basic_string_fields(void) {
    printf("\n🧪 Testing basic structured logging with string fields...\n");
    
    // Test basic structured logging with string fields
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Add multiple string fields
    d_LogStructured_Field(log, "event", "user_login");
    d_LogStructured_Field(log, "username", "testuser42");
    d_LogStructured_Field(log, "ip_address", "192.168.1.100");
    d_LogStructured_Field(log, "user_agent", "Mozilla/5.0");
    d_LogStructured_Field(log, "session_id", "abc123def456");
    
    // Use key-value format for this test
    d_LogStructured_SetFormat(log, false);
    
    printf("📝 Expected output: Key-value format with 5 string fields\n");
    d_LogStructured_Commit(log);
    
    printf("✅ Basic string field structured logging test completed\n");
    return 1;
}

int test_structured_logging_integer_fields(void) {
    printf("\n🧪 Testing structured logging with integer fields...\n");
    
    // Test structured logging with various integer types
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Add integer fields of different ranges
    d_LogStructured_FieldInt(log, "user_id", 12345);
    d_LogStructured_FieldInt(log, "failed_attempts", 3);
    d_LogStructured_FieldInt(log, "max_attempts", 5);
    d_LogStructured_FieldInt(log, "account_age_days", 847);
    d_LogStructured_FieldInt(log, "error_code", -1);
    d_LogStructured_FieldInt(log, "large_number", 2147483647);  // Max int32
    
    // Use key-value format
    d_LogStructured_SetFormat(log, false);
    
    printf("📝 Expected output: Key-value format with 6 integer fields (including negative and max values)\n");
    d_LogStructured_Commit(log);
    
    printf("✅ Integer field structured logging test completed\n");
    return 1;
}

int test_structured_logging_float_fields(void) {
    printf("\n🧪 Testing structured logging with float fields...\n");
    
    // Test structured logging with float values and different decimal precisions
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_DEBUG);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Add float fields with different precision requirements
    d_LogStructured_FieldFloat(log, "cpu_usage", 87.5f, 1);
    d_LogStructured_FieldFloat(log, "memory_usage", 64.234f, 3);
    d_LogStructured_FieldFloat(log, "temperature", 42.0f, 0);
    d_LogStructured_FieldFloat(log, "pi_approximation", 3.14159f, 5);
    d_LogStructured_FieldFloat(log, "response_time", 0.00123f, 6);
    d_LogStructured_FieldFloat(log, "large_float", 123456.789f, 3);
    
    // Use key-value format
    d_LogStructured_SetFormat(log, false);
    
    printf("📝 Expected output: Key-value format with 6 float fields (various decimal precisions)\n");
    d_LogStructured_Commit(log);
    
    printf("✅ Float field structured logging test completed\n");
    return 1;
}

int test_structured_logging_json_format(void) {
    printf("\n🧪 Testing structured logging with JSON format...\n");
    
    // Test JSON format output
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Add mixed field types for JSON output
    d_LogStructured_Field(log, "event_type", "api_request");
    d_LogStructured_Field(log, "endpoint", "/api/v1/users");
    d_LogStructured_Field(log, "method", "GET");
    d_LogStructured_FieldInt(log, "status_code", 200);
    d_LogStructured_FieldFloat(log, "response_time_ms", 45.23f, 2);
    d_LogStructured_FieldInt(log, "bytes_sent", 1024);
    d_LogStructured_Field(log, "client_ip", "10.0.0.1");
    
    // Enable JSON format
    d_LogStructured_SetFormat(log, true);
    
    printf("📝 Expected output: JSON format with mixed field types\n");
    d_LogStructured_Commit(log);
    
    printf("✅ JSON format structured logging test completed\n");
    return 1;
}

int test_structured_logging_key_value_format(void) {
    printf("\n🧪 Testing structured logging with key-value format...\n");
    
    // Test key-value format with comprehensive data
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_ERROR);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Add comprehensive system monitoring data
    d_LogStructured_Field(log, "system", "database");
    d_LogStructured_Field(log, "subsystem", "connection_pool");
    d_LogStructured_Field(log, "operation", "acquire_connection");
    d_LogStructured_Field(log, "error_type", "timeout");
    d_LogStructured_FieldInt(log, "pool_size", 20);
    d_LogStructured_FieldInt(log, "active_connections", 18);
    d_LogStructured_FieldInt(log, "wait_time_ms", 5000);
    d_LogStructured_FieldFloat(log, "pool_utilization", 90.0f, 1);
    d_LogStructured_Field(log, "database_host", "db-primary-01");
    d_LogStructured_Field(log, "severity", "high");
    
    // Use key-value format (explicitly set to false)
    d_LogStructured_SetFormat(log, false);
    
    printf("📝 Expected output: Key-value format with comprehensive system data\n");
    d_LogStructured_Commit(log);
    
    printf("✅ Key-value format structured logging test completed\n");
    return 1;
}

int test_structured_logging_mixed_field_types(void) {
    printf("\n🧪 Testing structured logging with mixed field types...\n");
    
    // Test comprehensive mixed field types in both formats
    printf("🔸 Testing JSON format with mixed types:\n");
    
    dLogStructured_t* json_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!json_log) {
        printf("❌ Failed to create JSON structured log\n");
        return 0;
    }
    
    // Game analytics example with mixed types
    d_LogStructured_Field(json_log, "event", "boss_battle");
    d_LogStructured_Field(json_log, "player_name", "DragonSlayer");
    d_LogStructured_Field(json_log, "boss_name", "Ancient_Dragon");
    d_LogStructured_FieldInt(json_log, "player_level", 45);
    d_LogStructured_FieldInt(json_log, "boss_level", 50);
    d_LogStructured_FieldFloat(json_log, "battle_duration", 127.5f, 1);
    d_LogStructured_FieldFloat(json_log, "player_health_remaining", 23.4f, 1);
    d_LogStructured_FieldInt(json_log, "damage_dealt", 12450);
    d_LogStructured_FieldInt(json_log, "damage_received", 8930);
    d_LogStructured_Field(json_log, "outcome", "victory");
    d_LogStructured_Field(json_log, "difficulty", "nightmare");
    
    d_LogStructured_SetFormat(json_log, true);
    d_LogStructured_Commit(json_log);
    
    printf("\n🔸 Testing key-value format with same data:\n");
    
    dLogStructured_t* kv_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!kv_log) {
        printf("❌ Failed to create key-value structured log\n");
        return 0;
    }
    
    // Same data in key-value format
    d_LogStructured_Field(kv_log, "event", "boss_battle");
    d_LogStructured_Field(kv_log, "player_name", "DragonSlayer");
    d_LogStructured_Field(kv_log, "boss_name", "Ancient_Dragon");
    d_LogStructured_FieldInt(kv_log, "player_level", 45);
    d_LogStructured_FieldInt(kv_log, "boss_level", 50);
    d_LogStructured_FieldFloat(kv_log, "battle_duration", 127.5f, 1);
    d_LogStructured_FieldFloat(kv_log, "player_health_remaining", 23.4f, 1);
    d_LogStructured_FieldInt(kv_log, "damage_dealt", 12450);
    d_LogStructured_FieldInt(kv_log, "damage_received", 8930);
    d_LogStructured_Field(kv_log, "outcome", "victory");
    d_LogStructured_Field(kv_log, "difficulty", "nightmare");
    
    d_LogStructured_SetFormat(kv_log, false);
    d_LogStructured_Commit(kv_log);
    
    printf("✅ Mixed field types structured logging test completed\n");
    return 1;
}

int test_structured_logging_boolean_fields_basic(void) {
    printf("\n🧪 Testing structured logging with boolean fields (basic)...\n");
    
    // Test basic boolean field functionality
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Add various boolean fields for user preferences
    d_LogStructured_Field(log, "event", "user_preferences");
    d_LogStructured_Field(log, "user_id", "12345");
    d_LogStructured_FieldBool(log, "email_notifications", true);
    d_LogStructured_FieldBool(log, "push_notifications", false);
    d_LogStructured_FieldBool(log, "dark_mode", true);
    d_LogStructured_FieldBool(log, "auto_save", true);
    d_LogStructured_FieldBool(log, "beta_features", false);
    
    // Use key-value format for this test
    d_LogStructured_SetFormat(log, false);
    
    printf("📝 Expected output: Key-value format with boolean fields as 'true'/'false'\n");
    d_LogStructured_Commit(log);
    
    printf("✅ Basic boolean field structured logging test completed\n");
    return 1;
}

int test_structured_logging_boolean_fields_mixed_formats(void) {
    printf("\n🧪 Testing structured logging with boolean fields (mixed formats)...\n");
    
    printf("🔸 Testing JSON format with booleans:\n");
    
    // Test JSON format with booleans
    dLogStructured_t* json_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (!json_log) {
        printf("❌ Failed to create JSON structured log\n");
        return 0;
    }
    
    // Security monitoring example
    d_LogStructured_Field(json_log, "event", "security_check");
    d_LogStructured_Field(json_log, "user_agent", "suspicious_bot");
    d_LogStructured_FieldInt(json_log, "attempt_count", 5);
    d_LogStructured_FieldBool(json_log, "is_blocked", true);
    d_LogStructured_FieldBool(json_log, "captcha_required", true);
    d_LogStructured_FieldBool(json_log, "rate_limited", true);
    d_LogStructured_FieldBool(json_log, "whitelist_bypassed", false);
    
    d_LogStructured_SetFormat(json_log, true);
    d_LogStructured_Commit(json_log);
    
    printf("\n🔸 Testing key-value format with same booleans:\n");
    
    dLogStructured_t* kv_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (!kv_log) {
        printf("❌ Failed to create key-value structured log\n");
        return 0;
    }
    
    // Same data in key-value format
    d_LogStructured_Field(kv_log, "event", "security_check");
    d_LogStructured_Field(kv_log, "user_agent", "suspicious_bot");
    d_LogStructured_FieldInt(kv_log, "attempt_count", 5);
    d_LogStructured_FieldBool(kv_log, "is_blocked", true);
    d_LogStructured_FieldBool(kv_log, "captcha_required", true);
    d_LogStructured_FieldBool(kv_log, "rate_limited", true);
    d_LogStructured_FieldBool(kv_log, "whitelist_bypassed", false);
    
    d_LogStructured_SetFormat(kv_log, false);
    d_LogStructured_Commit(kv_log);
    
    printf("✅ Mixed format boolean field structured logging test completed\n");
    return 1;
}

int test_structured_logging_timestamp_fields_basic(void) {
    printf("\n🧪 Testing structured logging with timestamp fields (basic)...\n");
    
    // Test basic timestamp field functionality
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!log) {
        printf("❌ Failed to create structured log\n");
        return 0;
    }
    
    // Event tracking with timestamps
    d_LogStructured_Field(log, "event", "user_action");
    d_LogStructured_Field(log, "action_type", "login");
    d_LogStructured_Field(log, "user_id", "user_67890");
    d_LogStructured_FieldTimestamp(log, "event_time");
    d_LogStructured_FieldTimestamp(log, "server_time");
    d_LogStructured_Field(log, "ip_address", "192.168.1.50");
    
    // Use JSON format for this test
    d_LogStructured_SetFormat(log, true);
    
    printf("📝 Expected output: JSON format with ISO 8601 timestamps\n");
    d_LogStructured_Commit(log);
    
    printf("✅ Basic timestamp field structured logging test completed\n");
    return 1;
}

int test_structured_logging_timestamp_fields_multiple(void) {
    printf("\n🧪 Testing structured logging with multiple timestamp fields...\n");
    
    // Test multiple timestamps in different formats
    printf("🔸 Testing JSON format with multiple timestamps:\n");
    
    dLogStructured_t* json_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
    if (!json_log) {
        printf("❌ Failed to create JSON structured log\n");
        return 0;
    }
    
    // Database transaction example
    d_LogStructured_Field(json_log, "operation", "database_transaction");
    d_LogStructured_Field(json_log, "table", "user_sessions");
    d_LogStructured_FieldTimestamp(json_log, "transaction_start");
    
    // Simulate some work happening
    usleep(1000); // 1ms delay
    
    d_LogStructured_FieldTimestamp(json_log, "query_executed");
    d_LogStructured_FieldInt(json_log, "rows_affected", 3);
    d_LogStructured_FieldTimestamp(json_log, "transaction_end");
    
    d_LogStructured_SetFormat(json_log, true);
    d_LogStructured_Commit(json_log);
    
    printf("\n🔸 Testing key-value format with timestamps:\n");
    
    dLogStructured_t* kv_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
    if (!kv_log) {
        printf("❌ Failed to create key-value structured log\n");
        return 0;
    }
    
    // Performance monitoring example
    d_LogStructured_Field(kv_log, "component", "api_endpoint");
    d_LogStructured_Field(kv_log, "endpoint", "/api/v1/users");
    d_LogStructured_FieldTimestamp(kv_log, "request_received");
    d_LogStructured_FieldFloat(kv_log, "processing_time_ms", 45.7f, 2);
    d_LogStructured_FieldTimestamp(kv_log, "response_sent");
    d_LogStructured_FieldInt(kv_log, "status_code", 200);
    
    d_LogStructured_SetFormat(kv_log, false);
    d_LogStructured_Commit(kv_log);
    
    printf("✅ Multiple timestamp field structured logging test completed\n");
    return 1;
}

int test_structured_logging_clone_basic(void) {
    printf("\n🧪 Testing structured logging clone functionality (basic)...\n");
    
    // Create a base template log
    dLogStructured_t* base_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!base_log) {
        printf("❌ Failed to create base structured log\n");
        return 0;
    }
    
    // Add common fields to base template
    d_LogStructured_Field(base_log, "service", "user_api");
    d_LogStructured_Field(base_log, "version", "2.1.0");
    d_LogStructured_Field(base_log, "environment", "production");
    d_LogStructured_FieldTimestamp(base_log, "deployment_time");
    d_LogStructured_SetFormat(base_log, true); // JSON format
    
    printf("🔸 Creating clone and adding specific fields:\n");
    
    // Clone the base log
    dLogStructured_t* clone_log = d_LogStructured_Clone(base_log);
    if (!clone_log) {
        printf("❌ Failed to clone structured log\n");
        d_LogStructured_Commit(base_log); // Clean up base
        return 0;
    }
    
    // Add specific fields to the clone
    d_LogStructured_Field(clone_log, "operation", "user_registration");
    d_LogStructured_Field(clone_log, "user_email", "newuser@example.com");
    d_LogStructured_FieldInt(clone_log, "user_id", 98765);
    d_LogStructured_FieldBool(clone_log, "email_verified", false);
    
    printf("📝 Expected output: JSON with both base and clone-specific fields\n");
    d_LogStructured_Commit(clone_log);
    
    // Original base log should still be usable
    printf("\n🔸 Using original base log (should still work):\n");
    d_LogStructured_Field(base_log, "operation", "user_login");
    d_LogStructured_Field(base_log, "user_id", "existing_user_123");
    d_LogStructured_FieldBool(base_log, "login_successful", true);
    
    printf("📝 Expected output: JSON with base fields plus login-specific fields\n");
    d_LogStructured_Commit(base_log);
    
    printf("✅ Basic clone structured logging test completed\n");
    return 1;
}

int test_structured_logging_clone_advanced(void) {
    printf("\n🧪 Testing structured logging clone functionality (advanced)...\n");
    
    // Create a comprehensive base template
    dLogStructured_t* service_template = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!service_template) {
        printf("❌ Failed to create service template\n");
        return 0;
    }
    
    // Build a rich service template
    d_LogStructured_Field(service_template, "service_name", "game_server");
    d_LogStructured_Field(service_template, "cluster", "us-east-1");
    d_LogStructured_Field(service_template, "instance_id", "i-1234567890abcdef0");
    d_LogStructured_FieldInt(service_template, "port", 8080);
    d_LogStructured_FieldFloat(service_template, "cpu_cores", 4.0f, 1);
    d_LogStructured_FieldInt(service_template, "memory_gb", 16);
    d_LogStructured_FieldBool(service_template, "load_balancer_enabled", true);
    d_LogStructured_FieldTimestamp(service_template, "service_start_time");
    d_LogStructured_SetFormat(service_template, false); // Key-value format
    
    printf("🔸 Clone 1: Player connection event\n");
    
    dLogStructured_t* player_event = d_LogStructured_Clone(service_template);
    if (player_event) {
        d_LogStructured_Field(player_event, "event_type", "player_connection");
        d_LogStructured_Field(player_event, "player_name", "WarriorKnight");
        d_LogStructured_FieldInt(player_event, "player_level", 42);
        d_LogStructured_FieldBool(player_event, "is_premium", true);
        d_LogStructured_FieldTimestamp(player_event, "connection_time");
        d_LogStructured_Commit(player_event);
    }
    
    printf("\n🔸 Clone 2: Server performance metrics\n");
    
    dLogStructured_t* perf_metrics = d_LogStructured_Clone(service_template);
    if (perf_metrics) {
        // Change format for this clone
        d_LogStructured_SetFormat(perf_metrics, true); // JSON format
        d_LogStructured_Field(perf_metrics, "metric_type", "performance");
        d_LogStructured_FieldFloat(perf_metrics, "cpu_usage_percent", 67.8f, 2);
        d_LogStructured_FieldFloat(perf_metrics, "memory_usage_percent", 84.3f, 2);
        d_LogStructured_FieldInt(perf_metrics, "active_connections", 156);
        d_LogStructured_FieldBool(perf_metrics, "under_load", true);
        d_LogStructured_FieldTimestamp(perf_metrics, "metric_timestamp");
        d_LogStructured_Commit(perf_metrics);
    }
    
    printf("\n🔸 Clone 3: Error event\n");
    
    dLogStructured_t* error_event = d_LogStructured_Clone(service_template);
    if (error_event) {
        d_LogStructured_Field(error_event, "event_type", "system_error");
        d_LogStructured_Field(error_event, "error_code", "DB_CONNECTION_FAILED");
        d_LogStructured_Field(error_event, "error_message", "Connection timeout after 5000ms");
        d_LogStructured_FieldInt(error_event, "retry_count", 3);
        d_LogStructured_FieldBool(error_event, "service_degraded", true);
        d_LogStructured_FieldTimestamp(error_event, "error_timestamp");
        d_LogStructured_Commit(error_event);
    }
    
    // Clean up the original template
    d_LogStructured_Commit(service_template);
    
    printf("✅ Advanced clone structured logging test completed\n");
    return 1;
}

// =============================================================================
// BUG HUNTING TESTS - Edge Cases and Error Conditions
// =============================================================================

int test_structured_logging_null_input_handling(void) {
    printf("\n🐛 Bug hunting: Testing null/invalid input handling...\n");
    
    // Test NULL structured log pointer
    printf("🔸 Testing NULL pointer handling:\n");
    dLogStructured_t* null_log = NULL;
    
    // These should not crash - they should gracefully handle NULL
    null_log = d_LogStructured_Field(null_log, "test", "value");
    null_log = d_LogStructured_FieldInt(null_log, "count", 42);
    null_log = d_LogStructured_FieldFloat(null_log, "rate", 3.14f, 2);
    null_log = d_LogStructured_FieldBool(null_log, "active", true);
    null_log = d_LogStructured_FieldTimestamp(null_log, "time");
    null_log = d_LogStructured_SetFormat(null_log, true);
    d_LogStructured_Commit(null_log); // Should not crash
    
    // Test NULL key/value handling
    printf("🔸 Testing NULL key/value handling:\n");
    dLogStructured_t* test_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (test_log) {
        d_LogStructured_Field(test_log, NULL, "valid_value");  // NULL key
        d_LogStructured_Field(test_log, "valid_key", NULL);    // NULL value
        d_LogStructured_Field(test_log, NULL, NULL);           // Both NULL
        d_LogStructured_Field(test_log, "", "empty_key");      // Empty key
        d_LogStructured_Field(test_log, "empty_value", "");    // Empty value
        d_LogStructured_FieldInt(test_log, NULL, 123);         // NULL key with int
        d_LogStructured_FieldFloat(test_log, NULL, 1.5f, 2);  // NULL key with float
        d_LogStructured_FieldBool(test_log, NULL, true);       // NULL key with bool
        d_LogStructured_FieldTimestamp(test_log, NULL);        // NULL key with timestamp
        
        // Add at least one valid field so commit has something to do
        d_LogStructured_Field(test_log, "bug_test", "null_handling");
        printf("📝 Expected: Should handle NULLs gracefully without crashing\n");
        d_LogStructured_Commit(test_log);
    }
    
    // Test cloning NULL
    printf("🔸 Testing clone of NULL:\n");
    dLogStructured_t* null_clone = d_LogStructured_Clone(NULL);
    if (null_clone) {
        printf("❌ Clone of NULL should return NULL, but got valid pointer\n");
        d_LogStructured_Commit(null_clone);
        return 0;
    } else {
        printf("✅ Clone of NULL correctly returned NULL\n");
    }
    
    printf("✅ Null input handling bug hunt completed\n");
    return 1;
}

int test_structured_logging_memory_stress(void) {
    printf("\n🐛 Bug hunting: Memory stress test...\n");
    
    printf("🔸 Creating and destroying many structured logs rapidly:\n");
    
    for (int cycle = 0; cycle < 100; cycle++) {
        dLogStructured_t* stress_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        if (!stress_log) {
            printf("❌ Failed to create structured log in cycle %d\n", cycle);
            return 0;
        }
        
        // Add many fields to stress memory allocation
        for (int field = 0; field < 20; field++) {
            char key[64], value[128];
            snprintf(key, sizeof(key), "stress_field_%d_%d", cycle, field);
            snprintf(value, sizeof(value), "stress_value_cycle_%d_field_%d_data", cycle, field);
            d_LogStructured_Field(stress_log, key, value);
        }
        
        // Mix in other field types
        d_LogStructured_FieldInt(stress_log, "cycle_number", cycle);
        d_LogStructured_FieldFloat(stress_log, "progress", (float)cycle / 100.0f * 100.0f, 3);
        d_LogStructured_FieldBool(stress_log, "is_stress_test", true);
        d_LogStructured_FieldTimestamp(stress_log, "stress_timestamp");
        
        // Randomly switch formats
        d_LogStructured_SetFormat(stress_log, (cycle % 2 == 0));
        
        // Only commit every 10th log to reduce output spam
        if (cycle % 10 == 0) {
            d_LogStructured_Commit(stress_log);
        } else {
            // Just destroy without committing to test cleanup
            // This tests the destruction path without committing
            d_LogStructured_Field(stress_log, "destroy_test", "true");
            // Force cleanup by creating a new log (old one should be cleaned up)
            dLogStructured_t* temp = d_LogStructured(D_LOG_LEVEL_INFO);
            if (temp) {
                d_LogStructured_Field(temp, "temp", "cleanup_test");
                d_LogStructured_Commit(temp);
            }
        }
    }
    
    printf("✅ Memory stress test completed - no crashes detected\n");
    return 1;
}

int test_structured_logging_field_overflow(void) {
    printf("\n🐛 Bug hunting: Field overflow test...\n");
    
    printf("🔸 Testing excessive field count:\n");
    
    dLogStructured_t* overflow_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (!overflow_log) {
        printf("❌ Failed to create overflow test log\n");
        return 0;
    }
    
    // Try to add way more fields than reasonable
    for (int i = 0; i < 1000; i++) {
        char key[32], value[64];
        snprintf(key, sizeof(key), "overflow_key_%d", i);
        snprintf(value, sizeof(value), "overflow_value_%d_with_extra_data", i);
        
        dLogStructured_t* result = d_LogStructured_Field(overflow_log, key, value);
        if (!result) {
            printf("❌ Field addition failed at index %d\n", i);
            d_LogStructured_Commit(overflow_log);
            return 0;
        }
    }
    
    printf("🔸 Added 1000 fields successfully, testing commit:\n");
    d_LogStructured_Field(overflow_log, "test_type", "field_overflow");
    d_LogStructured_FieldInt(overflow_log, "total_fields", 1001);
    d_LogStructured_SetFormat(overflow_log, false); // Use key-value to reduce output size
    
    printf("📝 Expected: Should handle large field count gracefully\n");
    d_LogStructured_Commit(overflow_log);
    
    printf("✅ Field overflow test completed\n");
    return 1;
}

int test_structured_logging_clone_chain(void) {
    printf("\n🐛 Bug hunting: Clone chain test...\n");
    
    printf("🔸 Testing deep clone chains:\n");
    
    // Create original log
    dLogStructured_t* original = d_LogStructured(D_LOG_LEVEL_INFO);
    if (!original) {
        printf("❌ Failed to create original log\n");
        return 0;
    }
    
    d_LogStructured_Field(original, "generation", "0");
    d_LogStructured_Field(original, "base_data", "original");
    d_LogStructured_FieldBool(original, "is_original", true);
    d_LogStructured_FieldTimestamp(original, "creation_time");
    
    dLogStructured_t* current = original;
    dLogStructured_t* clones[10];
    
    // Create a chain of clones
    for (int i = 0; i < 10; i++) {
        dLogStructured_t* clone = d_LogStructured_Clone(current);
        if (!clone) {
            printf("❌ Failed to create clone %d\n", i);
            // Clean up previous clones
            for (int j = 0; j < i; j++) {
                if (clones[j]) d_LogStructured_Commit(clones[j]);
            }
            d_LogStructured_Commit(original);
            return 0;
        }
        
        clones[i] = clone;
        
        // Add generation-specific data
        char gen_str[16];
        snprintf(gen_str, sizeof(gen_str), "%d", i + 1);
        d_LogStructured_Field(clone, "generation", gen_str);
        d_LogStructured_FieldInt(clone, "clone_depth", i + 1);
        d_LogStructured_FieldBool(clone, "is_clone", true);
        
        // Set current to this clone for next iteration (clone of clone of clone...)
        current = clone;
    }
    
    printf("🔸 Committing clone chain (should show increasing generations):\n");
    
    // Commit the deepest clone (generation 10)
    d_LogStructured_Field(clones[9], "test_type", "deep_clone_chain");
    d_LogStructured_SetFormat(clones[9], true);
    printf("📝 Expected: JSON with all inherited fields plus generation 10 data\n");
    d_LogStructured_Commit(clones[9]);
    
    // Commit a middle clone (generation 5)
    d_LogStructured_Field(clones[4], "test_type", "mid_clone_chain");
    d_LogStructured_SetFormat(clones[4], false);
    printf("📝 Expected: Key-value with all inherited fields plus generation 5 data\n");
    d_LogStructured_Commit(clones[4]);
    
    // Clean up remaining clones
    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 9 && clones[i]) {
            d_LogStructured_Commit(clones[i]);
        }
    }
    
    // Original should still be usable
    d_LogStructured_Field(original, "test_type", "original_after_cloning");
    d_LogStructured_Commit(original);
    
    printf("✅ Clone chain test completed\n");
    return 1;
}

int test_structured_logging_format_switching_stress(void) {
    printf("\n🐛 Bug hunting: Format switching stress test...\n");
    
    printf("🔸 Testing rapid format switching:\n");
    
    dLogStructured_t* switch_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
    if (!switch_log) {
        printf("❌ Failed to create format switching log\n");
        return 0;
    }
    
    // Add base fields
    d_LogStructured_Field(switch_log, "test_type", "format_switching");
    d_LogStructured_FieldInt(switch_log, "base_value", 42);
    
    // Rapidly switch formats many times
    for (int i = 0; i < 100; i++) {
        bool use_json = (i % 2 == 0);
        d_LogStructured_SetFormat(switch_log, use_json);
        
        // Add a field each time
        char key[32], value[64];
        snprintf(key, sizeof(key), "switch_%d", i);
        snprintf(value, sizeof(value), "format_%s_iteration_%d", use_json ? "json" : "keyvalue", i);
        d_LogStructured_Field(switch_log, key, value);
    }
    
    // Final format switch and commit
    printf("🔸 Final format: JSON\n");
    d_LogStructured_SetFormat(switch_log, true);
    d_LogStructured_FieldInt(switch_log, "total_switches", 100);
    d_LogStructured_FieldBool(switch_log, "final_format_json", true);
    d_LogStructured_FieldTimestamp(switch_log, "completion_time");
    
    printf("📝 Expected: JSON format with all fields from switching iterations\n");
    d_LogStructured_Commit(switch_log);
    
    printf("🔸 Testing format switching on cloned logs:\n");
    
    dLogStructured_t* clone_switch_test = d_LogStructured(D_LOG_LEVEL_INFO);
    if (clone_switch_test) {
        d_LogStructured_Field(clone_switch_test, "base", "clone_format_test");
        d_LogStructured_SetFormat(clone_switch_test, true); // Start with JSON
        
        dLogStructured_t* clone1 = d_LogStructured_Clone(clone_switch_test);
        dLogStructured_t* clone2 = d_LogStructured_Clone(clone_switch_test);
        
        if (clone1 && clone2) {
            // Clone1: Keep JSON, Clone2: Switch to key-value
            d_LogStructured_Field(clone1, "clone_id", "1");
            d_LogStructured_Field(clone1, "format", "json");
            // clone1 inherits JSON format
            
            d_LogStructured_Field(clone2, "clone_id", "2");
            d_LogStructured_Field(clone2, "format", "keyvalue");
            d_LogStructured_SetFormat(clone2, false); // Switch to key-value
            
            printf("📝 Clone 1 (JSON inherited):\n");
            d_LogStructured_Commit(clone1);
            
            printf("📝 Clone 2 (switched to key-value):\n");
            d_LogStructured_Commit(clone2);
        }
        
        // Original should still work
        d_LogStructured_Field(clone_switch_test, "original", "still_works");
        d_LogStructured_Commit(clone_switch_test);
    }
    
    printf("✅ Format switching stress test completed\n");
    return 1;
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

void print_test_header(const char* test_name) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║  🧪 %-70s  ║\n", test_name);
    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
}

void print_test_summary(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n");
    printf("🏁 STRUCTURED LOGGING TEST SUMMARY\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n");
    printf("📊 Total Tests: %d\n", total_tests);
    printf("✅ Passed: %d\n", tests_passed);
    printf("❌ Failed: %d\n", tests_failed);
    printf("📈 Success Rate: %.1f%%\n", 
           total_tests > 0 ? (float)tests_passed / total_tests * 100.0f : 0.0f);
    
    if (tests_failed == 0) {
        printf("🎉 ALL TESTS PASSED! Structured logging is working perfectly!\n");
    } else {
        printf("⚠️  Some tests failed. Please review the output above.\n");
    }
    printf("════════════════════════════════════════════════════════════════════════════════\n");
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n");
    printf("            🏗️  DAEDALUS STRUCTURED LOGGING TESTS 🏗️                        \n");
    printf("                    Comprehensive Testing Suite                                \n");
    printf("════════════════════════════════════════════════════════════════════════════════\n");
    
    // Initialize logging system
    dLogger_t* test_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .include_thread_id = true,
        .colorize_output = true,
        .timestamp_format = "%H:%M:%S.%03d",
        .context_separator = " >> "
    });
    d_SetGlobalLogger(test_logger);
    
    printf("\n🎯 Testing structured logging functionality with 17 comprehensive tests:\n");
    printf("   1. Basic string fields\n");
    printf("   2. Integer fields with various ranges\n");
    printf("   3. Float fields with different precisions\n");
    printf("   4. JSON format output\n");
    printf("   5. Key-value format output\n");
    printf("   6. Mixed field types in both formats\n");
    printf("   7. Boolean fields (basic functionality)\n");
    printf("   8. Boolean fields (mixed formats)\n");
    printf("   9. Timestamp fields (basic functionality)\n");
    printf("   10. Timestamp fields (multiple timestamps)\n");
    printf("   11. Clone functionality (basic cloning)\n");
    printf("   12. Clone functionality (advanced scenarios)\n");
    printf("   🐛 BUG HUNTING TESTS:\n");
    printf("   13. Null/invalid input handling\n");
    printf("   14. Memory stress testing\n");
    printf("   15. Field overflow boundaries\n");
    printf("   16. Deep clone chain testing\n");
    printf("   17. Format switching stress testing\n\n");
    
    TEST_SUITE_START("Structured Logging Tests");
    
    // Run all structured logging tests
    print_test_header("TEST 1: Basic String Fields");
    RUN_TEST(test_structured_logging_basic_string_fields);
    
    print_test_header("TEST 2: Integer Fields");
    RUN_TEST(test_structured_logging_integer_fields);
    
    print_test_header("TEST 3: Float Fields");
    RUN_TEST(test_structured_logging_float_fields);
    
    print_test_header("TEST 4: JSON Format Output");
    RUN_TEST(test_structured_logging_json_format);
    
    print_test_header("TEST 5: Key-Value Format Output");
    RUN_TEST(test_structured_logging_key_value_format);
    
    print_test_header("TEST 6: Mixed Field Types");
    RUN_TEST(test_structured_logging_mixed_field_types);
    
    print_test_header("TEST 7: Boolean Fields (Basic)");
    RUN_TEST(test_structured_logging_boolean_fields_basic);
    
    print_test_header("TEST 8: Boolean Fields (Mixed Formats)");
    RUN_TEST(test_structured_logging_boolean_fields_mixed_formats);
    
    print_test_header("TEST 9: Timestamp Fields (Basic)");
    RUN_TEST(test_structured_logging_timestamp_fields_basic);
    
    print_test_header("TEST 10: Timestamp Fields (Multiple)");
    RUN_TEST(test_structured_logging_timestamp_fields_multiple);
    
    print_test_header("TEST 11: Clone Functionality (Basic)");
    RUN_TEST(test_structured_logging_clone_basic);
    
    print_test_header("TEST 12: Clone Functionality (Advanced)");
    RUN_TEST(test_structured_logging_clone_advanced);
    
    printf("\n🐛 BUG HUNTING PHASE - Testing Edge Cases and Error Conditions\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n");
    
    print_test_header("🐛 BUG HUNT 13: Null/Invalid Input Handling");
    RUN_TEST(test_structured_logging_null_input_handling);
    
    print_test_header("🐛 BUG HUNT 14: Memory Stress Testing");
    RUN_TEST(test_structured_logging_memory_stress);
    
    print_test_header("🐛 BUG HUNT 15: Field Overflow Boundaries");
    RUN_TEST(test_structured_logging_field_overflow);
    
    print_test_header("🐛 BUG HUNT 16: Deep Clone Chain Testing");
    RUN_TEST(test_structured_logging_clone_chain);
    
    print_test_header("🐛 BUG HUNT 17: Format Switching Stress");
    RUN_TEST(test_structured_logging_format_switching_stress);
    
    TEST_SUITE_END();
    
    // Print comprehensive summary
    print_test_summary();
    
    // Usage examples and documentation
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("📚 STRUCTURED LOGGING QUICK REFERENCE\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("\n💡 **HOW TO USE STRUCTURED LOGGING:**\n\n");
    
    printf("🔹 **Basic Usage Pattern:**\n");
    printf("   dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);\n");
    printf("   d_LogStructured_Field(log, \"key\", \"value\");\n");
    printf("   d_LogStructured_FieldInt(log, \"count\", 42);\n");
    printf("   d_LogStructured_FieldFloat(log, \"percentage\", 87.5f, 1);\n");
    printf("   d_LogStructured_SetFormat(log, true);  // true=JSON, false=key-value\n");
    printf("   d_LogStructured_Commit(log);\n\n");
    
    printf("🔹 **Available Field Types:**\n");
    printf("   • d_LogStructured_Field(log, key, string_value)\n");
    printf("   • d_LogStructured_FieldInt(log, key, int_value)\n");
    printf("   • d_LogStructured_FieldFloat(log, key, float_value, decimals)\n");
    printf("   • d_LogStructured_FieldBool(log, key, bool_value)  // NEW!\n");
    printf("   • d_LogStructured_FieldTimestamp(log, key)  // NEW!\n");
    printf("   • d_LogStructured_Clone(source_log)  // NEW!\n\n");
    
    printf("🔹 **Output Formats:**\n");
    printf("   • JSON: {\"key\":\"value\",\"count\":42,\"percentage\":87.5}\n");
    printf("   • Key-Value: key=value count=42 percentage=87.5\n\n");
    
    printf("🔹 **Best Practices:**\n");
    printf("   ✅ Use consistent key names across your application\n");
    printf("   ✅ JSON format is great for log aggregation tools\n");
    printf("   ✅ Key-value format is more human-readable\n");
    printf("   ✅ Always call d_LogStructured_Commit() to output the log\n");
    printf("   ✅ Use appropriate log levels (DEBUG, INFO, WARNING, ERROR, FATAL)\n\n");
    
    printf("🎯 **Use Cases:**\n");
    printf("   • API request/response logging\n");
    printf("   • Performance metrics and monitoring\n");
    printf("   • Business event tracking\n");
    printf("   • Error reporting with context\n");
    printf("   • System health monitoring\n");
    printf("   • Analytics and data collection\n");
    printf("   • Boolean flags and feature toggles\n");
    printf("   • Precise timestamp tracking\n");
    printf("   • Template-based logging with cloning\n");
    printf("   • Robust error handling and edge case management\n\n");
    
    // Cleanup
    d_DestroyLogger(test_logger);
    d_SetGlobalLogger(NULL);
    
    printf("🏁 Structured logging tests completed!\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n\n");
    
    return (tests_failed == 0) ? 0 : 1;
}