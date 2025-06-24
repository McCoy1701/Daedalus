# Daedalus Logging System

A high-performance, feature-rich logging system for C applications. Supports multiple log levels, structured logging, rate limiting, and advanced filtering with beautiful colored output.

## Quick Start

```c
#include "Daedalus.h"

int main() {
    // Basic logging - works immediately
    d_LogInfo("Hello, world!");
    d_LogWarningF("Player health: %d/100", 25);
    d_LogError("Connection failed");

    return 0;
}
```

Output:
```
[12:34:56.789] [INFO]  {12345} Hello, world!
[12:34:56.790] [WARN]  {12345} Player health: 25/100
[12:34:56.791] [ERROR] {12345} Connection failed
```

## Example 1: File Logging Setup

```c
#include "Daedalus.h"

int main() {
    // Create custom logger
    dLogger_t* logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = true
    });

    // Add file handler
    FILE* log_file = fopen("game.log", "a");
    d_AddLogHandler(logger, d_FileLogHandler, log_file);
    d_AddLogHandler(logger, d_ConsoleLogHandler, NULL);

    d_SetGlobalLogger(logger);

    // Now logs go to both console and file
    d_LogInfo("Game started");
    d_LogDebugF("Loading level %d", 1);
    d_LogWarning("Low memory warning");

    // Cleanup
    d_DestroyLogger(logger);
    fclose(log_file);
    return 0;
}
```

## Example 2: Smart Filtering

```c
#include "Daedalus.h"

int main() {
    dLogger_t* logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .colorize_output = true
    });

    // Set up filters
    dLogFilterBuilder_t* filter = d_CreateFilterBuilder();
    d_FilterBuilder_AddDirectory(filter, "engine/", D_LOG_LEVEL_WARNING);  // Engine: warnings only
    d_FilterBuilder_AddDirectory(filter, "gameplay/", D_LOG_LEVEL_DEBUG);  // Gameplay: everything
    d_FilterBuilder_AddPrefix(filter, "PERF_", D_LOG_LEVEL_INFO);          // Performance logs
    d_FilterBuilder_Apply(logger, filter);
    d_DestroyFilterBuilder(filter);

    d_SetGlobalLogger(logger);

    // These logs will be filtered based on rules above
    d_LogDebug("Engine debug message");      // Filtered out (engine/ needs WARNING+)
    d_LogWarning("Engine warning");          // Shows (engine/ allows WARNING+)
    d_LogDebugF("PERF_FPS: %d", 60);        // Shows (PERF_ prefix allows INFO+)

    d_DestroyLogger(logger);
    return 0;
}
```

## Example 3: Rate Limiting

```c
#include "Daedalus.h"

void game_loop() {
    // Simulate game loop that could spam logs
    for (int frame = 0; frame < 1000; frame++) {
        // This could spam 1000 messages, but rate limiting controls it
        d_LogRateLimited(D_LOG_LEVEL_WARNING, 5, 2.0,
                        "High CPU usage detected");

        // Rate limited with formatting
        d_LogRateLimitedF(D_LOG_LEVEL_ERROR, 3, 1.0,
                         "Network error in frame %d", frame);

        // Regular conditional logging
        if (frame % 100 == 0) {
            d_LogInfoF("Game loop progress: %d/1000 frames", frame);
        }
    }
}

int main() {
    d_LogInfo("Starting rate limiting demo");
    game_loop();
    d_LogInfo("Demo completed - check how rate limiting prevented spam!");
    return 0;
}
```

## Example 4: Structured Logging

```c
#include "Daedalus.h"

void log_player_event(const char* player_name, int level, float health) {
    // JSON format structured logging
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(log, "event", "player_status");
    d_LogStructured_Field(log, "player_name", player_name);
    d_LogStructured_FieldInt(log, "level", level);
    d_LogStructured_FieldFloat(log, "health_percent", health, 1);
    d_LogStructured_FieldBool(log, "is_alive", health > 0);
    d_LogStructured_FieldTimestamp(log, "event_time");
    d_LogStructured_SetFormat(log, true);  // Enable JSON
    d_LogStructured_Commit(log);
    // Output: {"event":"player_status","player_name":"Hero","level":"5","health_percent":"85.0","is_alive":"true","event_time":"2024-01-15T12:00:33.024Z"}
}

void log_system_metrics(int cpu_usage, int memory_mb) {
    // Key-value format structured logging
    dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_DEBUG);
    d_LogStructured_Field(log, "metric_type", "system_performance");
    d_LogStructured_FieldInt(log, "cpu_usage_percent", cpu_usage);
    d_LogStructured_FieldInt(log, "memory_usage_mb", memory_mb);
    d_LogStructured_FieldBool(log, "performance_warning", cpu_usage > 80);
    d_LogStructured_SetFormat(log, false);  // Key-value format
    d_LogStructured_Commit(log);
    // Output: metric_type=system_performance cpu_usage_percent=75 memory_usage_mb=512 performance_warning=false
}

int main() {
    // Structured logging for analytics and monitoring
    log_player_event("DragonSlayer", 15, 67.5f);
    log_system_metrics(75, 512);

    return 0;
}
```

## Example 5: Complete Game Integration

```c
#include "Daedalus.h"

typedef struct {
    char name[32];
    int level;
    float health;
    float x, y;
} Player;

int main() {
    // Setup comprehensive logging
    dLogger_t* logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .colorize_output = true
    });

    FILE* analytics_file = fopen("analytics.log", "a");
    d_AddLogHandler(logger, d_ConsoleLogHandler, NULL);
    d_AddLogHandler(logger, d_FileLogHandler, analytics_file);
    d_SetGlobalLogger(logger);

    Player player = {"Hero", 5, 85.0f, 100.0f, 50.0f};

    // Context-aware logging
    dLogContext_t* game_ctx = d_PushLogContext("GameSession");
    d_LogInfo("Game session started");

    // Player spawn with structured data
    dLogStructured_t* spawn_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(spawn_log, "event", "player_spawn");
    d_LogStructured_Field(spawn_log, "player_name", player.name);
    d_LogStructured_FieldInt(spawn_log, "level", player.level);
    d_LogStructured_FieldFloat(spawn_log, "spawn_x", player.x, 2);
    d_LogStructured_FieldFloat(spawn_log, "spawn_y", player.y, 2);
    d_LogStructured_SetFormat(spawn_log, true);
    d_LogStructured_Commit(spawn_log);

    // Combat simulation with rate limiting
    dLogContext_t* combat_ctx = d_PushLogContext("Combat");
    for (int round = 1; round <= 10; round++) {
        d_LogInfoF("Combat round %d", round);

        // Rate-limited combat events
        d_LogRateLimitedF(D_LOG_LEVEL_WARNING, 3, 1.0,
                         "Player took damage in round %d", round);

        player.health -= 5.0f;
        d_LogIfF(player.health < 20.0f, D_LOG_LEVEL_ERROR,
                "CRITICAL: %s health is %0.1f!", player.name, player.health);
    }

    // Session end analytics
    dLogStructured_t* end_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(end_log, "event", "session_end");
    d_LogStructured_FieldFloat(end_log, "final_health", player.health, 1);
    d_LogStructured_FieldBool(end_log, "player_survived", player.health > 0);
    d_LogStructured_Commit(end_log);

    d_PopLogContext(combat_ctx);
    d_PopLogContext(game_ctx);

    // Cleanup
    d_DestroyLogger(logger);
    fclose(analytics_file);
    return 0;
}
```

## Quick Reference

### Log Levels
- `d_LogDebug("message")` - Development details
- `d_LogInfo("message")` - General information
- `d_LogWarning("message")` - Something needs attention
- `d_LogError("message")` - Something went wrong
- `d_LogFatal("message")` - Critical system failure

### Formatted Logging
- `d_LogInfoF("Player %s has %d health", name, health)`

### Structured Logging
- `d_LogStructured_Field(log, "key", "value")` - String field
- `d_LogStructured_FieldInt(log, "key", number)` - Integer field
- `d_LogStructured_FieldFloat(log, "key", value, decimals)` - Float field
- `d_LogStructured_FieldBool(log, "key", true/false)` - Boolean field
- `d_LogStructured_FieldTimestamp(log, "key")` - Current timestamp
- `d_LogStructured_SetFormat(log, true)` - JSON format (false = key-value)

### Rate Limiting
- `d_LogRateLimited(level, max_count, time_window, "message")`
- `d_LogRateLimitedF(level, max_count, time_window, "format", args...)`

### Conditional Logging
- `d_LogIf(condition, level, "message")`
- `d_LogIfF(condition, level, "format", args...)`

### Context Tracking
- `dLogContext_t* ctx = d_PushLogContext("ModuleName")`
- `d_PopLogContext(ctx)`

Perfect for game development, server applications, and any C project needing robust logging! 🚀
