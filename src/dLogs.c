// File: src/dLogs.c - Daedalus Logging System Implementation
// Define feature test macros before any includes
#define _POSIX_C_SOURCE 200809L  // For clock_gettime, struct timespec
#define _GNU_SOURCE              // For syscall, SYS_gettid

#include "Daedalus.h"
#include <stdarg.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

// For strcasecmp on some systems
#ifdef _WIN32
    #define strcasecmp _stricmp
#else
    #include <strings.h>
#endif

// For thread ID and mutex support
#ifdef _WIN32
    #include <windows.h>
    #define GET_THREAD_ID() GetCurrentThreadId()
    typedef CRITICAL_SECTION dMutex_t;
    #define MUTEX_INIT(m) InitializeCriticalSection(m)
    #define MUTEX_DESTROY(m) DeleteCriticalSection(m)
    #define MUTEX_LOCK(m) EnterCriticalSection(m)
    #define MUTEX_UNLOCK(m) LeaveCriticalSection(m)
#else
    #include <sys/syscall.h>
    #include <pthread.h>
    #define GET_THREAD_ID() (uint32_t)syscall(SYS_gettid)
    typedef pthread_mutex_t dMutex_t;
    #define MUTEX_INIT(m) pthread_mutex_init(m, NULL)
    #define MUTEX_DESTROY(m) pthread_mutex_destroy(m)
    #define MUTEX_LOCK(m) pthread_mutex_lock(m)
    #define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#endif

// =============================================================================
// CUSTOM COLOR PALETTE SYSTEM - Matching Your CSS Color Scheme
// =============================================================================

// ANSI Color codes for enhanced logging output
#define LOG_COLOR_RESET      "\033[0m"
#define LOG_COLOR_BOLD       "\033[1m"
#define LOG_COLOR_DIM        "\033[2m"
#define LOG_COLOR_UNDERLINE  "\033[4m"
#define LOG_COLOR_BLINK      "\033[5m"

// Custom RGB color support (24-bit) - Your Color Palette
// BLUES - Cool Ocean Tones
#define LOG_COLOR_BLUE_DARKEST   "\033[38;2;23;32;56m"    /* #172038 - Deep navy */
#define LOG_COLOR_BLUE_DARK      "\033[38;2;37;58;94m"    /* #253a5e - Dark blue */
#define LOG_COLOR_BLUE_MEDIUM    "\033[38;2;60;94;139m"   /* #3c5e8b - Medium blue */
#define LOG_COLOR_BLUE_LIGHT     "\033[38;2;79;143;186m"  /* #4f8fba - Sky blue */
#define LOG_COLOR_BLUE_LIGHTER   "\033[38;2;115;190;211m" /* #73bed3 - Light cyan */

// GREENS - Nature Earth Tones
#define LOG_COLOR_GREEN_DARKEST  "\033[38;2;25;51;45m"    /* #19332d - Dark forest */
#define LOG_COLOR_GREEN_DARK     "\033[38;2;37;86;46m"    /* #25562e - Deep green */
#define LOG_COLOR_GREEN_MEDIUM   "\033[38;2;70;130;50m"   /* #468232 - Forest green */
#define LOG_COLOR_GREEN_LIGHT    "\033[38;2;117;167;67m"  /* #75a743 - Grass green */
#define LOG_COLOR_GREEN_LIGHTER  "\033[38;2;168;202;88m"  /* #a8ca58 - Light lime */

// ORANGES - Fire Warm Tones
#define LOG_COLOR_ORANGE_DARKEST "\033[38;2;52;28;39m"    /* #341c27 - Dark maroon */
#define LOG_COLOR_ORANGE_DARK    "\033[38;2;96;44;44m"    /* #602c2c - Dark red-brown */
#define LOG_COLOR_ORANGE_MEDIUM  "\033[38;2;136;75;43m"   /* #884b2b - Brown-orange */
#define LOG_COLOR_ORANGE_LIGHT   "\033[38;2;190;119;43m"  /* #be772b - Orange */
#define LOG_COLOR_ORANGE_LIGHTER "\033[38;2;222;158;65m"  /* #de9e41 - Light orange */

// REDS - Bold Vibrant Tones
#define LOG_COLOR_RED_DARKEST    "\033[38;2;36;21;39m"    /* #241527 - Dark purple-red */
#define LOG_COLOR_RED_DARK       "\033[38;2;65;29;49m"    /* #411d31 - Deep maroon */
#define LOG_COLOR_RED_MEDIUM     "\033[38;2;117;36;56m"   /* #752438 - Dark red */
#define LOG_COLOR_RED_LIGHT      "\033[38;2;165;48;48m"   /* #a53030 - Red */
#define LOG_COLOR_RED_LIGHTER    "\033[38;2;207;87;60m"   /* #cf573c - Light red */

// PURPLES - Mystic Magical Tones
#define LOG_COLOR_PURPLE_DARKEST "\033[38;2;30;29;57m"    /* #1e1d39 - Deep indigo */
#define LOG_COLOR_PURPLE_DARK    "\033[38;2;64;39;81m"    /* #402751 - Dark purple */
#define LOG_COLOR_PURPLE_MEDIUM  "\033[38;2;122;54;123m"  /* #7a367b - Medium purple */
#define LOG_COLOR_PURPLE_LIGHT   "\033[38;2;162;62;140m"  /* #a23e8c - Light purple */

// GRAYS - Neutral Balanced Tones
#define LOG_COLOR_GRAY_DARKEST   "\033[38;2;9;10;20m"     /* #090a14 - Almost black */
#define LOG_COLOR_GRAY_DARK      "\033[38;2;21;29;40m"    /* #151d28 - Dark gray */
#define LOG_COLOR_GRAY_MEDIUM    "\033[38;2;57;74;80m"    /* #394a50 - Medium gray */
#define LOG_COLOR_GRAY_LIGHT     "\033[38;2;129;151;150m" /* #819796 - Light gray */
#define LOG_COLOR_GRAY_LIGHTER   "\033[38;2;168;181;178m" /* #a8b5b2 - Lighter gray */
#define LOG_COLOR_GRAY_LIGHTEST  "\033[38;2;199;207;204m" /* #c7cfcc - Very light gray */

// Semantic Color Mappings for Log Levels (matching your CSS semantic colors)
#define LOG_COLOR_DEBUG_PREFIX   LOG_COLOR_BLUE_LIGHT LOG_COLOR_BOLD      /* --color-info: var(--color-blue-light) */
#define LOG_COLOR_INFO_PREFIX    LOG_COLOR_GREEN_MEDIUM LOG_COLOR_BOLD     /* --color-success: var(--color-green-medium) */
#define LOG_COLOR_WARNING_PREFIX LOG_COLOR_ORANGE_LIGHTER LOG_COLOR_BOLD   /* --color-warning: var(--color-orange-lighter) */
#define LOG_COLOR_ERROR_PREFIX   LOG_COLOR_RED_LIGHT LOG_COLOR_BOLD       /* --color-error: var(--color-red-light) */
#define LOG_COLOR_FATAL_PREFIX   LOG_COLOR_PURPLE_LIGHT LOG_COLOR_BOLD LOG_COLOR_BLINK /* --color-secondary: var(--color-purple-medium) */

// CSS Variable Mapping Function - Direct correspondence to your root.css
static const char* get_semantic_color_for_level(dLogLevel_t level) {
    switch (level) {
        case D_LOG_LEVEL_DEBUG:   return LOG_COLOR_BLUE_LIGHT;      /* var(--color-info) */
        case D_LOG_LEVEL_INFO:    return LOG_COLOR_GREEN_MEDIUM;    /* var(--color-success) */
        case D_LOG_LEVEL_WARNING: return LOG_COLOR_ORANGE_LIGHTER;  /* var(--color-warning) */
        case D_LOG_LEVEL_ERROR:   return LOG_COLOR_RED_LIGHT;       /* var(--color-error) */
        case D_LOG_LEVEL_FATAL:   return LOG_COLOR_PURPLE_LIGHT;    /* var(--color-secondary) */
        default:                  return LOG_COLOR_GRAY_LIGHT;      /* var(--text-muted) */
    }
}

// Helper function to check if terminal supports colors
static bool is_color_terminal(void)
{
    // Check if we're writing to a terminal
    if (!isatty(STDOUT_FILENO)) return false;

    // Check TERM environment variable
    const char* term = getenv("TERM");
    if (!term) return false;

    // Basic color terminal detection
    return (strstr(term, "color") != NULL ||
            strstr(term, "xterm") != NULL ||
            strstr(term, "screen") != NULL ||
            strcmp(term, "linux") == 0);
}


// =============================================================================
// GLOBAL CONFIGURATION
// =============================================================================

// Global logging configuration (zero-initialized by default)
dLogGlobalConfig_t g_log_config = {
    .default_level = D_LOG_LEVEL_INFO,
    .logging_enabled = true
};

// Thread-local storage for format buffers
static __thread dString_t* tls_format_buffer = NULL;

// Global logger instance
static dLogger_t* g_global_logger = NULL;
static pthread_mutex_t rate_limit_mutex = PTHREAD_MUTEX_INITIALIZER;
// Global color support detection
static bool g_color_support_detected = false;
static bool g_supports_color = false;

// Forward declaration for stats function
static void update_log_stats(dLogger_t* logger, dLogLevel_t level, double processing_time, bool was_suppressed, bool was_rate_limited, bool handler_error);

// Context stack for tracking current logging context
static dLogContext_t* g_context_stack = NULL;

// Buffer for building hierarchical context strings
static __thread char tls_context_buffer[512] = {0};

/*
 * Build hierarchical context string from the stack
 */
static const char* get_current_context_name(void) {
    if (!g_context_stack) {
        return NULL;
    }

    // Build context hierarchy bottom-up
    dLogContext_t* contexts[32];  // Max 32 nested contexts
    int count = 0;

    // Collect contexts from current to root
    dLogContext_t* current = g_context_stack;
    while (current && count < 32) {
        contexts[count++] = current;
        current = current->parent;  // Follow parent chain
    }

    // Build context string top-down
    tls_context_buffer[0] = '\0';
    for (int i = count - 1; i >= 0; i--) {
        if (i < count - 1) {
            strcat(tls_context_buffer, "::");
        }
        strcat(tls_context_buffer, contexts[i]->name);
    }

    return tls_context_buffer;
}



// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================
// Rate limit cache declaration (moved here to be accessible)
static dArray_t* rate_limit_cache = NULL;

static void init_rate_limit_cache() {
    if (!rate_limit_cache) {
        rate_limit_cache = d_InitArray(100, sizeof(dLogRateLimit_t));
    }
}

static uint32_t hash_message(const char* message) {
    if (!message) return 0;

    uint32_t hash = 5381;
    int c;
    while ((c = *message++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/*
 * Get string representation of log level
 */
const char* d_LogLevel_ToString(dLogLevel_t level)
{
    switch (level) {
        case D_LOG_LEVEL_DEBUG:   return "DEBUG";
        case D_LOG_LEVEL_INFO:    return "INFO";
        case D_LOG_LEVEL_WARNING: return "WARNING";
        case D_LOG_LEVEL_ERROR:   return "ERROR";
        case D_LOG_LEVEL_FATAL:   return "FATAL";
        case D_LOG_LEVEL_OFF:     return "OFF";
        default:                  return "UNKNOWN";
    }
}

/*
 * Parse log level from string
 */
dLogLevel_t d_LogLevel_FromString(const char* str)
{
    if (!str) return D_LOG_LEVEL_INFO;

    // Check single character shortcuts
    if (str[1] == '\0') {
        switch (str[0]) {
            case 'D': case 'd': return D_LOG_LEVEL_DEBUG;
            case 'I': case 'i': return D_LOG_LEVEL_INFO;
            case 'W': case 'w': return D_LOG_LEVEL_WARNING;
            case 'E': case 'e': return D_LOG_LEVEL_ERROR;
            case 'F': case 'f': return D_LOG_LEVEL_FATAL;
            case 'O': case 'o': return D_LOG_LEVEL_OFF;
        }
    }

    // Simple case-insensitive comparison without strcasecmp
    // Convert to uppercase for comparison
    char upper_str[32];
    size_t len = strlen(str);
    if (len >= sizeof(upper_str)) return D_LOG_LEVEL_INFO;

    for (size_t i = 0; i <= len; i++) {
        upper_str[i] = (str[i] >= 'a' && str[i] <= 'z') ? str[i] - 32 : str[i];
    }

    if (strcmp(upper_str, "DEBUG") == 0)   return D_LOG_LEVEL_DEBUG;
    if (strcmp(upper_str, "INFO") == 0)    return D_LOG_LEVEL_INFO;
    if (strcmp(upper_str, "WARNING") == 0) return D_LOG_LEVEL_WARNING;
    if (strcmp(upper_str, "ERROR") == 0)   return D_LOG_LEVEL_ERROR;
    if (strcmp(upper_str, "FATAL") == 0)   return D_LOG_LEVEL_FATAL;
    if (strcmp(upper_str, "OFF") == 0)     return D_LOG_LEVEL_OFF;

    return D_LOG_LEVEL_INFO;  // Default
}

/*
 * Get ANSI color code for log level (enhanced with gaming theme)
 */
const char* d_LogLevel_GetColor(dLogLevel_t level)
{
    // Detect color support once
    if (!g_color_support_detected) {
        g_supports_color = is_color_terminal();
        g_color_support_detected = true;
    }

    if (!g_supports_color) return "";

    switch (level) {
        case D_LOG_LEVEL_DEBUG:   return LOG_COLOR_DEBUG_PREFIX;   // Cyan + Bold
        case D_LOG_LEVEL_INFO:    return LOG_COLOR_INFO_PREFIX;    // Green + Bold
        case D_LOG_LEVEL_WARNING: return LOG_COLOR_WARNING_PREFIX; // Yellow + Bold
        case D_LOG_LEVEL_ERROR:   return LOG_COLOR_ERROR_PREFIX;   // Red + Bold
        case D_LOG_LEVEL_FATAL:   return LOG_COLOR_FATAL_PREFIX;   // Magenta + Bold + Blink
        default:                  return "";                       // No color
    }
}

/*
 * Get color reset sequence
 */
const char* d_LogLevel_GetColorReset(void)
{
    if (!g_color_support_detected) {
        g_supports_color = is_color_terminal();
        g_color_support_detected = true;
    }

    return g_supports_color ? LOG_COLOR_RESET : "";
}

/*
 * Get gaming-themed prefix for log level
 */
const char* d_LogLevel_GetGamePrefix(dLogLevel_t level)
{
    switch (level) {
        case D_LOG_LEVEL_DEBUG:   return "[DEBUG]";
        case D_LOG_LEVEL_INFO:    return "[INFO] ";
        case D_LOG_LEVEL_WARNING: return "[WARN] ";
        case D_LOG_LEVEL_ERROR:   return "[ERROR]";
        case D_LOG_LEVEL_FATAL:   return "[FATAL]";
        default:                  return "[LOG]  ";
    }
}

/*
 * Get current timestamp as Unix time
 */
double d_GetTimestamp(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
    } else {
        // Fallback to time() if clock_gettime fails
        return (double)time(NULL);
    }
}

/*
 * Format timestamp into string with enhanced styling
 */
void d_FormatTimestamp(char* buffer, size_t buffer_size, double timestamp, const char* format)
{
    if (!buffer || buffer_size == 0) return;

    time_t seconds = (time_t)timestamp;
    int milliseconds = (int)((timestamp - seconds) * 1000);

    struct tm* tm_info = localtime(&seconds);
    if (!tm_info) {
        snprintf(buffer, buffer_size, "INVALID_TIME");
        return;
    }

    if (!format) {
        // Enhanced default format with milliseconds and gaming style
        size_t len = strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
        if (len > 0 && len < buffer_size - 5) {
            snprintf(buffer + len, buffer_size - len, ".%03d", milliseconds);
        }
    } else {
        strftime(buffer, buffer_size, format, tm_info);
    }
}

// =============================================================================
// THREAD-LOCAL BUFFER MANAGEMENT
// =============================================================================

/*
 * Get or create thread-local format buffer
 */
static dString_t* get_tls_buffer(void)
{
    if (!tls_format_buffer) {
        tls_format_buffer = d_InitString();
    } else {
        d_ClearString(tls_format_buffer);
    }
    return tls_format_buffer;
}

// =============================================================================
// ENHANCED CONSOLE OUTPUT HANDLER
// =============================================================================

/*
 * Enhanced console log handler with gaming-style formatting
 */
static void enhanced_console_handler(const dLogEntry_t* entry, void* user_data)
{
    (void)user_data; // Unused parameter

    if (!entry || !entry->message) return;

    dString_t* output = d_InitString();

    // Get colors
    const char* level_color = d_LogLevel_GetColor(entry->level);
    const char* reset_color = d_LogLevel_GetColorReset();

    // Build the output with enhanced formatting

    // Add timestamp if enabled
    dLogger_t* logger = g_global_logger;
    if (logger && logger->config.include_timestamp) {
        char timestamp_buf[64];
        d_FormatTimestamp(timestamp_buf, sizeof(timestamp_buf), entry->timestamp, logger->config.timestamp_format);

        d_AppendString(output, LOG_COLOR_DIM, 0);
        d_AppendString(output, "[", 0);
        d_AppendString(output, timestamp_buf, 0);
        d_AppendString(output, "]", 0);
        d_AppendString(output, reset_color, 0);
        d_AppendString(output, " ", 0);
    }

    // Add colored level with gaming prefix
    if (logger && logger->config.colorize_output) {
        d_AppendString(output, level_color, 0);
        d_AppendString(output, d_LogLevel_GetGamePrefix(entry->level), 0);
        d_AppendString(output, reset_color, 0);
    } else {
        // Fallback to simple level
        d_AppendString(output, "[", 0);
        d_AppendString(output, d_LogLevel_ToString(entry->level), 0);
        d_AppendString(output, "]", 0);
    }

    // Add thread ID if available (using your gray palette)
    if (entry->thread_id != 0) {
        if (g_supports_color) {
            d_AppendString(output, LOG_COLOR_GRAY_LIGHT LOG_COLOR_DIM, 0);
        }
        d_AppendString(output, " {", 0);

        char thread_buf[16];
        snprintf(thread_buf, sizeof(thread_buf), "%u", entry->thread_id);
        d_AppendString(output, thread_buf, 0);

        d_AppendString(output, "}", 0);
        if (g_supports_color) {
            d_AppendString(output, reset_color, 0);
        }
    }

    // Add file/line info if enabled
    if (logger && logger->config.include_file_info && entry->file) {
        d_AppendString(output, LOG_COLOR_DIM, 0);
        d_AppendString(output, " (", 0);
        d_AppendString(output, entry->file, 0);
        if (entry->line > 0) {
            char line_buf[16];
            snprintf(line_buf, sizeof(line_buf), ":%d", entry->line);
            d_AppendString(output, line_buf, 0);
        }
        d_AppendString(output, ")", 0);
        d_AppendString(output, reset_color, 0);
    }

    // Add function name if enabled
    if (logger && logger->config.include_function && entry->function) {
        d_AppendString(output, LOG_COLOR_DIM, 0);
        d_AppendString(output, " [", 0);
        d_AppendString(output, entry->function, 0);
        d_AppendString(output, "]", 0);
        d_AppendString(output, reset_color, 0);
    }

    // Add context if available (using your blue accent palette)
    if (entry->context) {
        if (g_supports_color) {
            d_AppendString(output, LOG_COLOR_BLUE_LIGHTER LOG_COLOR_BOLD, 0);
        }
        d_AppendString(output, " {", 0);
        d_AppendString(output, entry->context, 0);
        d_AppendString(output, "}", 0);
        if (g_supports_color) {
            d_AppendString(output, reset_color, 0);
        }
    }

    // Add separator and message
    d_AppendString(output, " ", 0);
    d_AppendString(output, d_PeekString(entry->message), 0);
    d_AppendString(output, "\n", 0);

    // Output to console
    printf("%s", d_PeekString(output));

    // For errors and fatals, also output to stderr
    if (entry->level >= D_LOG_LEVEL_ERROR) {
        fprintf(stderr, "%s", d_PeekString(output));
    }

    d_DestroyString(output);
}

// =============================================================================
// LOGGER MANAGEMENT
// =============================================================================

/*
 * Create a new logger with the specified configuration
 */
dLogger_t* d_CreateLogger(dLogConfig_t config)
{
    dLogger_t* logger = (dLogger_t*)calloc(1, sizeof(dLogger_t));
    if (!logger) return NULL;

    logger->config = config;
    logger->handlers = d_InitArray(4, sizeof(dLogHandlerReg_t));
    logger->contexts = d_InitArray(8, sizeof(char*));
    logger->format_buffer = d_InitString();
    logger->stats = (dLogStats_t*)calloc(1, sizeof(dLogStats_t));

    // Initialize mutex for thread safety
    logger->mutex = malloc(sizeof(dMutex_t));
    if (logger->mutex) {
        MUTEX_INIT((dMutex_t*)logger->mutex);
    }

    if (!logger->handlers || !logger->contexts || !logger->format_buffer || !logger->stats) {
        d_DestroyLogger(logger);
        return NULL;
    }

    // Set default configuration values
    if (!logger->config.timestamp_format) {
        logger->config.timestamp_format = "%Y-%m-%d %H:%M:%S";
    }
    if (!logger->config.context_separator) {
        logger->config.context_separator = "::";
    }

    // Add default console handler if none specified
    if (logger->handlers->count == 0) {
        d_AddLogHandler(logger, enhanced_console_handler, NULL);
    }

    return logger;
}

/*
 * Destroy a logger and free all associated resources
 */
void d_DestroyLogger(dLogger_t* logger)
{
    if (!logger) return;

    // If this is the global logger, unset it
    if (logger == g_global_logger) {
        g_global_logger = NULL;
    }

    // Free all resources
    if (logger->handlers) d_DestroyArray(logger->handlers);
    if (logger->contexts) d_DestroyArray(logger->contexts);
    if (logger->format_buffer) d_DestroyString(logger->format_buffer);
    if (logger->stats) free(logger->stats);
    if (logger->filters) {
        // Clean up filter engine with thread safety
        if (logger->mutex) {
            MUTEX_LOCK((dMutex_t*)logger->mutex);
        }
        if (logger->filters->rules) {
            d_DestroyArray(logger->filters->rules);
        }
        free(logger->filters);
        if (logger->mutex) {
            MUTEX_UNLOCK((dMutex_t*)logger->mutex);
        }
    }
    if (logger->mutex) {
        MUTEX_DESTROY((dMutex_t*)logger->mutex);
        free(logger->mutex);
    }

    free(logger);
}

/*
 * Set a logger as the global default logger
 */
void d_SetGlobalLogger(dLogger_t* logger)
{
    g_global_logger = logger;
    if (logger) {
        logger->is_global = true;
    }
}

/*
 * Get the current global logger
 */
dLogger_t* d_GetGlobalLogger(void)
{
    return g_global_logger;
}

/*
 * Set the minimum log level for a logger
 */
void d_SetLogLevel(dLogger_t* logger, dLogLevel_t level)
{
    if (!logger) {
        logger = g_global_logger;
    }

    if (logger) {
        logger->config.default_level = level;
    } else {
        // Set global default
        g_log_config.default_level = level;
    }
}

/*
 * Enable or disable logging globally
 */
void d_SetLoggingEnabled(bool enabled)
{
    g_log_config.logging_enabled = enabled;
}

/*
 * Check if logging is globally enabled
 */
bool d_IsLoggingEnabled(void)
{
    return g_log_config.logging_enabled;
}

/*
 * Get the current log level for a logger
 */
dLogLevel_t d_GetLogLevel(dLogger_t* logger)
{
    if (!logger) {
        logger = g_global_logger;
    }

    if (logger) {
        return logger->config.default_level;
    } else {
        return g_log_config.default_level;
    }
}

// =============================================================================
// LOG HANDLER MANAGEMENT
// =============================================================================

/*
 * Add a log handler to a logger
 */
void d_AddLogHandler(dLogger_t* logger, dLogHandler_t handler, void* user_data)
{
    if (!logger) {
        logger = g_global_logger;
    }

    if (!logger || !handler) return;

    // Thread-safe handler modification
    if (logger->mutex) {
        MUTEX_LOCK((dMutex_t*)logger->mutex);
    }

    dLogHandlerReg_t reg = {
        .handler = handler,
        .user_data = user_data,
        .min_level = D_LOG_LEVEL_DEBUG  // Default to all levels
    };

    d_AppendArray(logger->handlers, &reg);

    if (logger->mutex) {
        MUTEX_UNLOCK((dMutex_t*)logger->mutex);
    }
}

/*
 * Remove a log handler from a logger
 */
void d_RemoveLogHandler(dLogger_t* logger, dLogHandler_t handler)
{
    if (!logger) {
        logger = g_global_logger;
    }

    if (!logger || !handler || !logger->handlers) return;

    // Thread-safe handler modification
    if (logger->mutex) {
        MUTEX_LOCK((dMutex_t*)logger->mutex);
    }

    // Search for the handler and remove it
    for (size_t i = 0; i < logger->handlers->count; i++) {
        dLogHandlerReg_t* reg = (dLogHandlerReg_t*)d_GetDataFromArrayByIndex(logger->handlers, i);
        if (reg && reg->handler == handler) {
            // Found the handler - remove it by shifting remaining elements
            for (size_t j = i; j < logger->handlers->count - 1; j++) {
                dLogHandlerReg_t* current = (dLogHandlerReg_t*)d_GetDataFromArrayByIndex(logger->handlers, j);
                dLogHandlerReg_t* next = (dLogHandlerReg_t*)d_GetDataFromArrayByIndex(logger->handlers, j + 1);
                if (current && next) {
                    *current = *next;
                }
            }
            logger->handlers->count--;
            if (logger->mutex) {
                MUTEX_UNLOCK((dMutex_t*)logger->mutex);
            }
            return;
        }
    }

    if (logger->mutex) {
        MUTEX_UNLOCK((dMutex_t*)logger->mutex);
    }
}

// =============================================================================
// CORE LOGGING ENGINE
// =============================================================================

/*
 * Internal function to process a log entry through all handlers
 */
static void process_log_entry(dLogger_t* logger, dLogEntry_t* entry)
{
    if (!logger || !entry) return;

    // Thread-safe logging with mutex
    if (logger->mutex) {
        MUTEX_LOCK((dMutex_t*)logger->mutex);
    }

    // Update statistics
    g_log_config.total_logs_processed++;

    // Process through each handler
    for (size_t i = 0; i < logger->handlers->count; i++) {
        dLogHandlerReg_t* reg = (dLogHandlerReg_t*)d_GetDataFromArrayByIndex(logger->handlers, i);
        if (reg && reg->handler && entry->level >= reg->min_level) {
            reg->handler(entry, reg->user_data);
        }
    }

    if (logger->mutex) {
        MUTEX_UNLOCK((dMutex_t*)logger->mutex);
    }
}

/*
 * Log with explicit file/line/function information
 */
void d_LogEx(dLogLevel_t level, const char* file, int line, const char* func, const char* message)
{
    if (!g_log_config.logging_enabled) return;

    dLogger_t* logger = g_global_logger;
    if (!logger) return;

    // Check if this level should be logged
    if (level < logger->config.default_level) {
        g_log_config.total_logs_suppressed++;
        update_log_stats(logger, level, 0.0, true, false, false);
        return;
    }

    // Start performance timing
    double start_time = d_GetTimestamp();

    // Create log entry
    dLogEntry_t entry = {
        .level = level,
        .file = file,
        .line = line,
        .function = func,
        .timestamp = start_time,
        .context = get_current_context_name(),
        .thread_id = GET_THREAD_ID()
    };

    // Create a temporary string for the message (don't use TLS buffer)
    dString_t* msg_buffer = d_InitString();
    d_AppendString(msg_buffer, message, 0);
    entry.message = msg_buffer;

    // Process the entry
    process_log_entry(logger, &entry);

    // Clean up the temporary string
    d_DestroyString(msg_buffer);

    // Update performance stats
    double processing_time = d_GetTimestamp() - start_time;
    update_log_stats(logger, level, processing_time, false, false, false);
}

/*
 * Simple logging function that uses global logger
 * Log with explicit file/line/function and formatting
 */
void d_LogExF(dLogLevel_t level, const char* file, int line, const char* func, const char* format, ...)
{
    if (!g_log_config.logging_enabled) return;

    dLogger_t* logger = g_global_logger;
    if (!logger) return;

    // Check if this level should be logged
    if (level < logger->config.default_level) {
        g_log_config.total_logs_suppressed++;
        update_log_stats(logger, level, 0.0, true, false, false);
        return;
    }

    // Start performance timing
    double start_time = d_GetTimestamp();

    // Format the message
    dString_t* msg_buffer = get_tls_buffer();

    va_list args;
    va_start(args, format);

    // First, determine required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        // Ensure buffer has space
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
    }

    va_end(args);

    // Create and process log entry
    dLogEntry_t entry = {
        .message = msg_buffer,
        .level = level,
        .file = file,
        .line = line,
        .function = func,
        .timestamp = start_time,
        .context = get_current_context_name(),
        .thread_id = GET_THREAD_ID()
    };

    process_log_entry(logger, &entry);

    // Update performance stats
    double processing_time = d_GetTimestamp() - start_time;
    update_log_stats(logger, level, processing_time, false, false, false);
}

// =============================================================================
// SIMPLE LOGGING FUNCTIONS
// =============================================================================

/*
 * Log a message at the specified level
 */
void d_Log(dLogLevel_t level, const char* message)
{
    d_LogEx(level, NULL, 0, NULL, message);
}

/*
 * Log a formatted message at the specified level
 */
void d_LogF(dLogLevel_t level, const char* format, ...)
{
    if (!g_log_config.logging_enabled) return;

    dLogger_t* logger = g_global_logger;
    if (!logger || level < logger->config.default_level) return;

    // Format the message
    va_list args;
    va_start(args, format);

    dString_t* msg_buffer = get_tls_buffer();

    // Determine required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
    }

    va_end(args);

    d_LogEx(level, NULL, 0, NULL, d_PeekString(msg_buffer));
}

// Convenience functions
void d_LogDebug(const char* message)   { d_Log(D_LOG_LEVEL_DEBUG, message); }
void d_LogInfo(const char* message)    { d_Log(D_LOG_LEVEL_INFO, message); }
void d_LogWarning(const char* message) { d_Log(D_LOG_LEVEL_WARNING, message); }
void d_LogError(const char* message)   { d_Log(D_LOG_LEVEL_ERROR, message); }
void d_LogFatal(const char* message)   { d_Log(D_LOG_LEVEL_FATAL, message); }

// Formatted convenience functions - fixed variadic implementation
void d_LogDebugF(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    dString_t* msg_buffer = get_tls_buffer();
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
        d_Log(D_LOG_LEVEL_DEBUG, d_PeekString(msg_buffer));
    }

    va_end(args);
}

void d_LogInfoF(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    dString_t* msg_buffer = get_tls_buffer();
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
        d_Log(D_LOG_LEVEL_INFO, d_PeekString(msg_buffer));
    }

    va_end(args);
}

void d_LogWarningF(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    dString_t* msg_buffer = get_tls_buffer();
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
        d_Log(D_LOG_LEVEL_WARNING, d_PeekString(msg_buffer));
    }

    va_end(args);
}

void d_LogErrorF(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    dString_t* msg_buffer = get_tls_buffer();
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
        d_Log(D_LOG_LEVEL_ERROR, d_PeekString(msg_buffer));
    }

    va_end(args);
}

void d_LogFatalF(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    dString_t* msg_buffer = get_tls_buffer();
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char temp[needed + 1];
        va_copy(args_copy, args);
        vsnprintf(temp, sizeof(temp), format, args_copy);
        va_end(args_copy);
        d_AppendString(msg_buffer, temp, 0);
        d_Log(D_LOG_LEVEL_FATAL, d_PeekString(msg_buffer));
    }

    va_end(args);
}
// =============================================================================
// STATISTICS AND MONITORING
// =============================================================================

static dLogStats_t global_log_stats = {0};
static bool stats_initialized = false;

void d_GetLogStats(dLogger_t* logger, dLogStats_t* stats) {
    if (!stats) return;

    if (logger && logger->stats) {
        // Per-logger stats with thread safety
        if (logger->mutex) {
            MUTEX_LOCK((dMutex_t*)logger->mutex);
        }
        *stats = *logger->stats;
        if (logger->mutex) {
            MUTEX_UNLOCK((dMutex_t*)logger->mutex);
        }
    } else if (logger) {
        // Logger exists but no stats allocated - return zero stats
        memset(stats, 0, sizeof(dLogStats_t));
    } else {
        // Global stats
        *stats = global_log_stats;
    }
}

void d_ResetLogStats(dLogger_t* logger) {
    if (logger && logger->stats) {
        // Per-logger stats reset with thread safety
        if (logger->mutex) {
            MUTEX_LOCK((dMutex_t*)logger->mutex);
        }
        memset(logger->stats, 0, sizeof(dLogStats_t));
        if (logger->mutex) {
            MUTEX_UNLOCK((dMutex_t*)logger->mutex);
        }
    } else if (logger) {
        // Logger exists but no stats - do nothing
        return;
    } else {
        // Global stats reset
        memset(&global_log_stats, 0, sizeof(dLogStats_t));
        stats_initialized = false;
    }
}

// Internal function to update stats - integrated into logging system
static void update_log_stats(dLogger_t* logger, dLogLevel_t level, double processing_time, bool was_suppressed, bool was_rate_limited, bool handler_error) {
    // Update global stats
    if (!stats_initialized) {
        memset(&global_log_stats, 0, sizeof(dLogStats_t));
        stats_initialized = true;
    }

    global_log_stats.total_log_time += processing_time;
    if (level < D_LOG_LEVEL_OFF && level >= 0) {
        global_log_stats.logs_by_level[level]++;
    }

    if (was_suppressed) {
        global_log_stats.logs_suppressed++;
    }

    if (was_rate_limited) {
        global_log_stats.logs_rate_limited++;
    }

    if (handler_error) {
        global_log_stats.handler_errors++;
    }

    // Update per-logger stats if logger exists and has stats
    if (logger && logger->stats) {
        if (logger->mutex) {
            MUTEX_LOCK((dMutex_t*)logger->mutex);
        }

        logger->stats->total_log_time += processing_time;
        if (level < D_LOG_LEVEL_OFF && level >= 0) {
            logger->stats->logs_by_level[level]++;
        }

        if (was_suppressed) {
            logger->stats->logs_suppressed++;
        }

        if (was_rate_limited) {
            logger->stats->logs_rate_limited++;
        }

        if (handler_error) {
            logger->stats->handler_errors++;
        }

        if (logger->mutex) {
            MUTEX_UNLOCK((dMutex_t*)logger->mutex);
        }
    }
}

/*
 * Push a new logging context onto the stack
 */
// =============================================================================
// ENHANCED RATE LIMITING WITH STATS
// =============================================================================


// Enhanced rate limiting that properly tracks statistics
void d_LogRateLimited_Enhanced(dLogLevel_t level, uint32_t max_count, double time_window, const char* message) {
    if (!message) return;

    init_rate_limit_cache();

    uint32_t msg_hash = hash_message(message);
    double current_time = d_GetTimestamp();
    bool was_rate_limited = false;

    // Find existing rate limit entry
    dLogRateLimit_t* rate_limit = NULL;
    for (size_t i = 0; i < rate_limit_cache->count; i++) {
        dLogRateLimit_t* entry = (dLogRateLimit_t*)d_GetDataFromArrayByIndex(rate_limit_cache, i);
        if (entry->message_hash == msg_hash) {
            rate_limit = entry;
            break;
        }
    }

    // Create new entry if not found
    if (!rate_limit) {
        dLogRateLimit_t new_entry = {
            .message_hash = msg_hash,
            .count = 0,
            .max_count = max_count,
            .time_window = time_window,
            .first_log_time = current_time,
            .last_log_time = current_time
        };
        d_AppendArray(rate_limit_cache, &new_entry);
        rate_limit = (dLogRateLimit_t*)d_GetDataFromArrayByIndex(rate_limit_cache,
                                                                rate_limit_cache->count - 1);
    }

    // Check if we're still in the time window
    // Check if we should reset the time window
    if (current_time - rate_limit->first_log_time > time_window) {
        rate_limit->count = 0;
        rate_limit->first_log_time = current_time;
    }

    if (rate_limit->count < max_count) {
        double start_time = d_GetTimestamp();
        d_Log(level, message);
        double processing_time = d_GetTimestamp() - start_time;

        rate_limit->count++;
        rate_limit->last_log_time = current_time;

        // Update stats without rate limiting flag
        update_log_stats(g_global_logger, level, processing_time, false, false, false);
    } else {
        // Rate limit less aggressively - only suppress after hitting limit multiple times
        if (rate_limit->count == max_count) {
            d_LogWarningF("⚠️  Rate limiting activated for message hash %u (max %u per %.1fs)",
                         msg_hash, max_count, time_window);
        }
        rate_limit->count++;

        // Allow some messages through even when rate limited (every 10th message)
        if (rate_limit->count % 10 == 0) {
            d_LogWarningF("🚫 Rate limited message (suppressed %u times): %s",
                         rate_limit->count - max_count, message);
        }

        // Log was rate limited
        was_rate_limited = true;
        update_log_stats(g_global_logger, level, 0.0, false, was_rate_limited, false);
    }
}

// =============================================================================
// LOG FILTER BUILDER SYSTEM
// =============================================================================

dLogFilterBuilder_t* d_CreateFilterBuilder() {
    dLogFilterBuilder_t* builder = malloc(sizeof(dLogFilterBuilder_t));
    if (!builder) return NULL;

    builder->rules = d_InitArray(50, sizeof(dLogFilterRule_t));
    builder->next_priority = 1;

    return builder;
}

void d_FilterBuilder_AddDirectory(dLogFilterBuilder_t* builder, const char* path, dLogLevel_t level) {
    if (!builder || !path) return;

    dLogFilterRule_t rule = {
        .type = D_LOG_FILTER_DIRECTORY,
        .pattern = strdup(path),
        .pattern_len = strlen(path),
        .level = level,
        .priority = builder->next_priority++,
        .pattern_hash = hash_message(path),
        .recursive = true // Default to recursive for directory filtering
    };

    d_AppendArray(builder->rules, &rule);
}

void d_FilterBuilder_AddPrefix(dLogFilterBuilder_t* builder, const char* prefix, dLogLevel_t level) {
    if (!builder || !prefix) return;

    dLogFilterRule_t rule = {
        .type = D_LOG_FILTER_PREFIX,
        .pattern = strdup(prefix),
        .pattern_len = strlen(prefix),
        .level = level,
        .priority = builder->next_priority++,
        .pattern_hash = hash_message(prefix),
        .recursive = false
    };

    d_AppendArray(builder->rules, &rule);
}

void d_FilterBuilder_AddSuffix(dLogFilterBuilder_t* builder, const char* suffix, dLogLevel_t level) {
    if (!builder || !suffix) return;

    dLogFilterRule_t rule = {
        .type = D_LOG_FILTER_SUFFIX,
        .pattern = strdup(suffix),
        .pattern_len = strlen(suffix),
        .level = level,
        .priority = builder->next_priority++,
        .pattern_hash = hash_message(suffix),
        .recursive = false
    };

    d_AppendArray(builder->rules, &rule);
}

void d_FilterBuilder_Apply(dLogger_t* logger, dLogFilterBuilder_t* builder) {
    if (!builder) return;

    // Use global logger if none provided
    if (!logger) {
        logger = d_GetGlobalLogger();
    }

    if (!logger) return;

    // Create filter engine if it doesn't exist
    if (!logger->filters) {
        logger->filters = malloc(sizeof(dLogFilterEngine_t));
        if (!logger->filters) return;

        logger->filters->rules = d_InitArray(builder->rules->count, sizeof(dLogFilterRule_t));
        // Initialize other fields that exist in the struct
        logger->filters->cache_hits = 0;
        logger->filters->cache_misses = 0;
    }

    // Copy rules to logger's filter engine
    for (size_t i = 0; i < builder->rules->count; i++) {
        dLogFilterRule_t* rule = (dLogFilterRule_t*)d_GetDataFromArrayByIndex(builder->rules, i);
        d_AppendArray(logger->filters->rules, rule);
    }
}

int d_FilterBuilder_FromString(dLogger_t* logger, const char* config_str) {
    if (!config_str) return -1;

    // Use global logger if none provided
    if (!logger) {
        logger = d_GetGlobalLogger();
    }

    if (!logger) return -1;

    // Create a temporary builder
    dLogFilterBuilder_t* builder = d_CreateFilterBuilder();
    if (!builder) return -1;

    // Simple parser for filter strings like "src/*:DEBUG,tests/*:INFO"
    char* filter_copy = strdup(config_str);
    char* token = strtok(filter_copy, ",");
    int rules_added = 0;

    while (token) {
        char* colon = strchr(token, ':');
        if (colon) {
            *colon = '\0';
            char* pattern = token;
            char* level_str = colon + 1;

            dLogLevel_t level = d_LogLevel_FromString(level_str);
            if (level != D_LOG_LEVEL_OFF) {
                if (strstr(pattern, "*") || strstr(pattern, "/")) {
                    d_FilterBuilder_AddDirectory(builder, pattern, level);
                } else {
                    d_FilterBuilder_AddPrefix(builder, pattern, level);
                }
                rules_added++;
            }
        }
        token = strtok(NULL, ",");
    }

    // Apply the rules
    d_FilterBuilder_Apply(logger, builder);

    // Cleanup
    d_DestroyFilterBuilder(builder);
    free(filter_copy);

    return rules_added;
}

void d_DestroyFilterBuilder(dLogFilterBuilder_t* builder) {
    if (!builder) return;

    // Free all pattern strings
    for (size_t i = 0; i < builder->rules->count; i++) {
        dLogFilterRule_t* rule = (dLogFilterRule_t*)d_GetDataFromArrayByIndex(builder->rules, i);
        if (rule->pattern) {
            free((void*)rule->pattern);
        }
    }

    d_DestroyArray(builder->rules);
    free(builder);
}


// =============================================================================
// STRUCTURED LOGGING SYSTEM
// =============================================================================

typedef struct {
    char* key;
    char* value;
} dLogField_t;

dLogStructured_t* d_LogStructured(dLogLevel_t level) {
    dLogStructured_t* structured = malloc(sizeof(dLogStructured_t));
    if (!structured) return NULL;

    // Initialize base builder
    structured->base.buffer = d_InitString();
    structured->base.level = level;
    structured->base.logger = d_GetGlobalLogger();
    structured->base.file = NULL;
    structured->base.line = 0;
    structured->base.function = NULL;
    structured->base.committed = false;

    // Initialize structured fields
    structured->fields = d_InitArray(20, sizeof(dLogField_t));
    structured->in_json_mode = false;

    return structured;
}

dLogStructured_t* d_LogStructured_Field(dLogStructured_t* structured, const char* key, const char* value) {
    if (!structured || !key || !value || structured->base.committed) return structured;

    dLogField_t field = {
        .key = strdup(key),
        .value = strdup(value)
    };

    d_AppendArray(structured->fields, &field);
    return structured;
}

dLogStructured_t* d_LogStructured_FieldInt(dLogStructured_t* structured, const char* key, int value) {
    if (!structured || !key || structured->base.committed) return structured;

    char value_str[32];
    snprintf(value_str, sizeof(value_str), "%d", value);

    return d_LogStructured_Field(structured, key, value_str);
}

dLogStructured_t* d_LogStructured_FieldFloat(dLogStructured_t* structured, const char* key, float value, int decimals) {
    if (!structured || !key || structured->base.committed) return structured;

    char value_str[32];
    snprintf(value_str, sizeof(value_str), "%.*f", decimals, value);

    return d_LogStructured_Field(structured, key, value_str);
}

dLogStructured_t* d_LogStructured_SetFormat(dLogStructured_t* structured, bool json_format) {
    if (!structured || structured->base.committed) return structured;

    structured->in_json_mode = json_format;
    return structured;
}

void d_LogStructured_Commit(dLogStructured_t* structured) {
    if (!structured || structured->base.committed) return;

    // Ensure we have fields to process
    if (!structured->fields || structured->fields->count == 0) {
        d_LogWarning("🔍 [DEBUG] Structured log commit called with no fields");
        structured->base.committed = true;
        return;
    }

    // Build the structured message
    if (structured->in_json_mode) {
        d_AppendString(structured->base.buffer, "{", 0);
        for (size_t i = 0; i < structured->fields->count; i++) {
            dLogField_t* field = (dLogField_t*)d_GetDataFromArrayByIndex(structured->fields, i);
            if (!field || !field->key || !field->value) continue;
            if (i > 0) d_AppendString(structured->base.buffer, ",", 0);
            d_AppendString(structured->base.buffer, "\"", 0);
            d_AppendString(structured->base.buffer, field->key, 0);
            d_AppendString(structured->base.buffer, "\":\"", 0);
            d_AppendString(structured->base.buffer, field->value, 0);
            d_AppendString(structured->base.buffer, "\"", 0);
        }
        d_AppendString(structured->base.buffer, "}", 0);
    } else {
        // Key-value format
        for (size_t i = 0; i < structured->fields->count; i++) {
            dLogField_t* field = (dLogField_t*)d_GetDataFromArrayByIndex(structured->fields, i);
            if (!field || !field->key || !field->value) continue;
            if (i > 0) d_AppendString(structured->base.buffer, " ", 0);
            d_AppendString(structured->base.buffer, field->key, 0);
            d_AppendString(structured->base.buffer, "=", 0);
            d_AppendString(structured->base.buffer, field->value, 0);
        }
    }

    // Add debug indicator for structured logs
    const char* format_type = structured->in_json_mode ? "JSON" : "KV";

    // Commit the log using the correct d_LogEx signature
    d_LogEx(structured->base.level, structured->base.file, structured->base.line,
            structured->base.function, d_PeekString(structured->base.buffer));

    structured->base.committed = true;

    // Cleanup
    for (size_t i = 0; i < structured->fields->count; i++) {
        dLogField_t* field = (dLogField_t*)d_GetDataFromArrayByIndex(structured->fields, i);
        free(field->key);
        free(field->value);
    }

    d_DestroyArray(structured->fields);
    d_DestroyString(structured->base.buffer);
    free(structured);
}

dLogStructured_t* d_LogStructured_FieldBool(dLogStructured_t* structured, const char* key, bool value) {
    if (!structured || !key || structured->base.committed) return structured;

    const char* value_str = value ? "true" : "false";
    return d_LogStructured_Field(structured, key, value_str);
}

dLogStructured_t* d_LogStructured_FieldTimestamp(dLogStructured_t* structured, const char* key) {
    if (!structured || !key || structured->base.committed) return structured;

    // Get current timestamp
    double timestamp = d_GetTimestamp();

    // Convert to ISO 8601 format
    time_t time_secs = (time_t)timestamp;
    int milliseconds = (int)((timestamp - time_secs) * 1000);

    struct tm* tm_info = gmtime(&time_secs);
    char timestamp_str[32];
    snprintf(timestamp_str, sizeof(timestamp_str),
             "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec,
             milliseconds);

    return d_LogStructured_Field(structured, key, timestamp_str);
}

dLogStructured_t* d_LogStructured_Clone(dLogStructured_t* source) {
    if (!source || source->base.committed) return NULL;

    // Create new structured log with same level
    dLogStructured_t* clone = d_LogStructured(source->base.level);
    if (!clone) return NULL;

    // Copy format setting
    clone->in_json_mode = source->in_json_mode;

    // Copy all fields from source
    if (source->fields && source->fields->count > 0) {
        for (size_t i = 0; i < source->fields->count; i++) {
            dLogField_t* field = (dLogField_t*)d_GetDataFromArrayByIndex(source->fields, i);
            if (field && field->key && field->value) {
                d_LogStructured_Field(clone, field->key, field->value);
            }
        }
    }

    return clone;
}

// =============================================================================
// BUILT-IN LOG HANDLERS
// =============================================================================

/*
 * Console log handler - outputs to stdout/stderr with color support
 */
void d_ConsoleLogHandler(const dLogEntry_t* entry, void* user_data) {
    (void)user_data; // Suppress unused parameter warning

    if (!entry || !entry->message) return;

    // Use the enhanced console handler that's already implemented
    enhanced_console_handler(entry, user_data);
}

/*
 * File log handler - outputs to a file
 */
void d_FileLogHandler(const dLogEntry_t* entry, void* user_data) {
    if (!entry || !entry->message || !user_data) return;

    FILE* file = (FILE*)user_data;

    // Format timestamp if needed
    char time_buf[64] = "";
    if (g_global_logger && g_global_logger->config.include_timestamp) {
        d_FormatTimestamp(time_buf, sizeof(time_buf), entry->timestamp,
                         g_global_logger->config.timestamp_format);

    }

    // Write to file
    if (strlen(time_buf) > 0) {
        fprintf(file, "[%s] ", time_buf);
    }

    fprintf(file, "[%s] %s\n",
            d_LogLevel_ToString(entry->level),
            d_PeekString(entry->message));

    fflush(file); // Ensure immediate write
}

/*
 * String log handler - appends to a dString_t buffer
 */
void d_StringLogHandler(const dLogEntry_t* entry, void* user_data) {
    if (!entry || !entry->message || !user_data) return;

    dString_t* buffer = (dString_t*)user_data;

    // Format timestamp if needed
    char time_buf[64] = "";
    if (g_global_logger && g_global_logger->config.include_timestamp) {
        d_FormatTimestamp(time_buf, sizeof(time_buf), entry->timestamp,
                         g_global_logger->config.timestamp_format);

    }

    // Append to string buffer
    if (strlen(time_buf) > 0) {
        d_AppendString(buffer, "[", 0);
        d_AppendString(buffer, time_buf, 0);
        d_AppendString(buffer, "] ", 0);
    }

    d_AppendString(buffer, "[", 0);
    d_AppendString(buffer, d_LogLevel_ToString(entry->level), 0);
    d_AppendString(buffer, "] ", 0);
    d_AppendString(buffer, d_PeekString(entry->message), 0);
    d_AppendString(buffer, "\n", 0);
}

// =============================================================================
// LOG BUILDER PATTERN IMPLEMENTATION
// =============================================================================

/*
 * Begin building a log message
 */
dLogBuilder_t* d_LogBegin(dLogLevel_t level) {
    if (!g_log_config.logging_enabled) return NULL;

    dLogger_t* logger = g_global_logger;
    if (!logger || level < logger->config.default_level) return NULL;

    dLogBuilder_t* builder = malloc(sizeof(dLogBuilder_t));
    if (!builder) return NULL;

    builder->buffer = d_InitString();
    if (!builder->buffer) {
        free(builder);
        return NULL;
    }

    builder->level = level;
    builder->logger = logger;
    builder->file = NULL;
    builder->line = 0;
    builder->function = NULL;
    builder->committed = false;

    return builder;
}

/*
 * Append a string to the log builder
 */
dLogBuilder_t* d_LogBuilder_Append(dLogBuilder_t* builder, const char* text) {
    if (!builder || !text) return builder;

    d_AppendString(builder->buffer, text, 0);
    return builder;
}

/*
 * Append an integer to the log builder
 */
dLogBuilder_t* d_LogBuilder_AppendInt(dLogBuilder_t* builder, int value) {
    if (!builder) return builder;

    d_AppendInt(builder->buffer, value);
    return builder;
}

/*
 * Append a float to the log builder
 */
dLogBuilder_t* d_LogBuilder_AppendFloat(dLogBuilder_t* builder, float value, int decimals) {
    if (!builder) return builder;

    d_AppendFloat(builder->buffer, value, decimals);
    return builder;
}

/*
 * Append formatted text to the log builder
 */
dLogBuilder_t* d_LogBuilder_AppendF(dLogBuilder_t* builder, const char* format, ...) {
    if (!builder || !format) return builder;

    va_list args;
    va_start(args, format);

    // Calculate required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char* temp_buffer = malloc(needed + 1);
        if (temp_buffer) {
            vsnprintf(temp_buffer, needed + 1, format, args);
            d_AppendString(builder->buffer, temp_buffer, 0);
            free(temp_buffer);
        }
    }

    va_end(args);
    return builder;
}

/*
 * End/commit the log builder
 */
void d_LogBuilder_End(dLogBuilder_t* builder) {
    if (!builder || builder->committed) return;

    // Log the message
    d_LogEx(builder->level, builder->file, builder->line, builder->function,
            d_PeekString(builder->buffer));

    builder->committed = true;

    // Cleanup
    d_DestroyString(builder->buffer);
    free(builder);
}

/*
 * Alias for d_LogBuilder_End()
 */
void d_LogBuilder_Commit(dLogBuilder_t* builder) {
    d_LogBuilder_End(builder);
}

// =============================================================================
// LOG CONTEXT IMPLEMENTATION
// =============================================================================

// Thread-local storage for context stack (simplified implementation)

/*
 * Push a new logging context onto the stack
 */
dLogContext_t* d_PushLogContext(const char* name) {
    if (!name) return NULL;

    dLogContext_t* context = malloc(sizeof(dLogContext_t));
    if (!context) return NULL;

    context->name = strdup(name);
    if (!context->name) {
        free(context);
        return NULL;
    }

    context->parent = g_context_stack;  // Link to current top
    context->logger = g_global_logger;
    context->start_time = d_GetTimestamp();

    // Push to stack
    g_context_stack = context;

    return context;
}

/*
 * Pop a logging context from the stack
 */
void d_PopLogContext(dLogContext_t* context) {
    if (!context) return;

    // Proper stack management - only pop if it's the current top
    if (g_context_stack == context) {
        g_context_stack = context->parent;  // Pop to parent
    } else {
        // Handle out-of-order pops by searching and removing from chain
        dLogContext_t* current = g_context_stack;
        dLogContext_t* prev = NULL;

        while (current && current != context) {
            prev = current;
            current = current->parent;
        }

        if (current == context) {
            if (prev) {
                prev->parent = context->parent;  // Skip this context
            } else {
                g_context_stack = context->parent;  // Was at top
            }
        }
    }

    free((void*)context->name);
    free(context);
}

/*
 * Enable timing for a logging context
 */
void d_LogContext_EnableTiming(dLogContext_t* context) {
    if (!context) return;

    // Update start time for timing
    context->start_time = d_GetTimestamp();
}

// =============================================================================
// CONDITIONAL LOGGING IMPLEMENTATION
// =============================================================================

/*
 * Log only if condition is true
 */
void d_LogIf(bool condition, dLogLevel_t level, const char* message) {
    if (condition) {
        d_Log(level, message);
    }
}

/*
 * Log with printf formatting only if condition is true
 */
void d_LogIfF(bool condition, dLogLevel_t level, const char* format, ...) {
    if (!condition || !format) return;

    va_list args;
    va_start(args, format);

    // Calculate required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        char* temp_buffer = malloc(needed + 1);
        if (temp_buffer) {
            vsnprintf(temp_buffer, needed + 1, format, args);
            d_Log(level, temp_buffer);
            free(temp_buffer);
        }
    }

    va_end(args);
}

// =============================================================================
// RATE LIMITED LOGGING
// =============================================================================

// Helper function to format a string with va_list, needed by the rate limiter.
// NOTE: This function should exist in your dLogs.c. If not, add it.
void d_FormatStringV(dString_t* sb, const char* format, va_list args) {
    if (!sb || !format) return;

    d_ClearString(sb); // Clear the buffer for fresh use

    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed >= 0) {
        // Ensure string builder has capacity. Your dString library should handle this.
        // For this example, we assume it can grow.
        if (sb->len + needed + 1 > sb->alloced) {
             sb->str = realloc(sb->str, sb->len + needed + 1);
             sb->alloced = sb->len + needed + 1;
        }
        vsnprintf(sb->str + sb->len, needed + 1, format, args);
        sb->len += needed;
    }
}


void d_ResetRateLimiterCache() {
    if (rate_limit_cache) {
        d_DestroyArray(rate_limit_cache);
        rate_limit_cache = NULL;
    }
}

size_t d_GetRateLimiterCacheEntryCount(void) {
    if (!rate_limit_cache) return 0;
    return rate_limit_cache->count;
}


void d_LogRateLimited(dLogLevel_t level, uint32_t max_count, double time_window, const char* message) {
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, level, max_count, time_window, "%s", message);
}

// In src/dLogs.c

void d_LogRateLimitedF(dLogRateLimitFlag_t flag, dLogLevel_t level, uint32_t max_count, double time_window, const char* format, ...) {
    if (!format) return;

    // The entire logic is one atomic, thread-safe operation.
    pthread_mutex_lock(&rate_limit_mutex);

    dLogger_t* logger = g_global_logger;
    if (!logger || level < logger->config.default_level) {
        pthread_mutex_unlock(&rate_limit_mutex);
        return;
    }

    init_rate_limit_cache();

    va_list args;
    va_start(args, format);

    uint32_t message_hash;
    dString_t* message_buffer = get_tls_buffer();

    if (flag == D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE) {
        d_FormatStringV(message_buffer, format, args);
        message_hash = hash_message(d_PeekString(message_buffer));
    } else {
        message_hash = hash_message(format);
    }

    double current_time = d_GetTimestamp();
    dLogRateLimit_t* rate_limit = NULL;
    bool should_log = false;

    for (size_t i = 0; i < rate_limit_cache->count; i++) {
        dLogRateLimit_t* entry = (dLogRateLimit_t*)d_GetDataFromArrayByIndex(rate_limit_cache, i);
        if (entry && entry->message_hash == message_hash) {
            rate_limit = entry;
            break;
        }
    }

    if (!rate_limit) {
        // First time we've seen this message.
        // ** THE FIX IS HERE: Check max_count BEFORE the first log. **
        if (max_count > 0) {
            should_log = true;
            dLogRateLimit_t new_entry = { .message_hash = message_hash, .count = 1, .max_count = max_count, .time_window = time_window, .first_log_time = current_time };
            d_AppendArray(rate_limit_cache, &new_entry);
        }
    } else {
        // We've seen this message before.
        if (current_time - rate_limit->first_log_time > time_window) {
            // Window expired. Allow one log and reset.
            should_log = true;
            rate_limit->count = 1;
            rate_limit->first_log_time = current_time;
        } else if (rate_limit->count < max_count) {
            // Still in window, but under count. Allow.
            should_log = true;
            rate_limit->count++;
        }
    }

    if (should_log && flag == D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING) {
        d_FormatStringV(message_buffer, format, args);
    }

    va_end(args);
    pthread_mutex_unlock(&rate_limit_mutex);

    if (should_log) {
        d_Log(level, d_PeekString(message_buffer));
    }
}
