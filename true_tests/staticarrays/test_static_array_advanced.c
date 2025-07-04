// File: true_tests/staticarrays/test_static_array_advanced.c
// Educational Tests for Advanced Static Array Functions
// Demonstrates real-world benefits of d_GetFreeSpaceInStaticArray, d_FillDataInStaticArray, and d_PeekRawMemoryOfStaticArray

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS WITH LOGGING
// =============================================================================

dStaticArray_t* create_test_static_array(size_t capacity, size_t element_size)
{
    dStaticArray_t* arr = d_InitStaticArray(capacity, element_size);
    d_LogIfF(arr == NULL, D_LOG_LEVEL_ERROR, "Failed to create test static array (Capacity: %zu, Element Size: %zu)", capacity, element_size);
    return arr;
}

// =============================================================================
// EDUCATIONAL TEST FUNCTIONS DEMONSTRATING REAL-WORLD BENEFITS
// =============================================================================

/**
 * TEST 1: Game Inventory Management
 * Demonstrates the power of d_GetFreeSpaceInStaticArray() for efficient inventory management.
 * This function provides a quick and convenient way to:
 * - Determine available slots without iterating, boosting performance.
 * - Implement logic to prevent adding items to a full inventory.
 */
int test_game_inventory_management(void)
{
    d_LogInfo("VERIFICATION: Game Inventory Management - Leveraging d_GetFreeSpaceInStaticArray() for dynamic slot checking and prevention.");
    dLogContext_t* ctx = d_PushLogContext("InventoryTest");

    // --- Initialization: Setting up a fixed-size game inventory ---
    // A game inventory with a fixed number of slots (e.g., 10 for items).
    // Static arrays are ideal for this due to their predictable memory and capacity.
    dStaticArray_t* inventory = create_test_static_array(10, sizeof(int));
    TEST_ASSERT(inventory != NULL, "Inventory creation should succeed (not NULL)");
    TEST_ASSERT(inventory->capacity == 10, "Inventory capacity should be 10 as specified");
    TEST_ASSERT(inventory->count == 0, "New inventory should initially be empty (count 0)");

    d_LogInfoF("🎮 Created game inventory with %zu total slots.", inventory->capacity);

    // --- Initial State Verification: Empty Inventory ---
    size_t free_slots_initial = d_GetFreeSpaceInStaticArray(inventory);
    TEST_ASSERT(free_slots_initial == inventory->capacity, "Initially, free slots should equal capacity");
    TEST_ASSERT(free_slots_initial == 10, "Initial free slots should be 10");
    d_LogInfoF("Initial inventory state: %zu/%zu slots used, %zu slots free.",
               inventory->count, inventory->capacity, free_slots_initial);

    // --- Item Collection: Adding the first few items ---
    int sword = 1, shield = 2, potion = 3;

    d_AppendDataToStaticArray(inventory, &sword);
    TEST_ASSERT(inventory->count == 1, "Inventory count should be 1 after adding sword");
    d_AppendDataToStaticArray(inventory, &shield);
    TEST_ASSERT(inventory->count == 2, "Inventory count should be 2 after adding shield");
    d_AppendDataToStaticArray(inventory, &potion);
    TEST_ASSERT(inventory->count == 3, "Inventory count should be 3 after adding potion");

    size_t free_slots_after_initial_pickup = d_GetFreeSpaceInStaticArray(inventory);
    TEST_ASSERT(free_slots_after_initial_pickup == 7, "Free slots should be 7 after adding 3 items (10-3)");
    d_LogInfoF("After collecting Sword, Shield, Potion: %zu/%zu slots used, %zu slots free.",
               inventory->count, inventory->capacity, free_slots_after_initial_pickup);

    // --- Convenience & Performance Benefit 1: Quick space check for multiple items ---
    // d_GetFreeSpaceInStaticArray() allows for immediate decision-making without
    // complex calculations or iterating through the array. This is highly performant
    // for frequent checks, typical in game loops or rapid transactions.
    int items_to_pickup_large = 8;
    d_LogInfoF("\nATTEMPTING PICKUP: Player tries to pick up %d more items.", items_to_pickup_large);

    // This check is very efficient, as d_GetFreeSpaceInStaticArray is O(1)
    if (free_slots_after_initial_pickup >= items_to_pickup_large) {
        d_LogInfoF("SUCCESS: Inventory has %zu free slots, enough for %d items!", free_slots_after_initial_pickup, items_to_pickup_large);
        TEST_ASSERT(0, "This condition should not be met if free slots < items_to_pickup_large"); // Should fail if logic is wrong
    } else {
        d_LogWarningF("FAILURE: Cannot pickup %d items! Only %zu slots available. Need to drop %d items first.",
                      items_to_pickup_large, free_slots_after_initial_pickup, items_to_pickup_large - (int)free_slots_after_initial_pickup);
        TEST_ASSERT(1, "Inventory should correctly report insufficient space for 8 items"); // This assertion should pass
    }
    TEST_ASSERT(free_slots_after_initial_pickup < items_to_pickup_large, "Inventory should indeed not have enough space for 8 more items.");


    // --- Filling the Inventory: Demonstrating full state ---
    // We'll fill the remaining slots. The `if (d_GetFreeSpaceInStaticArray(inventory) > 0)`
    // check inside the loop is a prime example of its practical use – preventing
    // overflows and ensuring robust item addition.
    d_LogInfo("\nFILLING INVENTORY: Adding remaining items until full...");
    LOOP_TEST_START(); // If you have a loop test macro for logging
    for (int i = 0; i < (int)free_slots_after_initial_pickup + 2; i++) { // Try to add slightly more than needed
        int random_item = 10 + i;
        if (d_GetFreeSpaceInStaticArray(inventory) > 0) { // Convenient, O(1) check
            d_AppendDataToStaticArray(inventory, &random_item);
            TEST_ASSERT(inventory->count <= inventory->capacity, "Inventory count should never exceed capacity");
            d_LogInfoF("  Added item: %d. Free slots: %zu.", random_item, d_GetFreeSpaceInStaticArray(inventory));
        } else {
            d_LogWarningF("  Attempted to add item %d, but inventory is full. Max capacity reached.", random_item);
            TEST_ASSERT(d_GetFreeSpaceInStaticArray(inventory) == 0, "Free space should be 0 when inventory is full");
        }
    }
    LOOP_TEST_END(); // If you have a loop test macro for logging

    // --- Final State Verification: Full Inventory ---
    size_t free_slots_final = d_GetFreeSpaceInStaticArray(inventory);
    TEST_ASSERT(free_slots_final == 0, "Inventory should now be completely full (0 free slots)");
    TEST_ASSERT(inventory->count == inventory->capacity, "Inventory count should exactly match capacity when full");
    TEST_ASSERT(inventory->count == 10, "Inventory count should be 10 (full)");

    d_LogInfoF("\nFINAL INVENTORY STATE: %zu/%zu slots used, %zu slots free.",
               inventory->count, inventory->capacity, free_slots_final);

    // --- Cleanup ---
    d_DestroyStaticArray(inventory);
    d_PopLogContext(ctx);
    return 1; // Indicate test success
}
/**
 * TEST 2: Audio Buffer Initialization
 * Demonstrates how d_FillDataInStaticArray() efficiently initializes large data sets.
 *
 * This function highlights the practical benefits:
 * - **Superior Performance**: Achieves bulk initialization significantly faster than manual looping.
 * - **Convenience**: Simplifies code by replacing boilerplate loops with a single function call.
 * - **Guaranteed State**: Ensures all specified elements are set to the desired value reliably.
 */
int test_audio_buffer_initialization(void)
{
    d_LogInfo("VERIFICATION: Audio Buffer Initialization - Bulk data filling efficiency with d_FillDataInStaticArray().");
    dLogContext_t* ctx = d_PushLogContext("AudioBufferTest");

    // --- Setup: Create a large audio buffer ---
    const size_t BUFFER_SIZE = 1024;

    dStaticArray_t* audio_buffer = create_test_static_array(BUFFER_SIZE, sizeof(float));

    TEST_ASSERT(audio_buffer != NULL, "Audio buffer should be created successfully (not NULL)");
    TEST_ASSERT(audio_buffer->capacity == BUFFER_SIZE, "Audio buffer capacity should match requested size");
    TEST_ASSERT(audio_buffer->count == 0, "Newly created audio buffer should initially be empty");

    d_LogInfoF("Created audio buffer with %zu sample capacity (each sample is %zu bytes).",
               BUFFER_SIZE, sizeof(float));

    // --- Benefit Demonstration 1: Fast Bulk Initialization (Silence) ---
    // d_FillDataInStaticArray() uses optimized memory operations (like memset or memcpy)
    // internally, making it vastly faster for large datasets compared to a manual loop.
    float silence = 0.0f;
    d_LogInfoF("\nFILLING: Initializing entire buffer with silence (%.1f) using d_FillDataInStaticArray()...", silence);
    
    LOOP_TEST_START();
    int fill_result = d_FillDataInStaticArray(audio_buffer, &silence, BUFFER_SIZE);
    LOOP_TEST_END();
    TEST_ASSERT(fill_result == 0, "d_FillDataInStaticArray for silence should succeed (return 0)");

    TEST_ASSERT(audio_buffer->count == BUFFER_SIZE, "Audio buffer count should be full after d_FillDataInStaticArray()");
    d_LogInfoF("Successfully filled %zu samples with silence", BUFFER_SIZE);
    d_LogInfoF("Buffer state: %zu/%zu samples filled.", audio_buffer->count, audio_buffer->capacity);

    // --- Verification 1: Check contents after bulk fill ---
    // Randomly checking elements to ensure they were all set correctly.
    // This emphasizes that the operation is reliable across the entire buffer.
    float* first_sample = (float*)d_IndexDataFromStaticArray(audio_buffer, 0);
    float* middle_sample = (float*)d_IndexDataFromStaticArray(audio_buffer, BUFFER_SIZE / 2);
    float* last_sample = (float*)d_IndexDataFromStaticArray(audio_buffer, BUFFER_SIZE - 1);
    float* out_of_bounds_sample = (float*)d_IndexDataFromStaticArray(audio_buffer, BUFFER_SIZE); // Should be NULL

    TEST_ASSERT(first_sample != NULL && *first_sample == silence, "First sample (index 0) should be 0.0f (silence)");
    TEST_ASSERT(middle_sample != NULL && *middle_sample == silence, "Middle sample should be 0.0f (silence)");
    TEST_ASSERT(last_sample != NULL && *last_sample == silence, "Last sample should be 0.0f (silence)");
    TEST_ASSERT(out_of_bounds_sample == NULL, "Accessing out of bounds should return NULL");

    d_LogInfoF("Verification of silence: First=%.1f, Middle=%.1f, Last=%.1f.",
               first_sample ? *first_sample : -1.0f, // Use -1.0f as an indicator if NULL
               middle_sample ? *middle_sample : -1.0f,
               last_sample ? *last_sample : -1.0f);


    // --- Benefit Demonstration 2: Refilling with a different value (Amplitude) ---
    // Demonstrates the versatility of the function to reset or change all values.
    float amplitude = 0.8f;
    d_LogInfoF("\nREFILLING: Changing all samples to amplitude %.1f using d_FillDataInStaticArray()...", amplitude);

    double start_time_refill = d_GetTimestamp();
    int refill_result = d_FillDataInStaticArray(audio_buffer, &amplitude, BUFFER_SIZE);
    double refill_time = d_GetTimestamp() - start_time_refill;

    TEST_ASSERT(refill_result == 0, "d_FillDataInStaticArray (refill) should succeed");
    d_LogInfoF("Successfully refilled %zu samples with amplitude %.1f in %.6f seconds. (Convenience Benefit: Single call for bulk change)",
               BUFFER_SIZE, amplitude, refill_time);

    // --- Verification 2: Check contents after refill ---
    float* check_sample_refilled = (float*)d_IndexDataFromStaticArray(audio_buffer, BUFFER_SIZE / 4); // Check a quarter way in
    TEST_ASSERT(check_sample_refilled != NULL && *check_sample_refilled == amplitude, "Sample at 1/4 point should match new amplitude");
    d_LogInfoF("Refill verification: Sample at index %zu now equals %.1f.",
               BUFFER_SIZE / 4, check_sample_refilled ? *check_sample_refilled : -1.0f);

    // --- Illustrative Comparison: Manual Loop (DO NOT RUN IN PRODUCTION FOR PERFORMANCE) ---
    // This section is for conceptual understanding of *why* d_FillDataInStaticArray is better.
    // In real-world scenarios, you would NOT use a loop like this for large buffers.
    LOOP_TEST_START();
    float manual_val = 0.5f;
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        // In a real scenario, you'd use a direct pointer write or d_SetDataInStaticArray
        // For this example, we'll simulate the cost of individual assignment.
        *((float*)audio_buffer->data + i) = manual_val;
    }
    LOOP_TEST_END();
    TEST_ASSERT(*((float*)d_IndexDataFromStaticArray(audio_buffer, 0)) == manual_val, "Manual fill should have set the first element.");


    // --- Cleanup ---
    d_DestroyStaticArray(audio_buffer);

    d_PopLogContext(ctx);
    return 1; // Indicate test success
}

/**
 * TEST 3: Memory Serialization for Save Games
 * Demonstrates how d_PeekRawMemoryOfStaticArray() enables direct memory access.
 *
 * This function highlights the practical benefits for serialization:
 * - **Maximum Performance**: Avoids element-by-element copying, directly accessing the contiguous memory block.
 * This is crucial for large data structures or frequent save/load operations.
 * - **Simplicity**: Streamlines serialization/deserialization logic by providing a single pointer to the entire data block.
 * - **Interoperability**: Easily interfaces with low-level I/O functions (like fwrite/fread) or network protocols
 * that expect raw byte arrays.
 */
int test_save_game_serialization(void)
{
    d_LogInfo("VERIFICATION: Save Game Serialization - Direct memory access for performance and simplicity.");
    dLogContext_t* ctx = d_PushLogContext("SerializationTest");

    // --- Setup: Define PlayerStats structure and create static array ---
    typedef struct {
        int level;
        int health;
        int mana;
        int experience;
        int gold;
        uint8_t inventory_slots[10]; // Adding some more data to make it larger
    } PlayerStats;

    const size_t NUM_PLAYER_PROFILES = 1; // Testing with one player profile
    dStaticArray_t* player_data = create_test_static_array(NUM_PLAYER_PROFILES, sizeof(PlayerStats));
    TEST_ASSERT(player_data != NULL, "Player data array should be created successfully (not NULL)");
    TEST_ASSERT(player_data->capacity == NUM_PLAYER_PROFILES, "Player data array capacity should be 1");
    TEST_ASSERT(player_data->count == 0, "Player data array should initially be empty");

    // --- Initialize Player Stats ---
    PlayerStats stats_original = {25, 100, 50, 15750, 2500, {1,2,3,4,5,6,7,8,9,10}};
    d_AppendDataToStaticArray(player_data, &stats_original);
    TEST_ASSERT(player_data->count == NUM_PLAYER_PROFILES, "Player data array should have 1 element after append");

    d_LogInfoF("Original Player Stats - Level: %d, Health: %d, Mana: %d, Experience: %d, Gold: %d (Inventory item 1: %d)",
               stats_original.level, stats_original.health, stats_original.mana, stats_original.experience, stats_original.gold, stats_original.inventory_slots[0]);

    // --- Benefit Demonstration: Get raw memory for fast serialization ---
    // d_PeekRawMemoryOfStaticArray provides a direct pointer to the underlying contiguous
    // memory block where the elements are stored. This is ideal for saving/loading
    // entire structures or arrays in a single, highly efficient operation.
    d_LogInfo("\nSERIALIZATION DEMO: Obtaining raw memory pointer...");
    void* raw_memory_ptr = d_PeekRawMemoryOfStaticArray(player_data);

    TEST_ASSERT(raw_memory_ptr != NULL, "Should successfully obtain raw memory pointer for serialization (not NULL)");
    // Crucial assertion: Verify the pointer points to the internal data buffer.
    // This assumes `player_data->data` is the actual internal buffer.
    TEST_ASSERT(raw_memory_ptr == player_data->data, "Raw memory pointer should be identical to the internal data buffer.");
    TEST_ASSERT(player_data->count > 0 ? raw_memory_ptr == d_IndexDataFromStaticArray(player_data, 0) : true,
                "Raw memory pointer should be same as index 0 if array is not empty.");


    // --- Simulate Writing to a Save File / Network Stream ---
    // Instead of looping and copying element by element, you can pass 'raw_memory_ptr'
    // directly to functions like fwrite() or a network send function.
    size_t total_data_size_bytes = player_data->count * player_data->element_size;
    TEST_ASSERT(total_data_size_bytes == sizeof(PlayerStats) * NUM_PLAYER_PROFILES, "Calculated data size should match struct size * count");
    d_LogInfoF("Calculated save data size: %zu bytes (Convenience Benefit: Total size easily derived)", total_data_size_bytes);

    // Calculate a simple checksum of the raw data as proof of direct memory access.
    // This checksum relies on direct byte-level access to the structure's memory.
    uint8_t* byte_data = (uint8_t*)raw_memory_ptr;
    uint32_t checksum = 0;
    LOOP_TEST_START(); // Measuring performance of raw byte iteration
    for (size_t i = 0; i < total_data_size_bytes; i++) {
        checksum += byte_data[i];
    }
    LOOP_TEST_END(); // End of raw byte iteration measurement

    d_LogInfoF("Data checksum (from raw memory): 0x%08X (Proves direct byte-level access)", checksum);

    // --- Simulate Deserialization: Reading directly from raw memory ---
    // Cast the raw pointer back to the expected structure type to access data directly.
    // This avoids needing to copy data into a new struct just for reading.
    PlayerStats* loaded_stats_from_raw = (PlayerStats*)raw_memory_ptr;
    d_LogInfoF("Direct memory read (simulated load) - Level: %d, Gold: %d, Inventory[0]: %d",
               loaded_stats_from_raw->level, loaded_stats_from_raw->gold, loaded_stats_from_raw->inventory_slots[0]);

    // --- Verification: Ensure raw memory data matches original ---
    TEST_ASSERT(loaded_stats_from_raw->level == stats_original.level, "Raw stats level should match original");
    TEST_ASSERT(loaded_stats_from_raw->health == stats_original.health, "Raw stats health should match original");
    TEST_ASSERT(loaded_stats_from_raw->mana == stats_original.mana, "Raw stats mana should match original");
    TEST_ASSERT(loaded_stats_from_raw->experience == stats_original.experience, "Raw stats experience should match original");
    TEST_ASSERT(loaded_stats_from_raw->gold == stats_original.gold, "Raw stats gold should match original");
    TEST_ASSERT(loaded_stats_from_raw->inventory_slots[0] == stats_original.inventory_slots[0], "Raw stats inventory[0] should match original");
    TEST_ASSERT(loaded_stats_from_raw->inventory_slots[9] == stats_original.inventory_slots[9], "Raw stats inventory[9] should match original");

    d_LogInfo("Raw memory data matches original - serialization/deserialization would be accurate and highly performant.");

    // --- Edge Case: Peeking raw memory of an empty array ---
    d_LogInfo("\nEDGE CASE: Peeking raw memory of an empty array...");
    dStaticArray_t* empty_array = create_test_static_array(5, sizeof(int));
    TEST_ASSERT(empty_array != NULL, "Empty array should be created successfully");
    TEST_ASSERT(empty_array->count == 0, "Empty array should have count 0");

    void* empty_raw_ptr = d_PeekRawMemoryOfStaticArray(empty_array);
    // For an empty array, d_PeekRawMemoryOfStaticArray might return NULL or a valid, but unusable,
    // pointer to the allocated but uninitialized capacity. The best practice is to check array->count.
    // If it returns NULL for empty, that's fine. If it returns the actual internal data pointer,
    // it's up to the caller to check count before trying to dereference.
    // Let's assert it's not NULL if capacity > 0 (assuming create_test_static_array allocates)
    TEST_ASSERT(empty_raw_ptr != NULL, "Raw memory pointer for empty array (with capacity) should not be NULL");
    TEST_ASSERT(empty_array->count == 0, "Count of empty array should remain 0 after peek");
    d_LogInfoF("Raw memory pointer for empty array: %p (should be non-NULL if capacity > 0, check count before use)", empty_raw_ptr);

    d_DestroyStaticArray(empty_array);


    // --- Cleanup ---
    d_DestroyStaticArray(player_data);

    d_PopLogContext(ctx);
    return 1; // Indicate test success
}

// Define the Pixel struct globally or at least before its first use
typedef struct {
    uint8_t r, g, b, a;  // Red, Green, Blue, Alpha components
} Pixel;

/**
 * TEST 4: Pixel Buffer Management
 * Demonstrates all three functions (d_FillDataInStaticArray, d_GetFreeSpaceInStaticArray, d_PeekRawMemoryOfStaticArray)
 * working together seamlessly in a graphics context.
 *
 * This function highlights the practical benefits for real-time graphics:
 * - **Fast Initialization/Clearing**: d_FillDataInStaticArray for rapid screen clearing.
 * - **Efficient State Checking**: d_GetFreeSpaceInStaticArray for quickly verifying buffer occupancy.
 * - **High-Performance Pixel Manipulation**: d_PeekRawMemoryOfStaticArray for direct,
 * uninterrupted access to pixel data, vital for rendering loops and shader operations.
 * - **Coordinated Memory Management**: Shows how these functions combine for robust and efficient
 * handling of large pixel buffers.
 */
int test_pixel_buffer_management(void)
{
    d_LogInfo("VERIFICATION: Pixel Buffer Management - Combined benefits for graphics rendering.");
    dLogContext_t* ctx = d_PushLogContext("PixelBufferTest");

    // --- Setup: Create a large RGBA pixel buffer (4 bytes per pixel) ---
    const size_t WIDTH = 64, HEIGHT = 64; // A common resolution for smaller textures/buffers
    const size_t PIXEL_COUNT = WIDTH * HEIGHT;
    const size_t PIXEL_SIZE_BYTES = sizeof(Pixel);

    typedef struct {
        uint8_t r, g, b, a;  // Red, Green, Blue, Alpha components
    } Pixel;

    dStaticArray_t* framebuffer = create_test_static_array(PIXEL_COUNT, PIXEL_SIZE_BYTES);
    TEST_ASSERT(framebuffer != NULL, "Framebuffer should be created successfully (not NULL)");
    TEST_ASSERT(framebuffer->capacity == PIXEL_COUNT, "Framebuffer capacity should match calculated pixel count");
    TEST_ASSERT(framebuffer->element_size == PIXEL_SIZE_BYTES, "Framebuffer element size should be 4 bytes for RGBA");
    TEST_ASSERT(framebuffer->count == 0, "Newly created framebuffer should initially be empty");

    d_LogInfoF("Created %zux%zu framebuffer (%zu pixels total, %zu bytes/pixel).",
               WIDTH, HEIGHT, PIXEL_COUNT, PIXEL_SIZE_BYTES);

    // --- Phase 1: Clear screen to black using d_FillDataInStaticArray() ---
    // Benefit: Fast bulk initialization. Essential for clearing frames in real-time graphics.
    Pixel black_pixel = {0, 0, 0, 255}; // Black with full alpha
    d_LogInfoF("\nPHASE 1: Clearing framebuffer to black using d_FillDataInStaticArray()...");
    LOOP_TEST_START(); // Measure the performance of the fill operation
    int fill_result = d_FillDataInStaticArray(framebuffer, &black_pixel, PIXEL_COUNT);
    LOOP_TEST_END();
    TEST_ASSERT(fill_result == 0, "d_FillDataInStaticArray for black fill should succeed (return 0)");
    TEST_ASSERT(framebuffer->count == PIXEL_COUNT, "Framebuffer count should be full after d_FillDataInStaticArray()");

    d_LogInfoF("Framebuffer cleared to black. (Benefit: Rapid bulk operation for frame clearing)");

    // --- Phase 2: Verify buffer occupancy using d_GetFreeSpaceInStaticArray() ---
    // Benefit: Efficient state checking. Quickly confirms buffer is ready for drawing or full.
    d_LogInfo("\nPHASE 2: Verifying framebuffer occupancy using d_GetFreeSpaceInStaticArray()...");
    size_t free_space = d_GetFreeSpaceInStaticArray(framebuffer);
    d_LogInfoF("Framebuffer utilization: %zu/%zu pixels used, %zu free.",
               framebuffer->count, framebuffer->capacity, free_space);
    TEST_ASSERT(free_space == 0, "Framebuffer should have 0 free space after full fill");
    TEST_ASSERT(framebuffer->count == framebuffer->capacity, "Framebuffer count should match capacity when full");
    d_LogInfo("Framebuffer occupancy confirmed. (Benefit: Instantaneous check for buffer state)");


    // --- Phase 3: Fast Pixel Manipulation via d_PeekRawMemoryOfStaticArray() ---
    // Benefit: High-performance direct memory access. Allows graphics routines to
    // write directly to the pixel data without function call overhead for each pixel.
    d_LogInfo("\nPHASE 3: Obtaining raw pixel data and applying red gradient via direct memory access...");
    void* raw_pixels = d_PeekRawMemoryOfStaticArray(framebuffer);

    TEST_ASSERT(raw_pixels != NULL, "d_PeekRawMemoryOfStaticArray should successfully obtain raw pixel data (not NULL)");
    TEST_ASSERT(raw_pixels == framebuffer->data, "Raw pixel pointer should be identical to the internal data buffer.");
    TEST_ASSERT(raw_pixels == d_IndexDataFromStaticArray(framebuffer, 0), "Raw pixel pointer should point to the first element.");

    Pixel* pixel_array = (Pixel*)raw_pixels;

    // Apply a simple red gradient: X-coordinate determines red intensity.
    // This loop directly manipulates the memory, simulating a rendering routine.
    LOOP_TEST_START(); // Measure the performance of direct pixel manipulation
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            size_t index = y * WIDTH + x;
            uint8_t red_intensity = (uint8_t)((x * 255) / (WIDTH > 1 ? WIDTH - 1 : 1)); // Scale to 0-255, avoid div by zero
            pixel_array[index].r = red_intensity;
            // Keep g=0, b=0, a=255 as set by the initial fill operation
        }
    }
    LOOP_TEST_END(); // End of direct pixel manipulation measurement
    d_LogInfo("Red gradient applied to framebuffer via direct memory access. (Benefit: Max performance for rendering)");

    // --- Verification: Check gradient by inspecting specific pixels ---
    // Verify that the direct memory manipulation had the intended effect.
    Pixel* left_pixel = &pixel_array[HEIGHT / 2 * WIDTH + 0];         // Middle row, left edge (should be near black)
    Pixel* right_pixel = &pixel_array[HEIGHT / 2 * WIDTH + (WIDTH - 1)]; // Middle row, right edge (should be near full red)
    Pixel* middle_pixel = &pixel_array[HEIGHT / 2 * WIDTH + (WIDTH / 2)]; // Middle row, middle (should be medium red)

    d_LogInfoF("Gradient verification: Left pixel R=%d, Middle pixel R=%d, Right pixel R=%d.",
               left_pixel->r, middle_pixel->r, right_pixel->r);

    TEST_ASSERT(left_pixel->r >= 0 && left_pixel->r < 5, "Left pixel red intensity should be near 0 (e.g., 0-4)");
    TEST_ASSERT(right_pixel->r > 250, "Right pixel red intensity should be near 255 (e.g., 251-255)");
    TEST_ASSERT(middle_pixel->r > 120 && middle_pixel->r < 135, "Middle pixel red intensity should be around 127");
    TEST_ASSERT(left_pixel->g == 0 && left_pixel->b == 0 && left_pixel->a == 255, "Other channels should remain 0/255 from fill");


    d_LogInfo("Gradient effect successfully verified. Combined functions provide robust pixel buffer management.");

    // --- Cleanup ---
    d_DestroyStaticArray(framebuffer);

    d_PopLogContext(ctx);
    return 1; // Indicate test success
}

/**
 * TEST 5: Performance Analysis & Error Handling
 * Demonstrates edge cases, performance characteristics, and proper error handling
 * for d_GetFreeSpaceInStaticArray(), d_FillDataInStaticArray(), and d_PeekRawMemoryOfStaticArray().
 *
 * This test highlights:
 * - **Robustness**: How functions handle invalid inputs (e.g., NULL pointers).
 * - **Performance**: The efficiency gains of bulk operations vs. individual element manipulation.
 * - **Reliability**: How capacity constraints are enforced and memory consistency is maintained.
 * - **Best Practices**: The importance of understanding function limitations for stable applications.
 */
int test_performance_and_edge_cases(void)
{
    d_LogInfo("VERIFICATION: Performance Analysis & Edge Cases - Function robustness and efficiency.");
    dLogContext_t* ctx = d_PushLogContext("PerformanceTest");

    // --- Test 1: NULL Pointer Handling ---
    // Benefit: Ensures the functions don't crash or exhibit undefined behavior when given invalid pointers.
    // Critical for robust, production-ready code.
    d_LogInfo("\nTEST 1: Validating NULL pointer handling for all functions...");

    // d_GetFreeSpaceInStaticArray with NULL
    size_t free_space_null = d_GetFreeSpaceInStaticArray(NULL);
    TEST_ASSERT(free_space_null == 0, "d_GetFreeSpaceInStaticArray with NULL array should return 0 (no free space)");
    d_LogInfoF("d_GetFreeSpaceInStaticArray(NULL) returned: %zu.", free_space_null);

    // d_FillDataInStaticArray with NULL array
    int fill_result_null_array = d_FillDataInStaticArray(NULL, NULL, 0);
    TEST_ASSERT(fill_result_null_array == 1, "d_FillDataInStaticArray with NULL array should return error code 1");
    d_LogInfoF("d_FillDataInStaticArray(NULL, ...) returned error code: %d.", fill_result_null_array);

    // d_FillDataInStaticArray with valid array but NULL data (if element_size is not 0)
    // Create a dummy array to test NULL data pointer
    dStaticArray_t* dummy_array_for_null_data = create_test_static_array(5, sizeof(int));
    TEST_ASSERT(dummy_array_for_null_data != NULL, "Dummy array for NULL data test should be created.");
    int fill_result_null_data = d_FillDataInStaticArray(dummy_array_for_null_data, NULL, 5);
    // Depending on implementation, this might be 0 if 'element_size' is used, or 1 if 'source_data' is validated.
    // Assuming it should fail if source_data is NULL and count > 0.
    TEST_ASSERT(fill_result_null_data == 1, "d_FillDataInStaticArray with NULL source data (and count > 0) should return error code 1");
    d_LogInfoF("d_FillDataInStaticArray(array, NULL, ...) returned error code: %d.", fill_result_null_data);
    d_DestroyStaticArray(dummy_array_for_null_data);

    // d_PeekRawMemoryOfStaticArray with NULL
    void* raw_memory_null = d_PeekRawMemoryOfStaticArray(NULL);
    TEST_ASSERT(raw_memory_null == NULL, "d_PeekRawMemoryOfStaticArray with NULL array should return NULL");
    d_LogInfoF("d_PeekRawMemoryOfStaticArray(NULL) returned: %p.", raw_memory_null);

    d_LogInfo("NULL handling tests PASSED. Functions are robust against invalid array pointers.");


    // --- Test 2: Performance Comparison - Bulk Fill vs. Individual Assignment ---
    // Benefit: Quantifies the speed advantage of d_FillDataInStaticArray() for large datasets.
    // This is crucial for choosing the right function in performance-critical sections (e.g., game engines).
    d_LogInfo("\nTEST 2: Performance comparison - Bulk fill vs individual assignment (large data set)...");

    const size_t PERF_SIZE = 100000; // Increased size for more noticeable difference
    dStaticArray_t* perf_array = create_test_static_array(PERF_SIZE, sizeof(int));
    TEST_ASSERT(perf_array != NULL, "Performance array should be created successfully");
    TEST_ASSERT(perf_array->capacity == PERF_SIZE, "Performance array capacity should match PERF_SIZE");

    int test_value = 42;

    // Method 1: Bulk fill using d_FillDataInStaticArray
    d_LogInfoF("  Measuring d_FillDataInStaticArray for %zu elements...", PERF_SIZE);
    LOOP_TEST_START(); // Start timing for bulk fill
    int bulk_fill_res = d_FillDataInStaticArray(perf_array, &test_value, PERF_SIZE);
    LOOP_TEST_END(); // End timing for bulk fill
    TEST_ASSERT(bulk_fill_res == 0, "Bulk fill operation should succeed");
    TEST_ASSERT(perf_array->count == PERF_SIZE, "Array count should be full after bulk fill");
    // Verify a random element
    TEST_ASSERT(*(int*)d_IndexDataFromStaticArray(perf_array, PERF_SIZE / 2) == test_value, "Middle element should be filled correctly after bulk fill");


    // Reset array for fair comparison. In a real scenario, you might destroy and re-create.
    // For test comparison, this direct reset is convenient assuming it does not invalidate internal pointers.
    perf_array->count = 0;
    // For a truly fair comparison, you might re-create the array or ensure the fill operation is completely independent.
    // Here, we're testing the *append* mechanism's overhead vs *fill*.
    d_LogInfoF("  Measuring individual d_AppendDataToStaticArray for %zu elements...", PERF_SIZE);
    LOOP_TEST_START(); // Start timing for individual append
    for (size_t i = 0; i < PERF_SIZE; i++) {
        // This simulates sequential insertion overhead
        int append_res = d_AppendDataToStaticArray(perf_array, &test_value);
        TEST_ASSERT(append_res == 0, "Individual append should succeed");
    }
    LOOP_TEST_END(); // End timing for individual append
    TEST_ASSERT(perf_array->count == PERF_SIZE, "Array count should be full after individual appends");
    TEST_ASSERT(*(int*)d_IndexDataFromStaticArray(perf_array, PERF_SIZE / 2) == test_value, "Middle element should be filled correctly after individual appends");


    d_LogInfo("Performance test completed. Check logs for speedup ratio.");
    d_DestroyStaticArray(perf_array);


    // --- Test 3: Capacity Overflow Protection for d_FillDataInStaticArray ---
    // Benefit: Prevents buffer overflows and ensures data integrity by enforcing array boundaries.
    // Critical for preventing crashes and security vulnerabilities.
    d_LogInfo("\nTEST 3: Testing capacity overflow protection for d_FillDataInStaticArray()...");

    dStaticArray_t* small_array = create_test_static_array(3, sizeof(int));
    TEST_ASSERT(small_array != NULL, "Small array should be created successfully");
    TEST_ASSERT(small_array->capacity == 3, "Small array capacity should be 3");
    TEST_ASSERT(small_array->count == 0, "Small array should initially be empty");

    int dummy_val = 99;

    // Attempt to fill more than capacity
    d_LogInfoF("  Attempting to fill %zu elements into array with capacity %zu...", (size_t)5, small_array->capacity);
    int overflow_result = d_FillDataInStaticArray(small_array, &dummy_val, 5); // Should fail

    TEST_ASSERT(overflow_result == 1, "d_FillDataInStaticArray with count > capacity should return error code 1");
    // Crucial: Ensure the array's state remains unchanged after a failed operation
    TEST_ASSERT(small_array->count == 0, "Array count should remain 0 after failed fill attempt to exceed capacity.");
    TEST_ASSERT(d_GetFreeSpaceInStaticArray(small_array) == 3, "Free space should remain capacity after failed overflow fill.");
    d_LogInfo("d_FillDataInStaticArray overflow protection PASSED. Array state preserved on failure.");

    // Test normal operation still works after a failed attempt (regression check)
    d_LogInfoF("  Attempting normal fill (%zu elements) after failed overflow attempt...", (size_t)2);
    int normal_result = d_FillDataInStaticArray(small_array, &dummy_val, 2);
    size_t final_free = d_GetFreeSpaceInStaticArray(small_array);

    TEST_ASSERT(normal_result == 0, "Normal fill within capacity should succeed");
    TEST_ASSERT(small_array->count == 2, "Array count should be 2 after normal fill");
    TEST_ASSERT(final_free == 1, "Free space should be 1 after normal fill");
    TEST_ASSERT(*(int*)d_IndexDataFromStaticArray(small_array, 0) == dummy_val, "First element should be filled correctly.");
    d_LogInfoF("Normal fill successful: Result=%d, Free space=%zu, Count=%zu.",
               normal_result, final_free, small_array->count);

    d_DestroyStaticArray(small_array);


    // --- Test 4: Memory Consistency Check (Index vs. Raw Memory) ---
    // Benefit: Verifies that d_IndexDataFromStaticArray() and d_PeekRawMemoryOfStaticArray()
    // provide consistent views of the same underlying memory. This builds confidence in their
    // reliability for data access.
    d_LogInfo("\nTEST 4: Testing memory consistency between d_IndexDataFromStaticArray() and d_PeekRawMemoryOfStaticArray()...");

    dStaticArray_t* consistency_array = create_test_static_array(100, sizeof(double));
    TEST_ASSERT(consistency_array != NULL, "Consistency array should be created successfully");
    TEST_ASSERT(consistency_array->capacity == 100, "Consistency array capacity should be 100");

    double pi_val = 3.1415926535;
    double e_val = 2.7182818284;

    // Fill first 50 elements with PI, then fill all 100 with E (overwriting the PI)
    d_FillDataInStaticArray(consistency_array, &pi_val, 50); // Fill first half
    d_FillDataInStaticArray(consistency_array, &e_val, 100); // Fill all 100 with E (this will overwrite PI)
    TEST_ASSERT(consistency_array->count == 100, "Consistency array count should be 100 after full fill");


    // Check a value in the middle via d_IndexDataFromStaticArray
    double* indexed_value_ptr = (double*)d_IndexDataFromStaticArray(consistency_array, 25);
    TEST_ASSERT(indexed_value_ptr != NULL, "Indexed value pointer at index 25 should be valid");
    TEST_ASSERT(fabs(*indexed_value_ptr - e_val) < 1e-9, "Indexed value at 25 should be 'e_val'");
    d_LogInfoF("Value at index 25 via d_IndexDataFromStaticArray: %.10f", *indexed_value_ptr);

    // Get raw memory pointer and check the same position
    double* raw_array_ptr = (double*)d_PeekRawMemoryOfStaticArray(consistency_array);
    TEST_ASSERT(raw_array_ptr != NULL, "Raw array pointer should be valid (not NULL)");
    TEST_ASSERT(raw_array_ptr == consistency_array->data, "Raw array pointer should match internal data pointer");

    double raw_value_at_index = raw_array_ptr[25]; // Direct access
    d_LogInfoF("Value at index 25 via d_PeekRawMemoryOfStaticArray: %.10f", raw_value_at_index);

    TEST_ASSERT(fabs(*indexed_value_ptr - raw_value_at_index) < 1e-9, "Indexed value and raw value should be identical for the same position.");
    d_LogInfo("Memory consistency PASSED. Both access methods point to the same data.");

    // Verify a value near the end, specifically using raw access.
    double* indexed_value_ptr_end = (double*)d_IndexDataFromStaticArray(consistency_array, 99);
    double raw_value_at_index_end = raw_array_ptr[99];
    TEST_ASSERT(fabs(*indexed_value_ptr_end - e_val) < 1e-9, "Indexed value at 99 should be 'e_val'");
    TEST_ASSERT(fabs(raw_value_at_index_end - e_val) < 1e-9, "Raw value at 99 should be 'e_val'");
    TEST_ASSERT(fabs(*indexed_value_ptr_end - raw_value_at_index_end) < 1e-9, "Indexed and raw values at end should be identical.");


    d_DestroyStaticArray(consistency_array);
    d_PopLogContext(ctx);
    return 1; // Indicate test success
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

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
    
    d_LogInfo("Starting Advanced Static Array Function Tests");
    d_LogInfo("These tests demonstrate real-world benefits of the new utility functions");
    
    TEST_SUITE_START("MIDAS Enhanced Advanced Static Array Tests");

    // Run all educational tests
    RUN_TEST(test_game_inventory_management);
    RUN_TEST(test_audio_buffer_initialization);
    RUN_TEST(test_save_game_serialization);
    RUN_TEST(test_pixel_buffer_management);
    RUN_TEST(test_performance_and_edge_cases);
    
    TEST_SUITE_END();

    d_LogInfo("Advanced Static Array Test Suite completed.");
    d_LogInfo("Benefits demonstrated:");
    d_LogInfo("- Efficient capacity checking (d_GetFreeSpaceInStaticArray)");
    d_LogInfo("- Fast bulk initialization (d_FillDataInStaticArray)");
    d_LogInfo("- Direct memory access for performance (d_PeekRawMemoryOfStaticArray)");

    d_DestroyLogger(logger);

    // The TEST_SUITE_END macro should handle the return value based on total_tests and tests_passed
    // Assuming tests.h provides a mechanism for main to return success/failure based on global counts.
    return (total_tests == tests_passed) ? 0 : 1; 
}