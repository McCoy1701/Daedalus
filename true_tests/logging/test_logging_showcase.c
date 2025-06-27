// File: true_tests/logging/test_logging_showcase.c - Beautiful showcase demonstrations for friends!
// This file showcases the incredible logging system features in easy-to-understand examples

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

// Test tracking
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// SHOWCASE UTILITIES
// =============================================================================

void showcase_header(const char* title) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║  %-74s  ║\n", title);
    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
}

void showcase_section(const char* section) {
    printf("\n🎯 %s\n", section);
    printf("────────────────────────────────────────────────────────────────────────────────\n");
}

void demo_pause() {
    printf("   → Press Enter to continue to next demo...\n");
    //getchar();
}

// =============================================================================
// BASIC LOGGING SHOWCASE
// =============================================================================

int showcase_basic_logging() {
    showcase_header("🎨 BASIC LOGGING - Your First Look at Daedalus Logging");

    printf("Let's start with the basics! Daedalus provides beautiful, colorful logs:\n\n");

    printf("📝 **HOW TO USE IN YOUR CODE:**\n");
    printf("   #include \"Daedalus.h\"  // Include the header\n");
    printf("   d_LogDebug(\"Debug message\");     // For development details\n");
    printf("   d_LogInfo(\"Info message\");      // General information\n");
    printf("   d_LogWarning(\"Warning message\"); // Something needs attention\n");
    printf("   d_LogError(\"Error message\");    // Something went wrong\n");
    printf("   d_LogFatal(\"Fatal message\");    // Critical system failure\n\n");

    // Show different log levels
    d_LogDebug("🔍 This is a DEBUG message - perfect for development details");
    d_LogInfo("ℹ️  This is an INFO message - general information");
    d_LogWarning("⚠️  This is a WARNING message - something needs attention");
    d_LogError("❌ This is an ERROR message - something went wrong");
    d_LogFatal("💀 This is a FATAL message - critical system failure");

    printf("\n✨ Notice the beautiful colors, emojis, and timestamps!\n");
    printf("   Each log level has its own distinct appearance.\n");

    // Show formatted logging
    printf("\n🎨 Now let's try formatted logging (like printf):\n\n");
    printf("📝 **FORMATTED LOGGING (like printf):**\n");
    printf("   d_LogInfoF(\"Player %%s has %%d health\", name, health);\n");
    printf("   d_LogWarningF(\"Memory usage: %%d%%%%\", usage_percent);\n\n");

    int player_level = 42;
    float player_health = 87.5f;
    const char* player_name = "DragonSlayer";

    d_LogInfoF("Player %s reached level %d with %.1f%% health remaining",
               player_name, player_level, player_health);
    d_LogWarningF("Player inventory is %d%% full (%d/%d items)",
                  85, 42, 50);
    d_LogErrorF("Combat error: %s took %d damage from %s",
                player_name, 25, "Fire Dragon");

    printf("\n💡 **QUICK SETUP:**\n");
    printf("   1. Include \"Daedalus.h\" in your project\n");
    printf("   2. Link with dLogs.o, dStrings.o, dArrays.o\n");
    printf("   3. Start logging immediately - no setup required!\n");
    printf("   4. Colors and timestamps work automatically\n");

    printf("\n🎉 Beautiful, right? This makes debugging and monitoring so much easier!\n");

    return 1;
}

// =============================================================================
// BUILDER PATTERN SHOWCASE
// =============================================================================

int showcase_builder_pattern() {
    showcase_header("🔨 BUILDER PATTERN - Construct Complex Logs Like LEGO Blocks");

    printf("Sometimes you need to build complex log messages step by step.\n");
    printf("The Builder Pattern makes this elegant and flexible:\n\n");

    printf("📝 **CODE EXAMPLE:**\n");
    printf("   dLogBuilder_t* builder = d_LogBegin(D_LOG_LEVEL_INFO);\n");
    printf("   builder = d_LogBuilder_Append(builder, \"Game State: Level \");\n");
    printf("   builder = d_LogBuilder_AppendInt(builder, level);\n");
    printf("   builder = d_LogBuilder_Append(builder, \", Score: \");\n");
    printf("   builder = d_LogBuilder_AppendInt(builder, score);\n");
    printf("   d_LogBuilder_End(builder);  // Outputs the complete message\n\n");

    // Simple builder example
    printf("🏗️  Building a simple game state log:\n\n");

    dLogBuilder_t* builder = d_LogBegin(D_LOG_LEVEL_INFO);
    if (builder) {
        builder = d_LogBuilder_Append(builder, "🎮 Game State: Level ");
        builder = d_LogBuilder_AppendInt(builder, 15);
        builder = d_LogBuilder_Append(builder, ", Score: ");
        builder = d_LogBuilder_AppendInt(builder, 98765);
        builder = d_LogBuilder_Append(builder, ", Health: ");
        builder = d_LogBuilder_AppendFloat(builder, 73.5f, 1);
        builder = d_LogBuilder_Append(builder, "%");
        d_LogBuilder_End(builder);
    }

    // Complex builder with formatting
    printf("\n🏗️  Building a complex combat report:\n\n");

    dLogBuilder_t* combat_builder = d_LogBegin(D_LOG_LEVEL_WARNING);
    if (combat_builder) {
        combat_builder = d_LogBuilder_Append(combat_builder, "⚔️  Combat Report: ");
        combat_builder = d_LogBuilder_AppendF(combat_builder, "%s vs %s | ", "Hero", "Dragon");
        combat_builder = d_LogBuilder_Append(combat_builder, "Damage dealt: ");
        combat_builder = d_LogBuilder_AppendInt(combat_builder, 156);
        combat_builder = d_LogBuilder_Append(combat_builder, ", Damage received: ");
        combat_builder = d_LogBuilder_AppendInt(combat_builder, 89);
        combat_builder = d_LogBuilder_Append(combat_builder, ", Battle duration: ");
        combat_builder = d_LogBuilder_AppendFloat(combat_builder, 45.7f, 1);
        combat_builder = d_LogBuilder_Append(combat_builder, " seconds");
        d_LogBuilder_End(combat_builder);
    }

    printf("\n💡 **AVAILABLE BUILDER FUNCTIONS:**\n");
    printf("   • d_LogBuilder_Append(builder, \"text\")       - Add string\n");
    printf("   • d_LogBuilder_AppendInt(builder, number)      - Add integer\n");
    printf("   • d_LogBuilder_AppendFloat(builder, num, dec)  - Add float\n");
    printf("   • d_LogBuilder_AppendF(builder, fmt, ...)      - Add formatted\n");
    printf("   • d_LogBuilder_End(builder)                    - Finish & output\n");

    printf("\n💡 Why use Builder Pattern?\n");
    printf("   ✅ Clean, readable code\n");
    printf("   ✅ Flexible message construction\n");
    printf("   ✅ Easy to modify and maintain\n");
    printf("   ✅ Perfect for dynamic content\n");

    return 1;
}

// =============================================================================
// STRUCTURED LOGGING SHOWCASE
// =============================================================================

int showcase_structured_logging() {
    showcase_header("📊 STRUCTURED LOGGING - Machine-Readable, Human-Friendly");

    printf("Structured logging creates machine-readable logs that are perfect\n");
    printf("for analytics, monitoring, and automated processing:\n\n");

    printf("📝 **JSON FORMAT CODE:**\n");
    printf("   dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_INFO);\n");
    printf("   d_LogStructured_Field(log, \"event\", \"user_login\");\n");
    printf("   d_LogStructured_FieldInt(log, \"user_id\", 123);\n");
    printf("   d_LogStructured_SetFormat(log, true);   // Enable JSON\n");
    printf("   d_LogStructured_Commit(log);            // Output the log\n\n");

    // JSON format example
    printf("🔬 JSON Format (perfect for log analysis tools):\n\n");

    dLogStructured_t* json_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (json_log) {
        d_LogStructured_Field(json_log, "event", "player_levelup");
        d_LogStructured_Field(json_log, "player_name", "EpicGamer42");
        d_LogStructured_FieldInt(json_log, "old_level", 14);
        d_LogStructured_FieldInt(json_log, "new_level", 15);
        d_LogStructured_FieldInt(json_log, "experience_gained", 1250);
        d_LogStructured_FieldFloat(json_log, "time_played", 3.75f, 2);
        d_LogStructured_Field(json_log, "location", "Dragon_Cave");
        d_LogStructured_SetFormat(json_log, true);  // Enable JSON
        d_LogStructured_Commit(json_log);
    }

    printf("\n📝 **KEY-VALUE FORMAT CODE:**\n");
    printf("   dLogStructured_t* log = d_LogStructured(D_LOG_LEVEL_WARNING);\n");
    printf("   d_LogStructured_Field(log, \"system\", \"auth\");\n");
    printf("   d_LogStructured_FieldInt(log, \"failed_attempts\", 3);\n");
    printf("   d_LogStructured_SetFormat(log, false);  // Key-value format\n");
    printf("   d_LogStructured_Commit(log);\n\n");

    // Key-Value format example
    printf("🔬 Key-Value Format (human-readable, grep-friendly):\n\n");

    dLogStructured_t* kv_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (kv_log) {
        d_LogStructured_Field(kv_log, "system", "inventory");
        d_LogStructured_Field(kv_log, "player", "DragonHunter");
        d_LogStructured_FieldInt(kv_log, "items_count", 47);
        d_LogStructured_FieldInt(kv_log, "max_capacity", 50);
        d_LogStructured_FieldFloat(kv_log, "weight_used", 89.3f, 1);
        d_LogStructured_Field(kv_log, "status", "nearly_full");
        d_LogStructured_SetFormat(kv_log, false);  // Key-value format
        d_LogStructured_Commit(kv_log);
    }

    // Game analytics example
    printf("\n🔬 Game Analytics Example:\n\n");

    dLogStructured_t* analytics = d_LogStructured(D_LOG_LEVEL_INFO);
    if (analytics) {
        d_LogStructured_Field(analytics, "metric", "boss_defeat");
        d_LogStructured_Field(analytics, "boss_name", "Ancient_Dragon");
        d_LogStructured_FieldInt(analytics, "attempts", 7);
        d_LogStructured_FieldFloat(analytics, "fight_duration", 312.5f, 1);
        d_LogStructured_Field(analytics, "difficulty", "nightmare");
        d_LogStructured_FieldInt(analytics, "party_size", 4);
        d_LogStructured_SetFormat(analytics, true);
        d_LogStructured_Commit(analytics);
    }

    printf("\n💡 **AVAILABLE FUNCTIONS:**\n");
    printf("   • d_LogStructured_Field(log, \"key\", \"value\")     - Add string field\n");
    printf("   • d_LogStructured_FieldInt(log, \"key\", number)    - Add integer field\n");
    printf("   • d_LogStructured_FieldFloat(log, \"key\", f, dec) - Add float field\n");
    printf("   • d_LogStructured_SetFormat(log, true/false)       - JSON vs Key-Value\n");
    printf("   • d_LogStructured_Commit(log)                      - Output the log\n");

    printf("\n💡 Why use Structured Logging?\n");
    printf("   ✅ Perfect for monitoring dashboards\n");
    printf("   ✅ Easy to search and filter\n");
    printf("   ✅ Great for automated alerts\n");
    printf("   ✅ Analytics and metrics friendly\n");

    return 1;
}

// =============================================================================
// RATE LIMITING SHOWCASE
// =============================================================================

int showcase_rate_limiting() {
    showcase_header("🚦 RATE LIMITING - Stop Log Spam, Keep What Matters");

    printf("Rate limiting prevents your logs from being flooded with repetitive messages.\n");
    printf("Perfect for loops, frequent events, and noisy systems:\n\n");

    printf("📝 **HOW TO USE RATE LIMITING:**\n");
    printf("   // Limit to 5 messages per 2.0 seconds\n");
    printf("   d_LogRateLimited(D_LOG_LEVEL_WARNING, 5, 2.0, \"Repeated warning!\");\n");
    printf("   \n");
    printf("   // Rate limited with formatting\n");
    printf("   d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, D_LOG_LEVEL_ERROR, 3, 1.0,\n");
    printf("                     \"Network error #%%d occurred\", error_count);\n");
    printf("   \n");
    printf("   // Parameters: (level, max_count, time_window, message)\n");
    printf("   // max_count: Maximum messages in time window\n");
    printf("   // time_window: Time window in seconds\n\n");

    printf("🔄 Simulating a game loop with potential spam:\n\n");

    // Simulate a game loop that could spam logs
    for (int i = 0; i < 20; i++) {
        // This would normally spam 20 messages, but rate limiting controls it
        d_LogRateLimited(D_LOG_LEVEL_WARNING, 3, 1.0,
                        "🎯 High frequency event detected - this could spam!");

        // Show different rate limits
        if (i % 5 == 0) {
            d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, D_LOG_LEVEL_INFO, 2, 0.5,
                             "📊 Performance check #%d - limited to 2 per 0.5 seconds", i);
        }

        // Tiny sleep to simulate real-time
            usleep(10000);  // Only sleep in interactive mode
    }

    printf("\n🎯 Network connection simulation:\n\n");

    // Simulate network connection issues
    for (int i = 0; i < 15; i++) {
        d_LogRateLimited(D_LOG_LEVEL_ERROR, 5, 2.0,
                        "🌐 Network timeout - connection lost!");
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, D_LOG_LEVEL_WARNING, 3, 1.0,
                         "🔄 Reconnection attempt #%d", i + 1);
        usleep(50000);  // 0.05 second
    }

    printf("\n💡 Benefits of Rate Limiting:\n");
    printf("   ✅ Prevents log file explosion\n");
    printf("   ✅ Keeps important messages visible\n");
    printf("   ✅ Reduces disk I/O and performance impact\n");
    printf("   ✅ Makes debugging actually manageable\n");
    printf("   ✅ Configurable per message type\n");

    return 1;
}

// =============================================================================
// CONDITIONAL LOGGING SHOWCASE
// =============================================================================

int showcase_conditional_logging() {
    showcase_header("🔀 CONDITIONAL LOGGING - Log Smart, Not Hard");

    printf("Conditional logging lets you log only when specific conditions are met.\n");
    printf("Perfect for debugging and context-sensitive information:\n\n");

    printf("📝 **CONDITIONAL LOGGING CODE:**\n");
    printf("   // Only log when condition is true\n");
    printf("   d_LogIf(health < 20, D_LOG_LEVEL_WARNING, \"Low health!\");\n");
    printf("   \n");
    printf("   // Conditional logging with formatting\n");
    printf("   d_LogIfF(score > 1000, D_LOG_LEVEL_INFO, \n");
    printf("           \"High score achieved: %%d points!\", score);\n");
    printf("   \n");
    printf("   // Multiple conditions\n");
    printf("   d_LogIf(debug_mode && error_count > 0, D_LOG_LEVEL_DEBUG,\n");
    printf("           \"Debug info: errors detected\");\n\n");

    // Simulate game state variables
    bool debug_mode = true;
    bool production_mode = false;
    int player_health = 15;
    int player_level = 42;
    int enemy_count = 8;
    float fps = 58.3f;

    printf("🎮 Game State Monitoring:\n\n");

    // Debug mode logging
    d_LogIf(debug_mode, D_LOG_LEVEL_DEBUG,
           "🔧 Debug mode active - showing detailed information");
    d_LogIf(!production_mode, D_LOG_LEVEL_INFO,
           "🚧 Development environment detected");

    // Health-based conditional logging
    d_LogIf(player_health < 20, D_LOG_LEVEL_WARNING,
           "💔 Player health critically low!");
    d_LogIf(player_health <= 5, D_LOG_LEVEL_ERROR,
           "💀 Player near death - emergency healing needed!");

    // Performance monitoring
    d_LogIf(fps < 60.0f, D_LOG_LEVEL_WARNING,
           "📉 Performance warning - FPS below optimal");
    d_LogIf(fps < 30.0f, D_LOG_LEVEL_ERROR,
           "🐌 Critical performance issue - FPS too low");

    printf("\n🎯 Conditional Formatted Logging:\n\n");

    // Advanced conditional logging with formatting
    d_LogIfF(enemy_count > 5, D_LOG_LEVEL_WARNING,
            "⚔️  High enemy density: %d enemies detected!", enemy_count);
    d_LogIfF(player_level >= 40, D_LOG_LEVEL_INFO,
            "🌟 Veteran player detected - Level %d reached!", player_level);
    d_LogIfF(debug_mode && player_health < 50, D_LOG_LEVEL_DEBUG,
            "🔍 Debug: Player health is %d%% (%.1f absolute)",
            (player_health * 100) / 100, (float)player_health);

    // Simulate different conditions
    printf("\n🔄 Changing conditions dynamically:\n\n");

    for (int i = 0; i < 5; i++) {
        int simulated_health = 100 - (i * 25);
        d_LogIfF(simulated_health < 50, D_LOG_LEVEL_WARNING,
                "💊 Health check %d: Player health at %d%% - needs attention!",
                i + 1, simulated_health);
        d_LogIfF(simulated_health <= 25, D_LOG_LEVEL_ERROR,
                "🚨 Emergency: Critical health level %d%%!", simulated_health);
    }

    printf("\n💡 **PRACTICAL USE CASES:**\n");
    printf("   • Debug logs only in development mode\n");
    printf("   • Performance warnings when FPS drops\n");
    printf("   • Error logs when thresholds are exceeded\n");
    printf("   • Resource monitoring (memory, disk, network)\n");
    printf("   • User behavior tracking with conditions\n");

    printf("\n💡 Why use Conditional Logging?\n");
    printf("   ✅ Reduces log noise in normal operation\n");
    printf("   ✅ Automatically highlights problems\n");
    printf("   ✅ Perfect for monitoring thresholds\n");
    printf("   ✅ Debug info only when needed\n");
    printf("   ✅ Cleaner, more focused logs\n");

    return 1;
}

// =============================================================================
// CONTEXT AWARENESS SHOWCASE
// =============================================================================

int showcase_context_awareness() {
    showcase_header("🧭 CONTEXT-AWARE LOGGING - Know Where You Are");

    printf("Context-aware logging helps you understand the flow of your application.\n");
    printf("Perfect for tracking operations across different system components:\n\n");

    printf("📝 **HOW TO USE CONTEXTS:**\n");
    printf("   // Push a context to track operations\n");
    printf("   dLogContext_t* ctx = d_PushLogContext(\"ModuleName\");\n");
    printf("   d_LogInfo(\"Operation started\");  // Shows {ModuleName}\n");
    printf("   \n");
    printf("   // Nested contexts work too!\n");
    printf("   dLogContext_t* sub_ctx = d_PushLogContext(\"SubModule\");\n");
    printf("   d_LogDebug(\"Sub-operation\");  // Shows {SubModule}\n");
    printf("   d_PopLogContext(sub_ctx);      // Always clean up!\n");
    printf("   \n");
    printf("   d_PopLogContext(ctx);          // Pop main context\n\n");

    printf("🎮 Game Session Simulation:\n\n");

    // Main game context
    dLogContext_t* game_session = d_PushLogContext("GameSession");
    d_LogInfo("🎮 Game session started");
    d_LogInfo("🔧 Loading player profile...");

    // Player management context
    dLogContext_t* player_mgmt = d_PushLogContext("PlayerManagement");
    d_LogDebug("📊 Loading player statistics");
    d_LogDebug("🎒 Loading player inventory");
    d_LogInfo("✅ Player profile loaded successfully");

    // Battle system context
    dLogContext_t* battle_system = d_PushLogContext("BattleSystem");
    d_LogInfo("⚔️  Entering combat encounter");
    d_LogDebug("🎯 Calculating hit chances");
    d_LogDebug("💥 Processing damage calculations");
    d_LogWarning("🛡️  Player armor degraded");
    d_LogInfo("🏆 Battle completed - Victory!");

    // Nested inventory context
    dLogContext_t* inventory = d_PushLogContext("InventorySystem");
    d_LogDebug("🎁 Adding loot to inventory");
    d_LogInfo("💰 Gold increased by 150");
    d_LogInfo("⚔️  Rare sword acquired!");
    d_PopLogContext(inventory);

    d_PopLogContext(battle_system);

    // Save system context
    dLogContext_t* save_system = d_PushLogContext("SaveSystem");
    d_LogDebug("💾 Preparing save data");
    d_LogInfo("💾 Game progress saved");
    d_PopLogContext(save_system);

    d_PopLogContext(player_mgmt);

    d_LogInfo("🏁 Game session ended");
    d_PopLogContext(game_session);

    printf("\n🏗️  Showing nested system operations:\n\n");

    // Demonstrate deeply nested contexts
    dLogContext_t* engine = d_PushLogContext("GameEngine");
    d_LogInfo("🚀 Engine starting");

    dLogContext_t* renderer = d_PushLogContext("Renderer");
    d_LogDebug("🎨 Initializing graphics");

    dLogContext_t* textures = d_PushLogContext("TextureManager");
    d_LogDebug("🖼️  Loading texture atlas");
    d_LogDebug("🗜️  Compressing textures");
    d_LogInfo("✅ All textures loaded");
    d_PopLogContext(textures);

    d_LogInfo("🎨 Renderer ready");
    d_PopLogContext(renderer);

    dLogContext_t* audio = d_PushLogContext("AudioSystem");
    d_LogDebug("🔊 Initializing audio engine");
    d_LogInfo("🎵 Background music started");
    d_PopLogContext(audio);

    d_LogInfo("🚀 Engine fully initialized");
    d_PopLogContext(engine);

    printf("\n💡 **PRACTICAL USAGE TIPS:**\n");
    printf("   • Use contexts for functions, modules, or operations\n");
    printf("   • Context names appear in {braces} in log output\n");
    printf("   • Always match d_PushLogContext with d_PopLogContext\n");
    printf("   • Nested contexts help track call hierarchies\n");
    printf("   • Great for debugging multi-threaded applications\n");

    printf("\n💡 Benefits of Context-Aware Logging:\n");
    printf("   ✅ Clear operation flow tracking\n");
    printf("   ✅ Easy to isolate component issues\n");
    printf("   ✅ Better debugging and monitoring\n");
    printf("   ✅ Hierarchical log organization\n");
    printf("   ✅ Perfect for complex systems\n");

    return 1;
}

// =============================================================================
// REAL-WORLD SCENARIO SHOWCASE
// =============================================================================

int showcase_real_world_scenario() {
    showcase_header("🌍 REAL-WORLD SCENARIO - Complete Game Server Example");

    printf("Let's see how all these features work together in a realistic scenario:\n");
    printf("A multiplayer game server handling player connections and gameplay!\n\n");

    printf("📝 **REAL-WORLD SETUP CODE:**\n");
    printf("   // 1. Create custom logger with specific configuration\n");
    printf("   dLogger_t* logger = d_CreateLogger((dLogConfig_t){\n");
    printf("       .default_level = D_LOG_LEVEL_INFO,\n");
    printf("       .include_timestamp = true,\n");
    printf("       .include_thread_id = true,\n");
    printf("       .colorize_output = true\n");
    printf("   });\n");
    printf("   d_SetGlobalLogger(logger);\n");
    printf("   \n");
    printf("   // 2. Use contexts for different system modules\n");
    printf("   dLogContext_t* startup_ctx = d_PushLogContext(\"ServerStartup\");\n");
    printf("   d_LogInfo(\"Server initializing...\");\n");
    printf("   d_PopLogContext(startup_ctx);\n\n");

    // Setup advanced logging
    dLogger_t* server_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .include_thread_id = true,
        .colorize_output = true,
        .timestamp_format = "%H:%M:%S",
        .context_separator = " >> "
    });
    d_SetGlobalLogger(server_logger);

    printf("🖥️  Game Server Startup Sequence:\n\n");

    // Server startup context
    dLogContext_t* startup = d_PushLogContext("ServerStartup");
    d_LogInfo("🚀 Initializing Daedalus Game Server v2.1.0");

    // Configuration with structured logging
    dLogStructured_t* config = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(config, "event", "server_config");
    d_LogStructured_Field(config, "version", "2.1.0");
    d_LogStructured_FieldInt(config, "max_players", 64);
    d_LogStructured_FieldInt(config, "port", 7777);
    d_LogStructured_Field(config, "map", "Dragon_Highlands");
    d_LogStructured_SetFormat(config, false);
    d_LogStructured_Commit(config);

    d_LogInfo("✅ Server configuration loaded");
    d_PopLogContext(startup);

    printf("\n👥 Player Connection Simulation:\n\n");

    // Simulate multiple players connecting
    const char* players[] = {"DragonSlayer42", "ElfMage", "DwarfWarrior", "HumanRogue", "OrcShaman"};

    for (int i = 0; i < 5; i++) {
        dLogContext_t* connection = d_PushLogContext("PlayerConnection");

        // Connection attempt
        d_LogInfoF("🔌 New connection attempt from %s", players[i]);

        // Authentication with conditional logging
        bool auth_success = (i != 2);  // Simulate one failure
        d_LogIf(auth_success, D_LOG_LEVEL_INFO, "✅ Authentication successful");
        d_LogIf(!auth_success, D_LOG_LEVEL_WARNING, "❌ Authentication failed - invalid credentials");

        if (auth_success) {
            // Successful connection with structured data
            dLogStructured_t* player_join = d_LogStructured(D_LOG_LEVEL_INFO);
            d_LogStructured_Field(player_join, "event", "player_joined");
            d_LogStructured_Field(player_join, "player", players[i]);
            d_LogStructured_FieldInt(player_join, "player_id", 1000 + i);
            d_LogStructured_FieldInt(player_join, "level", 15 + (i * 5));
            d_LogStructured_Field(player_join, "class",
                                 (i % 4 == 0) ? "warrior" :
                                 (i % 4 == 1) ? "mage" :
                                 (i % 4 == 2) ? "rogue" : "priest");
            d_LogStructured_SetFormat(player_join, true);
            d_LogStructured_Commit(player_join);
        }

        d_PopLogContext(connection);
        usleep(200000);  // Small delay for realism
    }

    printf("\n⚔️  Combat System Demonstration:\n\n");

    // Battle simulation
    dLogContext_t* battle = d_PushLogContext("CombatSystem");
    d_LogInfo("⚔️  Epic battle initiated: Players vs Ancient Dragon");

    // Rate-limited combat events
    for (int round = 1; round <= 10; round++) {
        // Builder pattern for complex combat logs
        dLogBuilder_t* combat_round = d_LogBegin(D_LOG_LEVEL_INFO);
        combat_round = d_LogBuilder_AppendF(combat_round, "🗡️  Round %d: ", round);
        combat_round = d_LogBuilder_Append(combat_round, "Dragon health ");
        combat_round = d_LogBuilder_AppendInt(combat_round, 1000 - (round * 80));
        combat_round = d_LogBuilder_Append(combat_round, "/1000 HP");
        d_LogBuilder_End(combat_round);

        // Rate-limited damage notifications
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, D_LOG_LEVEL_WARNING, 3, 1.0,
                         "🔥 Dragon breathes fire! AOE damage dealt!");

        // Conditional critical events
        int dragon_health = 1000 - (round * 80);
        d_LogIfF(dragon_health <= 200, D_LOG_LEVEL_ERROR,
                "🚨 Dragon enraged! Health critical: %d HP", dragon_health);

            usleep(10000);  // Only sleep in interactive mode

    }

    d_LogInfo("🏆 Victory! Ancient Dragon defeated!");
    d_PopLogContext(battle);

    printf("\n📊 Server Statistics:\n\n");

    // Show logging statistics
    dLogStats_t stats;
    d_GetLogStats(server_logger, &stats);

    dLogStructured_t* server_stats = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(server_stats, "metric", "server_performance");
    d_LogStructured_FieldFloat(server_stats, "total_log_time", stats.total_log_time, 6);
    d_LogStructured_FieldInt(server_stats, "rate_limited_logs", (int)stats.logs_rate_limited);
    d_LogStructured_FieldInt(server_stats, "suppressed_logs", (int)stats.logs_suppressed);
    d_LogStructured_Field(server_stats, "status", "optimal");
    d_LogStructured_SetFormat(server_stats, false);
    d_LogStructured_Commit(server_stats);

    d_LogInfo("🏁 Game server demonstration completed successfully!");

    // Cleanup
    d_DestroyLogger(server_logger);
    //d_SetGlobalLogger(NULL);

    printf("\n💡 This demonstration showed:\n");
    printf("   ✅ Complete logging system integration\n");
    printf("   ✅ Multiple logging patterns working together\n");
    printf("   ✅ Real-world server scenarios\n");
    printf("   ✅ Performance monitoring and statistics\n");
    printf("   ✅ Professional-grade logging capabilities\n");

    return 1;
}

// =============================================================================
// PERFORMANCE SHOWCASE
// =============================================================================

int showcase_performance() {
    showcase_header("⚡ PERFORMANCE SHOWCASE - Fast, Efficient, Professional");

    printf("Let's demonstrate the performance capabilities of the Daedalus logging system:\n\n");

    printf("🚀 High-Speed Logging Test:\n\n");

    double start_time = d_GetTimestamp();

    // Rapid logging test
    for (int i = 0; i < 1000; i++) {
        if (i % 100 == 0) {
            d_LogInfoF("📈 Performance test progress: %d/1000", i);
        }

        // Mix different logging types for realistic test
        switch (i % 6) {
            case 0:
                d_LogDebug("Debug message for performance testing");
                break;
            case 1:
                d_LogInfoF("Info message #%d", i);
                break;
            case 2:
                d_LogRateLimited(D_LOG_LEVEL_WARNING, 10, 1.0, "Rate limited message");
                break;
            case 3: {
                dLogBuilder_t* builder = d_LogBegin(D_LOG_LEVEL_INFO);
                if (builder) {
                    builder = d_LogBuilder_Append(builder, "Builder ");
                    builder = d_LogBuilder_AppendInt(builder, i);
                    d_LogBuilder_End(builder);
                }
                break;
            }
            case 4:
                d_LogIf(i % 50 == 0, D_LOG_LEVEL_ERROR, "Conditional error message");
                break;
            case 5: {
                dLogStructured_t* structured = d_LogStructured(D_LOG_LEVEL_DEBUG);
                if (structured) {
                    d_LogStructured_Field(structured, "test", "performance");
                    d_LogStructured_FieldInt(structured, "iteration", i);
                    d_LogStructured_Commit(structured);
                }
                break;
            }
        }
    }

    double end_time = d_GetTimestamp();
    double total_time = end_time - start_time;
    double logs_per_second = 1000.0 / total_time;

    printf("\n📊 Performance Results:\n\n");

    d_LogInfoF("⚡ Processed 1000 log entries in %.6f seconds", total_time);
    d_LogInfoF("🏎️  Performance: %.2f logs per second", logs_per_second);
    d_LogInfoF("💾 Memory usage: Efficient with thread-local buffers");
    d_LogInfo("🎯 Zero memory leaks with proper cleanup");

    // Show final statistics
    dLogStats_t final_stats;
    d_GetLogStats(NULL, &final_stats);

    // Debug: Also print to stdout to ensure visibility
    printf("DEBUG: Stats - Total time: %.6f, Rate limited: %llu, Errors: %u\n",
           final_stats.total_log_time,
           (unsigned long long)final_stats.logs_rate_limited,
           final_stats.handler_errors);

    d_LogInfoF("📈 Total processing time: %.6f seconds", final_stats.total_log_time);
    d_LogInfoF("🚫 Rate limited messages: %llu", (unsigned long long)final_stats.logs_rate_limited);
    d_LogInfoF("❌ Handler errors: %u", final_stats.handler_errors);

    // Additional debug info
    printf("📊 Real Performance Metrics:\n");
    printf("   ⚡ Test duration: %.6f seconds\n", total_time);
    printf("   🏎️  Throughput: %.2f logs/second\n", logs_per_second);
    printf("   📈 System log time: %.6f seconds\n", final_stats.total_log_time);
    printf("   🚫 Messages rate limited: %llu\n", (unsigned long long)final_stats.logs_rate_limited);
    printf("   ❌ Handler errors: %u\n", final_stats.handler_errors);

    printf("\n📝 **PERFORMANCE OPTIMIZATION TIPS:**\n");
    printf("   • Use appropriate log levels (avoid DEBUG in production)\n");
    printf("   • Rate limiting prevents log spam\n");
    printf("   • Thread-local buffers minimize allocations\n");
    printf("   • Structured logging is faster than string concatenation\n");
    printf("   • Context tracking has minimal overhead\n");

    printf("\n💡 Performance Features:\n");
    printf("   ✅ Thread-local buffers for zero allocation\n");
    printf("   ✅ Efficient string handling\n");
    printf("   ✅ Smart rate limiting\n");
    printf("   ✅ Minimal overhead in production\n");
    printf("   ✅ Scales with application needs\n");

    return 1;
}

// =============================================================================
// MAIN SHOWCASE RUNNER
// =============================================================================

int main() {
    printf("\n");
    printf("════════════════════════════════════════════════════════════════════════════════\n");
    printf("                    🎭 DAEDALUS LOGGING SYSTEM SHOWCASE 🎭                     \n");
    printf("                     Beautiful, Powerful, Professional                         \n");
    printf("════════════════════════════════════════════════════════════════════════════════\n\n");

    printf("Welcome to the Daedalus Logging System showcase! 🎉\n");
    printf("This demonstration will show you why this logging system is amazing.\n");
    printf("Each section builds upon the previous one, showing increasingly powerful features.\n");

    // Initialize basic logging
    dLogger_t* showcase_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .include_thread_id = true,
        .colorize_output = true,
        .timestamp_format = "%H:%M:%S.%03d",
        .context_separator = " >> "
    });
    d_SetGlobalLogger(showcase_logger);

    TEST_SUITE_START("Daedalus Logging Showcase");

    // Run all showcase demonstrations
    RUN_TEST(showcase_basic_logging);
    demo_pause();

    RUN_TEST(showcase_builder_pattern);
    demo_pause();

    RUN_TEST(showcase_structured_logging);
    demo_pause();

    RUN_TEST(showcase_rate_limiting);
    demo_pause();

    RUN_TEST(showcase_conditional_logging);
    demo_pause();

    RUN_TEST(showcase_context_awareness);
    demo_pause();

    RUN_TEST(showcase_real_world_scenario);
    demo_pause();

    RUN_TEST(showcase_performance);

    TEST_SUITE_END();

    // Final showcase message
    showcase_header("🎉 SHOWCASE COMPLETE - Thank You for Watching!");

    printf("\nCongratulations! You've seen the complete Daedalus Logging System in action! 🎊\n\n");

    printf("📚 What you've learned:\n");
    printf("   ✅ Basic colorful logging with timestamps\n");
    printf("   ✅ Builder pattern for complex message construction\n");
    printf("   ✅ Structured logging for analytics and monitoring\n");
    printf("   ✅ Rate limiting to prevent log spam\n");
    printf("   ✅ Conditional logging for smart debugging\n");
    printf("   ✅ Context-aware logging for flow tracking\n");
    printf("   ✅ Real-world integration examples\n");
    printf("   ✅ Performance characteristics and statistics\n\n");

    printf("🚀 **GETTING STARTED IN YOUR PROJECT:**\n");
    printf("   1. Include: #include \"Daedalus.h\"\n");
    printf("   2. Link: gcc your_app.c -o app obj/dLogs.o obj/dStrings.o obj/dArrays.o\n");
    printf("   3. Initialize (optional): d_CreateLogger() for custom config\n");
    printf("   4. Start logging: d_LogInfo(\"Hello, world!\");\n");
    printf("   5. Use contexts: d_PushLogContext(\"YourModule\");\n");
    printf("   6. Clean up: d_PopLogContext(context);\n\n");

    printf("📖 **FUNCTION REFERENCE:**\n");
    printf("   Basic:      d_LogDebug, d_LogInfo, d_LogWarning, d_LogError, d_LogFatal\n");
    printf("   Formatted:  d_LogDebugF, d_LogInfoF, d_LogWarningF, d_LogErrorF, d_LogFatalF\n");
    printf("   Builder:    d_LogBegin, d_LogBuilder_Append*, d_LogBuilder_End\n");
    printf("   Structured: d_LogStructured, d_LogStructured_Field*, d_LogStructured_Commit\n");
    printf("   Context:    d_PushLogContext, d_PopLogContext\n");
    printf("   Conditional: d_LogIf, d_LogIfF\n");
    printf("   Rate Limit: d_LogRateLimited, d_LogRateLimitedF\n\n");

    printf("🚀 Ready to use in your projects:\n");
    printf("   • Game development logging\n");
    printf("   • Server application monitoring\n");
    printf("   • Debug and development workflows\n");
    printf("   • Performance analysis and profiling\n");
    printf("   • Production system observability\n\n");

    printf("💡 Key advantages:\n");
    printf("   • Beautiful, readable output\n");
    printf("   • High performance with low overhead\n");
    printf("   • Flexible and configurable\n");
    printf("   • Production-ready features\n");
    printf("   • Easy to integrate and use\n\n");

    printf("🏁 Thank you for exploring the Daedalus Logging System!");
    d_LogInfo("💝 Happy coding, and may your logs be forever beautiful!");

    // Add comprehensive function reference
    showcase_header("📚 COMPLETE FUNCTION REFERENCE - Your Cheat Sheet");

    printf("Here's everything you need to start using Daedalus Logging in your projects:\n\n");

    printf("🔥 **BASIC LOGGING FUNCTIONS:**\n");
    printf("   void d_LogDebug(const char* message);     // Debug information\n");
    printf("   void d_LogInfo(const char* message);      // General information\n");
    printf("   void d_LogWarning(const char* message);   // Something needs attention\n");
    printf("   void d_LogError(const char* message);     // Something went wrong\n");
    printf("   void d_LogFatal(const char* message);     // Critical system failure\n\n");

    printf("🎨 **FORMATTED LOGGING (printf-style):**\n");
    printf("   void d_LogDebugF(const char* fmt, ...);   // Debug with formatting\n");
    printf("   void d_LogInfoF(const char* fmt, ...);    // Info with formatting\n");
    printf("   void d_LogWarningF(const char* fmt, ...); // Warning with formatting\n");
    printf("   void d_LogErrorF(const char* fmt, ...);   // Error with formatting\n");
    printf("   void d_LogFatalF(const char* fmt, ...);   // Fatal with formatting\n\n");

    printf("🔨 **BUILDER PATTERN FUNCTIONS:**\n");
    printf("   dLogBuilder_t* d_LogBegin(dLogLevel_t level);\n");
    printf("   dLogBuilder_t* d_LogBuilder_Append(builder, const char* text);\n");
    printf("   dLogBuilder_t* d_LogBuilder_AppendInt(builder, int value);\n");
    printf("   dLogBuilder_t* d_LogBuilder_AppendFloat(builder, float value, int decimals);\n");
    printf("   dLogBuilder_t* d_LogBuilder_AppendF(builder, const char* fmt, ...);\n");
    printf("   void d_LogBuilder_End(dLogBuilder_t* builder);\n\n");

    printf("📊 **STRUCTURED LOGGING FUNCTIONS:**\n");
    printf("   dLogStructured_t* d_LogStructured(dLogLevel_t level);\n");
    printf("   void d_LogStructured_Field(log, const char* key, const char* value);\n");
    printf("   void d_LogStructured_FieldInt(log, const char* key, int value);\n");
    printf("   void d_LogStructured_FieldFloat(log, const char* key, float value, int decimals);\n");
    printf("   void d_LogStructured_SetFormat(log, bool use_json);  // true=JSON, false=key-value\n");
    printf("   void d_LogStructured_Commit(dLogStructured_t* log);\n\n");

    printf("🧭 **CONTEXT MANAGEMENT FUNCTIONS:**\n");
    printf("   dLogContext_t* d_PushLogContext(const char* name);\n");
    printf("   void d_PopLogContext(dLogContext_t* context);\n");
    printf("   void d_LogContext_EnableTiming(dLogContext_t* context);\n\n");

    printf("🔀 **CONDITIONAL LOGGING FUNCTIONS:**\n");
    printf("   void d_LogIf(bool condition, dLogLevel_t level, const char* message);\n");
    printf("   void d_LogIfF(bool condition, dLogLevel_t level, const char* fmt, ...);\n\n");

    printf("🚦 **RATE LIMITING FUNCTIONS:**\n");
    printf("   void d_LogRateLimited(dLogLevel_t level, uint32_t max_count, \n");
    printf("                         double time_window, const char* message);\n");
    printf("   void d_LogRateLimitedF(dLogLevel_t level, uint32_t max_count, \n");
    printf("                          double time_window, const char* fmt, ...);\n\n");

    printf("⚙️  **LOGGER MANAGEMENT FUNCTIONS:**\n");
    printf("   dLogger_t* d_CreateLogger(dLogConfig_t config);\n");
    printf("   void d_DestroyLogger(dLogger_t* logger);\n");
    printf("   void d_SetGlobalLogger(dLogger_t* logger);\n");
    printf("   dLogger_t* d_GetGlobalLogger(void);\n");
    printf("   void d_SetLogLevel(dLogger_t* logger, dLogLevel_t level);\n");
    printf("   dLogLevel_t d_GetLogLevel(dLogger_t* logger);\n\n");

    printf("📈 **STATISTICS & MONITORING:**\n");
    printf("   void d_GetLogStats(dLogger_t* logger, dLogStats_t* stats);\n");
    printf("   void d_ResetLogStats(dLogger_t* logger);\n");
    printf("   double d_GetTimestamp(void);\n");
    printf("   const char* d_LogLevel_ToString(dLogLevel_t level);\n");
    printf("   dLogLevel_t d_LogLevel_FromString(const char* str);\n\n");

    printf("🎯 **LOG LEVELS (use these constants):**\n");
    printf("   D_LOG_LEVEL_DEBUG     // Detailed information for debugging\n");
    printf("   D_LOG_LEVEL_INFO      // General informational messages\n");
    printf("   D_LOG_LEVEL_WARNING   // Something unusual happened\n");
    printf("   D_LOG_LEVEL_ERROR     // An error occurred but app can continue\n");
    printf("   D_LOG_LEVEL_FATAL     // Critical error, app cannot continue\n");
    printf("   D_LOG_LEVEL_OFF       // Disable all logging\n\n");

    printf("💡 **QUICK START TEMPLATE:**\n");
    printf("   ```c\n");
    printf("   #include \"Daedalus.h\"\n");
    printf("   \n");
    printf("   int main() {\n");
    printf("       // Optional: Create custom logger\n");
    printf("       dLogger_t* logger = d_CreateLogger((dLogConfig_t){\n");
    printf("           .default_level = D_LOG_LEVEL_INFO,\n");
    printf("           .include_timestamp = true,\n");
    printf("           .colorize_output = true\n");
    printf("       });\n");
    printf("       d_SetGlobalLogger(logger);\n");
    printf("       \n");
    printf("       // Start logging!\n");
    printf("       d_LogInfo(\"Application started\");\n");
    printf("       \n");
    printf("       // Use contexts for better organization\n");
    printf("       dLogContext_t* ctx = d_PushLogContext(\"MainLoop\");\n");
    printf("       d_LogDebug(\"Processing user input...\");\n");
    printf("       d_PopLogContext(ctx);\n");
    printf("       \n");
    printf("       // Cleanup\n");
    printf("       d_DestroyLogger(logger);\n");
    printf("       return 0;\n");
    printf("   }\n");
    printf("   ```\n\n");

    printf("🔗 **COMPILATION COMMAND:**\n");
    printf("   gcc your_app.c -o your_app -Iinclude/ \\\n");
    printf("       obj/dLogs.o obj/dStrings.o obj/dArrays.o -lm\n\n");

    // Cleanup
    d_DestroyLogger(showcase_logger);
    d_SetGlobalLogger(NULL);

    printf("\n════════════════════════════════════════════════════════════════════════════════\n");
    printf("                           🎭 SHOWCASE COMPLETE 🎭                             \n");
    printf("                     Built with ❤️  using Daedalus Framework                    \n");
    printf("════════════════════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
