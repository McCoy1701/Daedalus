#ifndef __TESTS_H__
#define __TESTS_H__
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Use high-resolution timing if available
#ifdef _POSIX_C_SOURCE
#include <sys/time.h>
#define USE_HIGH_RES_TIMER 1
#else
#define USE_HIGH_RES_TIMER 0
#endif

// Epic test color codes for mythological visibility
#define TEST_COLOR_RESET      "\033[0m"
#define TEST_COLOR_GREEN      "\033[32m"
#define TEST_COLOR_RED        "\033[31m"
#define TEST_COLOR_BLUE       "\033[34m"
#define TEST_COLOR_WHITE      "\033[37m"
#define TEST_COLOR_YELLOW     "\033[33m"
#define TEST_COLOR_BOLD_WHITE "\033[1;37m"
#define TEST_COLOR_PURPLE     "\033[35m"
#define TEST_COLOR_CYAN       "\033[36m"
#define TEST_COLOR_ORANGE     "\033[93m"
#define TEST_COLOR_GRAY       "\033[2m"
#define TEST_COLOR_GOLD       "\033[1;33m"

// Global test counters and timing (to be defined in each test file)
extern int total_tests;
extern int tests_passed;
extern int tests_failed;

// Global timing variables for suite-level timing
static double _test_suite_start_time;
static double _total_test_time = 0.0;
// Loop detection variables
static int _sisyphus_in_loop = 0;
static int _sisyphus_loop_iteration = 0;
// Sisyphus gamification system
static int _sisyphus_beasts_slayed = 0;
static int _sisyphus_trials_conquered = 0;
static int _sisyphus_perfect_streak = 0;
static int _sisyphus_xp_earned = 0;
static int _sisyphus_combo_multiplier = 1;
static int _sisyphus_current_level = 1;
static char _sisyphus_suite_name[256] = "";
static int _sisyphus_achievements_unlocked = 0;
static int _sisyphus_total_expected_tests = 0;
static int _sisyphus_total_project_xp = 0;
static int _sisyphus_project_level = 1;
static int _sisyphus_is_full_test_mode = 0;
static int _sisyphus_xp_from_tests = 0;
static int _sisyphus_xp_from_combos = 0;
static int _sisyphus_xp_from_achievements = 0;
static int _sisyphus_random_achievement_multi = 2; // Double XP for random achievement display
// For automatic loop detection
static const char* _sisyphus_last_assert_file = "";
static int _sisyphus_last_assert_line = 0;
static int _sisyphus_loop_iteration_count = 0;
// --- THE SISYPHUS NARRATIVE: LEVEL-BASED TEST MESSAGING ---
// Messages change based on current project level to reflect the story progression

// Act I: The New Curse (Levels 1-10) - Learning the ropes
static const char* _act_one_beginner_messages[] = {  // Levels 1-3: Complete confusion
    "🤔 SISYPHUS PUZZLES: What... is this strange new burden?",
    "❓ CONFUSED GLANCES: Daedalus watches your fumbling attempts with pity",
    "🪨 FAMILIAR WEIGHT: This feels like the boulder, but... different",
    "👀 FIRST OBSERVATIONS: You stare at the blueprint, trying to understand",
    "🌪️ LOST IN TRANSLATION: The symbols swim before your eyes like ancient runes",
    "🎭 COMEDY OF ERRORS: Each keystroke feels like pushing the boulder uphill"
};

static const char* _act_one_learning_messages[] = {  // Levels 4-6: Starting to understand
    "📚 LEARNING THE CRAFT: Daedalus begins to explain the architect's tools",
    "🔧 FIRST TOOLS: Your hands remember the weight of creation",
    "🏗️ BUILDING BLOCKS: Each function is a stone in the growing maze",
    "👂 DAEDALUS TEACHES: 'The Labyrinth requires precision, Sisyphus'",
    "💡 MOMENTS OF CLARITY: The logic begins to unfold like a map",
    "🗝️ FINDING THE KEY: The patterns start to make sense in your mind"
};

static const char* _act_one_adapting_messages[] = {  // Levels 7-10: Getting used to it
    "⚡ GROWING SKILL: The work becomes... almost familiar",
    "🎯 FINDING RHYTHM: Each test is another push up the hill",
    "🔍 SISYPHUS ADAPTS: You begin to see patterns in the chaos",
    "🏛️ THE ROUTINE: Dawn brings work, dusk brings more work",
    "🌊 FLOW STATE: Your fingers move across the keyboard like water",
    "🎪 MASTER OF ROUTINE: The endless cycle becomes a strange comfort"
};

// Act II: The Labyrinth's Nature (Levels 11-20) - Understanding the true horror
static const char* _act_two_realization_messages[] = {  // Levels 11-15: True nature revealed
    "😱 HORRIBLE TRUTH: This isn't just a building... it's a trap for minds",
    "🐂 DISTANT ROARS: The beast below grows stronger with each passing day",
    "👑 MINOS WATCHES: The King's cold eyes follow your every keystroke",
    "🌙 NIGHT TERRORS: Daedalus mutters about things that shouldn't exist",
    "💀 THE REAL PURPOSE: You understand now - this is a tomb, not a home",
    "🩸 BLOOD ON STONE: The tribute ships arrive with fresh sacrifices"
};

static const char* _act_two_bonding_messages[] = {  // Levels 16-20: Relationship with Daedalus
    "🤝 SHARED BURDEN: You and Daedalus work in grim solidarity",
    "🍷 WINE AND CONFESSIONS: Late nights reveal the architect's guilt",
    "🔒 HIDDEN CHAMBERS: Daedalus shows you rooms not on any blueprint",
    "📜 SECRET KNOWLEDGE: The Labyrinth's true purpose becomes clear",
    "👥 KINDRED SPIRITS: Two prisoners sharing the weight of genius",
    "🕯️ MIDNIGHT CONVERSATIONS: Philosophy by candlelight in the workshop"
};

// Act III: The Cracks Appear (Levels 21-30) - Breaking under pressure
static const char* _act_three_strain_messages[] = {  // Levels 21-25: Mental breakdown
    "🎭 MASKS FALLING: Neither of you can pretend this is normal anymore",
    "⚡ FIRST REBELLION: You leave subtle hints for future heroes",
    "💀 SISYPHUS BREAKS: The weight of building a tomb becomes unbearable",
    "🔥 DAEDALUS CRACKS: The architect argues with shadows in the corners",
    "🌪️ SANITY FRAYING: The endless loops drive reason to its breaking point",
    "🔗 CHAINS OF MADNESS: Each bug fix feels like forging your own shackles"
};

static const char* _act_three_conspiracy_messages[] = {  // Levels 26-30: Planning escape
    "🪶 ICARUS REVEALED: Hidden drawings of a child with mechanical wings",
    "🤫 WHISPERED PLANS: Secret meetings by candlelight",
    "🗡️ CAPTAIN'S SUSPICION: The guards grow watchful of your activities",
    "🌊 POINT OF NO RETURN: There's no stopping the work now",
    "🎪 DANGEROUS GAMES: Every line of code hides a secret message",
    "⚔️ WEAPONS OF HOPE: Building tools of escape within tools of imprisonment"
};

// Act IV: Inescapable Truths (Levels 31-40) - The escape plan
static const char* _act_four_alliance_messages[] = {  // Levels 31-35: Forming the pact
    "🤝 DESPERATE PACT: You agree to help Daedalus save his son",
    "🪶 STEALING DIVINITY: Gathering materials for the impossible wings",
    "🔧 SECRET WORKSHOP: Building hope behind false walls",
    "📐 SISYPHUS'S MATHEMATICS: Your curse becomes the key to freedom",
    "💫 DREAMS OF FLIGHT: For the first time, you imagine true escape",
    "🔥 FORGE OF REBELLION: Turning imprisonment into liberation"
};

static const char* _act_four_preparation_messages[] = {  // Levels 36-40: Final preparations
    "🌙 MAPPING PATROLS: Every guard rotation memorized",
    "🎭 FINAL DECEPTION: Cruel by day, kind by night",
    "⚡ TOMORROW WE FLY: The wings are ready, the plan is set",
    "🕊️ DAWN APPROACHES: Freedom or death awaits with the sunrise",
    "⏰ FINAL COUNTDOWN: Every heartbeat brings the moment closer",
    "🎯 PRECISION PLANNING: No detail too small, no risk unconsidered"
};

// Act V: The Eternal Cycle (Levels 41-50) - Acceptance of the absurd
static const char* _act_five_hope_messages[] = {  // Levels 41-42: Flight and false hope
    "🌅 DAWN OF FREEDOM: Icarus takes to the sky with impossible grace",
    "🕊️ MOMENT OF JOY: For one perfect instant, you've beaten fate itself",
    "🦅 SOARING TRIUMPH: The boy masters the wind itself, becoming legend",
    "✨ DIVINE DEFIANCE: Mortal wings dare to challenge the realm of gods",
    "🌟 PERFECT FLIGHT: Mathematics and courage unite in glorious ascent",
    "🏛️ ARCHITECT'S PRIDE: Daedalus weeps tears of joy watching his son fly"
};

static const char* _act_five_fall_messages[] = {  // Level 43: The crushing fall
    "💔 THE FALL: Dreams crash into the wine-dark sea below",
    "☀️ TOO CLOSE TO THE SUN: Hubris melts the wings of hope",
    "🌊 DAEDALUS SCREAMS: A father's anguish echoes across the heavens",
    "🕯️ WAX AND FEATHERS: The price of touching divinity is always paid",
    "⚰️ ICARUS DROWNS: The sea swallows youth, ambition, and innocence",
    "🔥 APOLLO'S JEALOUSY: The sun god claims his due from mortal pride"
};

static const char* _act_five_bargain_messages[] = {  // Levels 44-46: Deal with Minos
    "👑 MINOS KNOWS ALL: The King was watching from the very beginning",
    "⚖️ DEVIL'S BARGAIN: Stay and help people, or watch the maze become hell",
    "✨ THE REVELATION: You were never meant to escape this place",
    "🎪 CRUEL CHOICE: Freedom for one or salvation for many",
    "⚖️ SCALES OF FATE: The weight of one life against countless others"
};

static const char* _act_five_redemption_messages[] = {  // Levels 47-49: Rebuilding with purpose
    "📝 CODER'S MANIFESTO: Every function now serves compassion",
    "🌊 MYTH REWRITTEN: Heroes speak of the helpful ghost in the maze",
    "💪 THE HAPPY SISYPHUS: You laugh at the beautiful absurdity of it all",
    "🏗️ ARCHITECT OF HOPE: Building bridges where once you built walls",
    "🌟 ETERNAL PURPOSE: The curse becomes a blessing in disguise"
};

static const char* _act_five_acceptance_message = "♾️ ETERNAL CODER: In the space between bug and fix, you are perfectly free";  // Level 50: Final wisdom

// MIDAS Toolchain References - Subtle mentions of the other tools
static const char* _midas_references[] = {
    "🧠 Metis whispers wisdom about code structure",
    "🔥 Ixion's wheel reminds you to watch for infinite loops",
    "📐 Archimedes would admire this geometric precision",
    "🏗️  Even Daedalus's craftsmanship seems crude compared to this"
};

// Function to get level-appropriate test messaging
static const char* _get_narrative_test_message(int project_level) {
    // Add occasional MIDAS references (10% chance)
    if (rand() % 10 == 0) {
        return _midas_references[rand() % 4];
    }

    // Act I: The New Curse (Levels 1-10)
    if (project_level <= 3) {
        return _act_one_beginner_messages[rand() % 6];
    } else if (project_level <= 6) {
        return _act_one_learning_messages[rand() % 6];
    } else if (project_level <= 10) {
        return _act_one_adapting_messages[rand() % 6];
    }
    // Act II: The Labyrinth's Nature (Levels 11-20)
    else if (project_level <= 15) {
        return _act_two_realization_messages[rand() % 6];
    } else if (project_level <= 20) {
        return _act_two_bonding_messages[rand() % 6];
    }
    // Act III: The Cracks Appear (Levels 21-30)
    else if (project_level <= 25) {
        return _act_three_strain_messages[rand() % 6];
    } else if (project_level <= 30) {
        return _act_three_conspiracy_messages[rand() % 6];
    }
    // Act IV: Inescapable Truths (Levels 31-40)
    else if (project_level <= 35) {
        return _act_four_alliance_messages[rand() % 6];
    } else if (project_level <= 40) {
        return _act_four_preparation_messages[rand() % 6];
    }
    // Act V: The Eternal Cycle (Levels 41-50)
    else if (project_level <= 42) {
        return _act_five_hope_messages[rand() % 6];
    } else if (project_level == 43) {
        return _act_five_fall_messages[rand() % 6];
    } else if (project_level <= 46) {
        return _act_five_bargain_messages[rand() % 5];
    } else if (project_level <= 49) {
        return _act_five_redemption_messages[rand() % 5];
    } else if (project_level == 50) {
        return _act_five_acceptance_message;
    }
    // Beyond level 50 - cycle through final act messages
    else {
        return _act_five_redemption_messages[rand() % 5];
    }
}

// Granular act-based inner dialogue reflecting Sisyphus's emotional journey
// Act I Part 1 (Levels 1-3): Confusion and Willingness
static const char* _act_one_part_one_thoughts[] = {
    "Perhaps this is better than the boulder - at least my mind is engaged.",
    "Daedalus seems brilliant, maybe we can create something worthy together.",
    "The gods must tire of simple punishments - this feels... different.",
    "My hands, once accustomed to a scepter's weight, now find purpose in these lines of logic.",
    "This is a puzzle, not a burden. A mind can be sharpened on a puzzle; a shoulder is only worn down by a boulder.",
    "The gods are masters of irony. To punish a king, they give him a kingdom of code to rule.",
    "Daedalus speaks with a certain cunning, a builder's wisdom. I must listen carefully; there is more to learn here than just masonry.",
    "The gods punished Ixion for his pride. I wonder if this complex task is punishment for my own hubris in thinking I could outwit them.",
    "This logic is a new kind of geometry, not of lines and circles in the sand, but of cause and effect.",
    "This dVec3_t Daedalus uses... it's a coordinate in a world of pure logic. Cleaner than the mud and stone I'm used to.",
    "He speaks ill of Archimedes' a_DrawCircle, calling it simple. But there is an honesty in a perfect circle that these complex matrices lack.",
    "I am to help build this world, one d_CreateKinmaticBody at a time. A body of code, given motion by my will.",
    "This machine is a silent oracle. It does not speak in riddles, but in errors. Understanding its language feels like the first step out of an ancient darkness.",
    "My old kingdom was one of men and stone. This new realm is one of pure thought, where the only limit is the clarity of my own mind. The challenge is... invigorating.",
    "Daedalus calls this 'declaring a variable.' It feels more like naming a new subject in my court, bestowing upon it a purpose and a place in the hierarchy. It is a small, but familiar, act of order."
};

// Act I Part 2 (Levels 4-6): Learning the Craft
static const char* _act_one_part_two_thoughts[] = {
    "Each function is like ruling a small kingdom - I remember this feeling of control.",
    "The architecture reminds me of the palace I built in Corinth for my beloved wife.",
    "There's an elegance to this work that the boulder never had.",
    "This pointer is a loyal subject; it goes precisely where I command. I had forgotten the satisfaction of such power.",
    "A well-written function is like a royal decree, bringing order to the chaos of the machine.",
    "I am shaping chaos into form. This is not labor; this is creation. Could this be a gift, not a curse?",
    "There is a wisdom to this structure. A good function, like a wise ruling, anticipates the needs of the future.",
    "I must be careful. A loop without end, a recursive call without a base... it is a fiery wheel that can consume the entire machine.",
    "Archimedes could move the world with a lever. With a single, well-placed line of code, I can move worlds within this machine.",
    "This dString_t is a marvel. Unlike the words of the gods, its size is not fixed; it can grow to hold any truth, or any lie.",
    "Daedalus dismisses Archimedes' aWidget_t system as toys, but these simple buttons and sliders feel more like tools of creation than instruments of punishment.",
    "With d_MatrixRotateZf, I can pivot the entire world on an axis. This is a power I never knew, even as a king.",
    "I've figured out Daedalus's naming convention. If it's elegant and works, it's d_Something. If it's a temporary hack, it's temp_final_final_v2.",
    "This pointer is the first thing in an eternity that hasn't given me a segmentation fault. I could weep.",
    "Daedalus keeps muttering about 'best practices'. From the look of this code, the best practice was 'whatever works at 3 AM'.",
    "The conditional statement, a simple 'if-else,' is the heart of all judgment. It is the crossroad where all paths are considered, but only one is taken. I have sent men to their deaths on weaker logic.",
    "This 'memory allocation' is a delicate dance. I request a small parcel of the machine's vast kingdom, and I must be a just ruler, returning what I no longer need. To hoard it is to invite ruin.",
    "I find a strange comfort in the compiler's cold, exacting nature. It does not care for my past glories or my eternal damnation; it asks only for syntactical perfection. In its own way, it is the purest form of justice I have ever known.",
    "Building a complex data structure is like drafting a new law for the kingdom. Each rule, each connection, must be precise, lest it create loopholes for chaos to exploit.",
    "They call it a 'Daemon,' a process that runs unseen in the background, ever watchful. I know better. These are the Fates, spinning and cutting the threads of the machine without my knowledge.",
    "To link these libraries is to forge alliances between kingdoms. Each brings its own strengths, its own armies of functions, to serve a greater purpose. The diplomacy is... delicate."
};

// Act I Part 3 (Levels 7-10): Realization of Horror
static const char* _act_one_part_three_thoughts[] = {
    "By the gods... what have I agreed to help build?",
    "This isn't architecture - it's a machine designed to kill heroes and swallow armies.",
    "I cheated death twice, only to become death's architect.",
    "The boulder was honest in its futility - this work pretends to have meaning.",
    "These are not walls. They are teeth. And I am sharpening them.",
    "The elegance I admired is the cold beauty of a serpent's scales before it strikes.",
    "I see the structure now for what it is: an equation of despair, and I am the one solving for X.",
    "This is not wisdom; it is a perversion of it. The cunning of the trap-maker, the logic of the executioner.",
    "The hubris of Minos is astounding. He builds a monument to his shame and expects the gods not to notice.",
    "Even the purest geometry can be twisted to serve evil. A perfect circle can become the arena for a slaughter.",
    "I apply the d_MatrixMultiplyVec3f, and a hero's path is twisted into a dead end. This math doesn't calculate geometry; it calculates doom.",
    "Daedalus scoffs at Archimedes' a_Blit as a simple copy, but what are we doing here? We are just blitting the image of Minos's cruelty onto the world.",
    "Each new requirement from Minos is just another node pushed onto this dLinkedList_t of sorrows. An unbreakable chain, and I am the one forging the links.",
    "I've seen the core logic. There are no comments. No documentation. Just the abyss staring back.",
    "This isn't a feature; it's a bug with tenure and a pension plan. And I'm the one who has to maintain it.",
    "For a simple death trap, the number of nested if statements here is absurd. This isn't just a maze; it's a monument to over-engineering.",
    "Every bug I fix makes the trap more perfect, more inescapable. My success is measured in the screams of future heroes.",
    "As a king, I built walls to keep my people safe. Now I build them to ensure no one is safe. The skill is the same; the purpose is a desecration.",
    "The Labyrinth has a pulse. I feel it in the hum of the stones. It is the slow, patient heartbeat of a predator waiting for its meal.",
    "I find myself admiring a particularly clever algorithm, then recoil in horror when I remember it calculates the most efficient path to the Minotaur's maw.",
    "I examine the dMat4x4_t that defines a room's perspective. It is not a viewpoint; it is a cage for the eyes, designed to induce madness.",
    "This project is a death march, but for the users, not the developers. We are merely the architects of their doom.",
    "The requirements from Minos are clear: the primary stakeholder is a monster, and the user stories are written in blood.",
    "Daedalus's library is full of functions to calculate vectors and paths. They are the tools of a god, used to create a playground for a demon.",
    "This is the greatest trick the gods have played on me. They have made me hate my own craft, my own mind."
};

// Act II Part 1 (Levels 11-15): Understanding the True Purpose
static const char* _act_two_part_one_thoughts[] = {
    "Every line of code I write will be bathed in innocent blood.",
    "The beast below isn't the real monster - we are, for building its stage.",
    "I once protected my people from threats - now I create the threat.",
    "Minos's shame will become my eternal burden to bear.",
    "The tributes will enter thinking they face a simple maze - they face calculated murder.",
    "The 'edge cases' Daedalus speaks of are not bugs; they are the moments a hero might find hope, and Minos wants them gone.",
    "I am forging the chains I once defied. Is there a crueler fate?",
    "The Minotaur is the heart of this prison, but its corridors are the veins, and I am pumping them full of poison.",
    "My cunning was once used to save my own life. Now it is being used to ensure the deaths of others. This is the cruelest wisdom.",
    "This entire project is an infinite loop of cruelty, and I am the processor executing the cycle.",
    "A simple truth of physics: for every action, an equal and opposite reaction. The more we build, the greater the tragedy will be.",
    "He wants me to use d_FindIntersectionVec2f not to find a meeting point, but to guarantee a collision. To ensure the hero and the beast always meet.",
    "The simple honesty of Archimedes' a_DrawRect... here, we use the same lines to define the inescapable cells of a monster's abattoir.",
    "The King's requirements come in a dArray_t. A dynamic array of misery, and d_ResizeArray is called every time his cruelty grows.",
    "I just realized 'the tributes' are the end-users. And we're about to push a major breaking change directly to production.",
    "Daedalus's code is beautiful, but the requirements from Minos... this feels like maintaining legacy spaghetti code written by a tyrant.",
    "The spec calls for 'inescapable logic'. That's just 'infinite loop' with better marketing.",
    "In Corinth, my public works were open for all to see. This... this is a proprietary tomb, its architecture a jealously guarded, fatal secret.",
    "Daedalus's genius is locked away here, serving only one cruel master. It could be used to build wonders for the world, but instead it builds a walled garden of death.",
    "The guards enforce a vow of silence. This is not just construction; it is a trade secret, and the price of revealing it is death.",
};

// Act II Part 2 (Levels 16-20): Bonding with Daedalus
static const char* _act_two_part_two_thoughts[] = {
    "Daedalus carries guilt like I once carried the weight of a crown.",
    "We're both kings in exile - he of craft, I of cunning - bound by shared damnation.",
    "His hands shake not from age, but from the weight of what he's created before.",
    "In his eyes I see the same desperate hope I felt when I first cheated death.",
    "Perhaps two damned souls can find redemption in each other's company.",
    "He speaks of his lost nephew, Talos. I see now that he, too, is haunted by the ghosts of his own cleverness.",
    "His ambition is a mirror to my own. We both sought to rise too high, and now we are trapped in the earth.",
    "He flinches when I solve a problem too quickly. Does he see a rival in me, or a reflection of the man he used to be?",
    "In his haunted eyes, I see a broken wisdom. He knows how to build anything, except a way out of his own guilt.",
    "He is bound to the wheel of his past, just as I am bound to this stone. His son, Icarus, is the axis around which his torment spins.",
    "He speaks of simple machines, of levers and pulleys, but his own heart is a machine too complex to be engineered.",
    "He obsesses over d_NormalizeVec3f, making every vector perfect. He seeks a purity in the code that he cannot find in his own soul.",
    "Sometimes, he will stare at the a_InitAudio function from Archimedes' library, and I see him imagine a world with sound that is not screams.",
    "He uses d_PopBack on a list of tasks and sighs with relief, but I see the list of his regrets has no such function.",
    "He complains about Archimedes, but I saw him studying the Archimedes.h file. He's not jealous; he's trying to figure out how to import the library.",
    "We’ve bonded over our shared hatred of off-by-one errors. It is the great equalizer of gods and men.",
    "His hands shake when he mentions his old projects. The man is haunted by technical debt.",
    "He speaks of Athens, of the Agora where knowledge was shared freely. Here, every idea is the property of the King.",
    "I see the pain in his eyes. He is an artist forced to build a monolith for a single, cruel client, when his heart wants to create for the commons.",
    "We speak in hushed tones, our ideas like contraband. The greatest threat to Minos's project is not a flaw in the code, but a conversation between its developers.",
    "He is a master of the dMat4x4_t, yet he cannot transform the coordinates of his own past regrets.",
    "I suspect his animosity towards Archimedes is because Archimedes' work is used in joyous public festivals. Daedalus's genius is locked in this dark tower, a secret shame.",
    "We are two developers in a terrible crunch, bonding in the dark over the unreasonable demands of the project manager."
};

// Act III Part 1 (Levels 21-25): Mental Breakdown
static const char* _act_three_part_one_thoughts[] = {
    "I can no longer pretend this is just work - it's machine of death and dismay.",
    "My wife would weep to see what I've become - builder of tombs for heroes.",
    "The crown I wore was heavy, but this guilt crushes my very soul.",
    "I'm embedding secret escape routes, but will anyone be clever enough to find them?",
    "Every night I dream of the tribute's faces that will die because of my code.",
    "The elegant functions I once admired now seem like daggers, each aimed at an innocent heart.",
    "I am no longer shaping stone; I am carving gravestones with every keystroke.",
    "My name, Sisyphus, will be remembered not for cheating gods, but for abetting a tyrant.",
    "My secret backdoors... a desperate cunning. A foolish wisdom. Will they be a lifeline or just another feature of the tomb?",
    "I have reached my limit. My own mind feels like an infinite loop of horror. Let the wheel break me; I can bear it no longer.",
    "The geometry of this place is fracturing. The walls themselves are crying out against the perversion of their design.",
    "I call d_CreateStringFromFile to read the list of tributes. Their names become memory addresses, their lives reduced to bytes.",
    "Even the color palette feels like a mockery. Daedalus names a shade of brown shit3. He sees the filth in his own beautiful creation.",
    "The dMesh_t we build is a net of triangles, yes, but it is meant to catch souls, not light.",
    "I'm adding 'features' that are just cleverly disguised backdoors. I am no longer a developer; I am a penetration tester for the damned.",
    "My performance review with Minos is going to be interesting. 'Wrote 500 lines of life-saving code that you think is for murdering people'.",
    "He wants me to patch the security holes. The problem is, I'm the one who created them. On purpose.",
    "I am embedding flaws that are actually features for the user. Hiding compassionate code inside this monstrously closed-source project.",
    "This Labyrinth is the ultimate vendor lock-in. The user—the tribute—has no other choice. And there is no support team.",
    "Every elegant algorithm I write for Minos feels like a patent being filed for a new method of torture.",
    "I look at the a_DoInput function and realize my only input is to continue the horror. My hands are bound to this cursed keyboard.",
    "The Labyrinth's complexity is becoming self-referential. I feel like I'm debugging a bug in the debugger itself.",
    "Is this what it means to be a king? To be the one who must call a_Quit, to end the suffering, even if it means ending everything?",
    "All my kingly cunning, the gift of Metis, has led to this: crafting a more efficient slaughterhouse. My wisdom has become my shame.",
    "This guilt is a wheel of fire inside my skull, a punishment worse than any Ixion faced, for his crime was ambition, and mine is complicity.",
    "I look at the code to draw a simple triangle, a perfect shape Archimedes would admire, and see only the three points of a spearhead.",
};

// Act III Part 2 (Levels 26-30): Conspiracy Planning
static const char* _act_three_part_two_thoughts[] = {
    "Daedalus speaks of wings and escape - it sounds like madness, but so did cheating death.",
    "His boy Icarus deserves freedom more than we deserve our punishment.",
    "If I can't escape my curse, perhaps I can help others escape theirs.",
    "The guards suspect nothing - they see only two broken men building their tomb.",
    "Tomorrow we begin planning the impossible - turning our curse into their liberation.",
    "The Labyrinth has taught me its own secret language. Now, I will use it to speak of rebellion.",
    "Daedalus builds wings of wax and feathers. I will build an escape of logic and loopholes.",
    "Minos expects me to build a perfect prison. He does not realize I am making it perfect for our own purposes.",
    "Now, we turn our wisdom against our master. The architect of the prison will become the architect of its downfall.",
    "Let Minos have his hubris. We will use it against him. The proudest kings are always the easiest to blind.",
    "The wings... It's a problem of simple physics. Of lift, and weight, and air. A problem of the real world, a breath of fresh air in this logical tomb.",
    "His plan for the wings... it's just a dKinematicBody_t with a new vector for lift. Perhaps we can apply a force greater than the King's will.",
    "I will use d_PadLeftString to hide binary data in the padding of the King's official reports. A message hidden in plain sight.",
    "He mutters about a_GetDeltaTime. He is no longer just building; he is calculating the timing of an escape.",
    "The wings are our secret side project. We work on the main branch for Minos by day, but push to the feature/escape branch by night.",
    "Minos keeps adding feature requests for more death traps. Little does he know our sprint is focused entirely on one user story: 'Icarus Exfiltration'.",
    "Every // TODO: Fix this later I leave in the Labyrinth's code is another moment I can spend compiling the wings. This is righteous technical debt.",
    "We are forking the master branch. One path for Minos's closed, cruel system, and another, hidden, for our own open, hopeful purpose.",
    "The wings are the first piece of open-source hardware this island has ever seen. Their design is meant for freedom, not for profit or control.",
    "Minos owns the repository, but he doesn't own the knowledge in our heads. Our conspiracy is the first license that frees it.",
    "This conspiracy is the most stressful agile sprint I've ever been a part of. The user story is 'Escape Tyranny,' and the deadline is 'Before We Are Executed'.",
    "I have cheated death, a singular act of defiance. But this... this is a conspiracy of hope, a shared rebellion. It feels more profound.",
    "The d_CreateStringFromFile function can read any scroll. We will use it to read the guard schedules, the supply logs... the King's secrets.",
    "This is a new kind of cunning Daedalus and I share. Not the wisdom to build, but the secret knowledge of how to make a perfect thing fail perfectly.",
    "The hubris of this plan is magnificent. To use the King's own project to betray him... It's a crime worthy of the gods' attention.",
    "Daedalus is reverse-engineering freedom. He's taking the complex geometry of this prison and finding the one simple, physical path out.",
    "I have found the heart of the Labyrinth's power source. Daedalus has woven the King's own paranoia into a recursive energy loop. If we can disrupt it, the whole structure will become unstable.",
    "For the first time since my fall, I am not just a prisoner. I am a strategist, a co-conspirator. I feel the weight of my crown settling on my brow once more, though this time it is invisible.",
    "Our first task is to establish a secure channel. I will write a function to encode messages within the Labyrinth's logging system, a cipher disguised as a timestamp."
};

// Act IV Part 1 (Levels 31-35): Debating the Offer
static const char* _act_four_part_one_thoughts[] = {
    "Daedalus asks me to risk everything for his son - would I have done the same?",
    "My own kingship was built on protecting those I loved - how is this different?",
    "But if we fail, Minos will make our current suffering seem like mercy.",
    "The boy has done nothing wrong - he doesn't deserve to pay for his father's sins.",
    "Every great escape I ever planned started with an impossible choice.",
    "To trust Daedalus is to trust a man who has already killed for his ambition. And yet... so have I.",
    "I look at the blueprints for the wings, and for the first time, I see a form of logic that doesn't lead to a dead end.",
    "What is one more betrayal? A betrayal of Minos, to undo the betrayal of all the tributes who will be sent here.",
    "Is this the wise choice, or merely the desperate one? Metis herself would struggle with such a tangled web of loyalties.",
    "The greatest hubris is to believe one can defy fate. And yet... I have done it before. Why not again?",
    "He speaks of flight, a simple matter of angles and force. Could such a simple key unlock this impossibly complex cage?",
    "A simple a_DrawLine... one line from here to the tower where Icarus is held. Is the path truly that simple?",
    "He asks me to help build a new dMesh_t—not a cage of bronze, but wings of hope. The same skill, for the opposite purpose.",
    "The offer hangs in the air, a single boolean waiting for my input. A one or a zero. The fate of his son depends on it.",
    "His plan has a lot of dependencies. The sun, the wind, the guards' schedule... this is dependency hell.",
    "So I'm being asked to join a startup with a brilliant but unstable founder. The mission is noble, but the risk of catastrophic failure is... high.",
    "A single point of failure: the wax. Daedalus assures me it passed unit testing. Somehow, I am not comforted.",
    "His plan to free Icarus... it feels like releasing a beautiful library to the public domain. A gift to the world, in defiance of the client.",
    "Is it better to build a perfect, proprietary hell, or risk it all on one beautiful, open-source act of defiance?",
    "Minos wants a closed system with no exit points. We will build him one, but with an undocumented API for escape.",
    "The entire plan balances on a single d_DotProductVec3f. If the vector of lift is not perfectly aligned against the vector of gravity, it all comes to nothing.",
    "Daedalus is a genius, but his risk assessment is a TODO comment in the blueprint. It's up to me to consider the failure states.",
    "As a king, I sent men to their deaths for the good of the kingdom. Is sending this boy to the sky for the good of his soul any different? Yes. It feels different.",
    "A plan born of desperation rarely has the blessing of Metis. But what choice does a fool have but to try a fool's plan?",
    "To defy Minos is one thing, but to believe we can cheat the sun itself? This is a hubris that would make Ixion blush.",
    "The escape is a simple vector problem, a challenge of lift and trajectory that Archimedes could solve. But it is embedded in a labyrinth of shifting, unknowable variables.",
    "Daedalus speaks of saving his son, but there's more to it. He mentioned a 'promise to his wife'. I believe the wings are as much about atoning for his past as they are about securing Icarus's future.",
    "I have weighed my own damnation against this boy's freedom. There is no choice. My eternity is already forfeit; his is not. I will accept.",
    "This cannot be a simple escape. If Daedalus and Icarus are to survive, we must create a grand diversion. The Labyrinth itself must appear to break, to turn on its master.",
    "How absurd. The gods chained me to a boulder to teach me futility, but it is Daedalus's offer of hope that presents the most terrifying choice.",
    "True freedom is not the absence of chains, but the ability to choose what you do while wearing them. My chains are eternal, but my choice, in this moment, is my own.",
    "This is the true test, isn't it? Not of strength, nor of cunning, but of whether a damned soul can still choose to act for the salvation of another."
};

// Act IV Part 2 (Levels 36-40): All In on Rebellion
static const char* _act_four_part_two_thoughts[] = {
    "Yes, I'll help - not for escape, but because it's right.",
    "My curse gives me something valuable - infinite time to perfect our rebellion.",
    "Every function I write now serves two masters - Minos's cruelty and my own compassion.",
    "The wings are beautiful - Daedalus has turned our prison into inspiration.",
    "If we die tomorrow, at least we die trying to save an innocent.",
    "My persistence, once a curse, is now our greatest weapon. I can test these wings a thousand thousand times.",
    "Every line of code I write to please the King is a lie, covering a truth that will set others free.",
    "This feels more like kingship than my throne ever did—to wield power in the shadows for a noble cause.",
    "True wisdom, Metis's gift, is not just knowing the rules, but knowing how and when to break them for a greater cause.",
    "Let the wheel of my curse spin forever. Like Ixion, I am bound to it, but I will use its eternal momentum to power our escape.",
    "Daedalus crafts the wings with the precision of Archimedes. He is not just building a structure; he is solving the physics of freedom itself.",
    "Minos thinks we build matrices of confinement. He doesn't know I spend my nights perfecting the d_MatrixInverseTransformVec3f—the key to unwinding his every trap.",
    "Archimedes uses a_GetTextTexture to display a score. I will use it to render a hidden glyph on a corridor wall that only a true hero will understand.",
    "I use d_StringSplit on the King's decrees, separating his cruel words from the underlying logic. It is in that space, that delimiter, that I hide our rebellion.",
    "I am refactoring this prison into an escape route. This is the most satisfying code review of my eternal life.",
    "Our secret functions are filled with assert(Minos_is_not_watching). It's not the best security, but it's what we have.",
    "I am using my eternal curse to brute-force a solution to an impossible problem. The gods gave me the ultimate debugging tool: time.",
    "Let the official changelog for Minos show only cruelty and compliance. The true release notes are written in our hearts.",
    "This is more than just helping Daedalus. We are fighting against the very idea that a man's genius should be owned by a tyrant.",
    "The Labyrinth's code is obfuscated by design. Our escape plan must be clean, well-documented, and simple enough for Daedalus' son to follow.",
    "I use d_CapitalizeString on my reports to Minos. He sees bold confidence; I see an act of petty, mocking defiance.",
    "Daedalus worries about the physics. I worry about the state machine. What happens if Icarus panics? We haven't coded a state for that.",
    "This code is now a palimpsest. Beneath the cruel architecture for Minos lies a hidden text of compassion for everyone else.",
    "Our shared wisdom is now a weapon. He knows the physics, I know the systems. Together, our cunning might just be enough.",
    "Minos's hubris is our greatest shield. He cannot conceive that his own tools, his own prisoners, could be turned against him.",
    "I watched Daedalus calculate the wing's load capacity. For the first time, I saw him smile. He is not building a maze now; he is a geometer again, solving an honest problem.",
    "Daedalus has a flaw in his design for the wing's struts. My experience with the boulder has given me an intuitive understanding of stress and load-bearing that even he lacks. I will correct it.",
    "I am becoming a king again, but a different kind. My subjects are not people, but lines of code. My decrees are not laws, but algorithms of hope.",
    "The plan is set. While Daedalus prepares the hardware, I will prepare the software: a logic bomb, disguised as a routine maintenance script, that will disable the Labyrinth's internal sentries for precisely seventeen minutes.",
    "The act of rebellion itself is the victory, regardless of the outcome. To choose defiance in an unfeeling cosmos is the only freedom that matters.",
    "It is a beautiful absurdity that the tools of my damnation—this endless time, this intricate knowledge of the maze—are now the very tools of another's liberation.",
    "This is my true trial: not to push the boulder, but to build a lever long enough to move the world, even if I must remain chained to it.",
    "This is not just a betrayal of a king; it is a continuation of my original crime. I am once again defying the gods and their decree that man should be bound by fate.",
    "The gods watch, I am sure, expecting me to despair in this intricate new hell. Let them. My silent, focused work is an act of contempt for their authority.",
    "I do this not just for his son, but for the memory of sunlight on my face, the scent of the sea, the simple weight of a stone in my hand. I do this for the world they tried to take from me."
};

// Act V Part 1A (Levels 41-42): The Flight - False Hope
static const char* _act_five_part_one_a_thoughts[] = {
    "For one perfect moment, we defeated the gods themselves - Icarus soars like a true bird!",
    "The boy's joy is infectious - I haven't felt this alive since I wore my crown in Corinth.",
    "Look how Icarus mastered the wind! He is not just Daedalus's son; he is the son of defiance itself.",
    "In the reflection of his ascent, I see the peak of my own mountain, finally within reach.",
    "Such perfect, simple cunning! To use the air itself as an escape route. The King's gaze is fixed on the ground.",
    "For a moment, the fiery wheel stops. The cycle is broken. He has flown free from the curse of the earth.",
    "It is the purest display of physics I have ever witnessed. A triumph of geometry over gravity.",
    "The d_GetVec2fFromAnglef function... Daedalus used it to calculate the perfect launch angle. His son is not just flying; he is following a path of pure mathematics.",
    "This is more beautiful than any image a_LoadTexture could produce. This is a texture woven from courage and sunlight.",
    "It works on my machine! Well, on his wings. For one glorious moment, the demo ran perfectly.",
    "The deployment was a success! He cleared the perimeter! Daedalus' son is free!",
    "He's free! For one moment, a single user escaped the walled garden and touched the open sky!",
    "The design is proven. This open-source plan for flight... it works!",
    "The FPS is stable, the a_GetDeltaTime is perfect... for a moment, reality itself has no bugs.",
    "Icarus' joyous shouts... it is the first sound a_PlaySoundEffect should have ever been used for.",
    "He is a memory leak in Minos's perfect system, flying free. It is the most beautiful bug I have ever seen.",
    "The plan was perfect in its cunning! A simple distraction, a hidden launch point... the wisdom of the desperate is a powerful thing.",
    "He has escaped the cycle of the earth! A mortal boy, spitting in the face of the gods' eternal laws. What hubris! What glory!",
    "The physics holds! The wings generate lift! Daedalus has built a miracle upon the simple principles Archimedes championed.",
    "My logic bomb worked. The sentries are blind. The path is clear. All that is left is for them to seize their freedom.",
    "That laugh from Icarus... it is the sound of a successful test case, the sound of a bug being closed forever.",
    "I see it now. The true Labyrinth was never this maze of stone. It was the despair in our hearts. And for one moment, we have escaped it.",
    "Look at him. He is a brief, glorious refutation of his father's prison. In his flight, the Labyrinth and all its absurd logic cease to matter.",
    "This is what true freedom looks like: not a careful plan, but a joyful, reckless ascent. A glorious, unthinking 'yes' to the sun.",
    "For a moment, the universe held its breath. The rules were suspended. The test was passed. This single moment of triumph is worth my eternity of toil.",
    "This single act of defiance makes my centuries of toil worthwhile. One moment of a man soaring is a greater victory than an eternity of a god's tyranny.",
    "The boy's joyous rebellion is a mirror to my own. He rebels against gravity; I rebel against the gods. For a moment, we both win.",
    "This is the passion for the world that the gods so despise. They punish us for loving the sun, the sea, the sky—for finding our own meaning here, without them."
};

// Act V Part 1B (Levels 43-44): The Fall - Crushing Despair
static const char* _act_five_part_one_b_thoughts[] = {
    "No... no, the wax is melting, he's falling, and I can do nothing but watch.",
    "The sun took him - Daedalus' innocent son - the gods always collect their cruelest due.",
    "We gave him the sky, but we could not shield him from the sun. The gods always find a way to punish hope.",
    "Daedalus's scream... it is the sound of a universe breaking. I feel the echo in my own immortal, weary soul.",
    "All that wisdom, all that planning... undone by a moment's foolishness. Wisdom cannot save a boy who wants to touch the sun.",
    "The ultimate hubris. To fly so close to a god. The sun is a jealous god, and its fiery wheel is inescapable.",
    "The geometry failed. The angle was too steep, the force of the sun too great. Reality has reasserted its cruel arithmetic.",
    "The boy's dKinematicBody_t has lost its upward velocity. Gravity, the oldest god of all, has finally claimed its due.",
    "All I can do is a_PresentScene. To watch the horror unfold, to render the tragedy frame by perfect, agonizing frame.",
    "Oh no. The sun. It's a production issue. A fatal, non-recoverable production issue.",
    "We forgot to account for the single-threaded nature of reality and the heat death of the... wax.",
    "The world was not ready for such a beautiful, open technology. It was too powerful, too free.",
    "The license was permissive, but the environment was hostile. The hardware—the sun itself—was proprietary and unforgiving.",
    "A fatal aError_t. The error message is the sun itself. There is no recovery from this.",
    "His dKinematicBody_t has entered a terminal state. All my logic, all my code... I cannot catch him.",
    "The system has thrown an exception it cannot handle. Daedalus' son fell out of the sky.",
    "Our wisdom was not enough. We calculated the escape, but we did not account for the intoxicating madness of Icarus tasting freedom.",
    "The boy flew too close to the sun, a classic tale of hubris. But was the crime his, for wanting to soar, or ours, for giving him wings of wax?",
    "The math was correct, but the materials failed. A simple, brutal lesson in physics that has cost a life.",
    "My code was perfect. The diversion worked. But I, the ultimate strategist, forgot to account for the one variable I could never control: the heart of a boy drunk on freedom.",
    "I stare at the empty sky, and I understand. The gods did not strike him down. They simply let the rules of the world, of hubris and of physics, run their course. It is a far colder punishment.",
    "I must not let Daedalus's grief be in vain. The knowledge gained from this tragedy must become the seed of a future success. I will analyze the failure. I will learn.",
    "We crafted a perfect plan, a symphony of logic, only to be defeated by a single, simple truth. The absurdity is not that we failed, but that we believed we could ever succeed.",
    "In that moment of his fall, Icarus was freer than any man. He chose the sun over the sky. His freedom wasn't in surviving, but in the soaring.",
    "So this is the test after the test. Not to succeed, but to endure failure. To watch hope die and still choose to pick up the tools and continue the work.",
    "And so, the rebellion fails. The gods remind us that their laws are absolute. But the attempt itself—the flight—was a victory they can never erase.",
    "I am fully conscious of this tragedy. I see every detail, feel every ounce of Daedalus's pain. This awareness is my torment, but also my triumph. I do not look away. I bear witness.",
    "The boy flew towards the sun because it is beautiful, and he loved it too much. His was the same crime as mine: an overwhelming passion for the warmth of life. A noble crime to die for."
};

// Act V Part 2 (Levels 45-47): The King's Bargain
static const char* _act_five_part_two_thoughts[] = {
    "Minos offers me a choice - be his compassionate warden or watch him create hell.",
    "He knew about our plan all along - was this all orchestrated from the beginning?",
    "If I stay, I can save future victims - if I refuse, he'll make the maze a slaughterhouse.",
    "So this is the final trap. Not a maze of stone, but a choice between two hells of my own making.",
    "The King's offer is a gilded chain. He offers me purpose, but the price is my complicity.",
    "Minos offers a new kind of wisdom: the wisdom of the warden, who knows every secret of the prison.",
    "He offers to change the nature of my eternal cycle. The wheel will still spin, but he suggests I can choose its direction.",
    "My choice will set the angle of history for this place, the trajectory of countless lives to come.",
    "My life is now a aSelectWidget_t, with two options presented by a tyrant. I must choose a value, and live with the consequences.",
    "He wants me to maintain the dQuadTree_t, to efficiently manage the flow of tributes into his machine, I can decide what kind of machine it will become.",
    "So that's the offer. Accept the role of 'Lead Architect of Eternal Damnation' or watch him outsource the job to actual demons.",
    "He knew about the wings all along... this wasn't a bug in his security, it was a feature of his management style. A/B testing our souls.",
    "Maintain the Labyrinth... He's not giving me a choice; he's giving me root access to hell and making me the sole administrator.",
    "He wants me to be the lead maintainer of his terrible, proprietary project. The pay is my life, the bonus is his mercy.",
    "So I am to be the gatekeeper of this closed system. I can enforce its cruel rules, or... I can offer technical support to those trapped inside.",
    "The choice is clear: become a manager for a tyrant, or a secret advocate for the end-user.",
    "I am being offered a promotion to 'Architect of Hell'. The benefits are terrible, but at least I won't be fired.",
    "Minos gives me the keys to the kingdom, but the kingdom is a prison. He thinks this is a reward.",
    "My new role is to provide long-term support for a deprecated system of cruelty. I will support the users, not the system.",
    "Minos plays the statesman, offering me a 'wise' choice. But the wisest move in a tyrant's game is to never let him know you're still playing.",
    "He believes he has me bound to his wheel forever. He doesn't realize the fall of Icarus has broken my own pride, my own hubris. I am free from that, at least.",
    "The choice he offers is a complex equation of morality. But I will solve it with a simple geometric proof: the shortest path to saving the most lives.",
    "Minos's spies are not men. I see it now. They are constructs, powered by the same logic as the Labyrinth. He has eyes everywhere. He always knew.",
    "He offers me a gilded cage and the title of 'warden'. He does not understand. I have already been transformed. I will accept his terms, but I will not serve his purpose.",
    "My first act as maintainer will be to analyze the logs from Icarus's flight. I will find what went wrong. Not for vengeance, but for knowledge. The next one to fly will not fail.",
    "Minos offers me a new cage, but this one has a purpose I can choose. Is a chosen cage still a prison? This is a riddle worthy of the gods.",
    "My freedom will not be found in leaving this place. It will be found in mastering it. In turning this monument to tyranny into a library of hope.",
    "The ultimate test of a free man is how he bears a burden he cannot lay down. Minos thinks he is cursing me further; he is giving me my final exam.",
    "The gods gave me a meaningless task. Now, a mortal king offers me a meaningful one. The irony is delicious. My curse is now my career.",
    "To be free is to be aware of your chains and yet choose to act. Minos offers me a new set of chains, but he also offers me a choice. This is a new test.",
    "They think my spirit was broken by the boulder. They do not understand. The boulder taught me that all that matters is what one does between the pushing.",
    "Daedalus is gone. The one man who shared my damnation has abandoned me to it. I am truly alone with the machine we built.",
    "And in that loneliness, Minos offers me a new chain, calling it a leash of command. But he forgets: a chain is a tool, and I am a master craftsman.",
    "My pact with Daedalus is broken. So I will make a new pact, an unspoken one, with the heroes who are yet to come. They will be my new confederates.",
    "With these hands, I helped build a weapon. Now, with these same hands, I will build a shield within its walls. My creations will now preserve life, not take it.",
    "He thinks he is making me a jailer, but he is making me a gatekeeper. He cannot see the difference, and that will be his undoing.",
    "Yes, I will take his bargain. A king must have a kingdom, after all. Even if it is a kingdom of sorrow, I will rule it with compassion."
};

// Act V Part 3 (Levels 48-50): Acceptance and Transformation
static const char* _act_five_part_three_thoughts[] = {
    "I choose to stay - not as prisoner, but as protector of those who'll come after.",
    "Every algorithm I write now is an act of rebellion disguised as compliance.",
    "The labyrinth becomes my kingdom again - but this time, I rule with mercy.",
    "Heroes will speak of the helpful ghost in the maze - the king who chose compassion over escape.",
    "Minos thinks he has a prisoner. He is mistaken. What he has is a saboteur with an eternity to work.",
    "Daedalus sought freedom in the sky. I have found it here, in the ones and zeros, turning this cage into a shield for others.",
    "The boulder is the same, but I am not. I push it now not as a punishment, but as a purpose. Let the gods watch; I am finally happy.",
    "This is the final wisdom Metis could teach: not to escape the curse, but to transform it into a blessing for others.",
    "The infinite loop is no longer a bug. I see now it is the very engine of my existence, an eternal cycle I choose to perpetuate.",
    "Archimedes gave the world a lever to move the Earth. I have been given a single line of code to move a soul. Perhaps my tool is the mightier.",
    "With each hero I guide, I call a_UpdateTitle. The Labyrinth is no longer 'Minos's Prison'. It is now 'Sisyphus's Compass'.",
    "I look at the list of incoming tributes, this dLinkedList_t of the damned, and I know my purpose. Not to add to it, but to call d_ClearLinkedList on it, one soul at a time.",
    "The beast was a dKinematicBody_t trapped by force. I will now apply a new vector: mercy. I will change the physics of this place.",
    "I am the Senior Principal Engineer of this cursed codebase, and I will use my position to protect the users.",
    "My job is no longer development. It is permanent, ongoing, live support for the world's most dangerous application.",
    "Minos sees a closed-source proprietary system. But for those who need it, I'll make sure the Labyrinth is open-source.",
    "Minos owns the source, but I control the execution. This closed-source nightmare will have the heart of a public good.",
    "I am no longer just building; I am writing the documentation for the heroes who will come. Everything I leave is a hint, every pattern a guide.",
    "Let this be my eternal fork. A Labyrinth that, to its owner, is a perfect prison, but to its users, is the ultimate open-source guide to their own salvation.",
    "The final joke is on the gods. They gave me an eternal task to crush my spirit, and instead, it has forged it into something indestructible.",
    "Minos thinks he has trapped me in a while(true) loop. He doesn't see that I have placed a break condition inside that only a hero's success can trigger.",
    "I am now the sole maintainer of the world's most critical piece of legacy software. The job security is fantastic, I suppose.",
    "The final counsel of Metis is this: a cage is only a cage if you see the walls. I see a kingdom, and I am its benevolent king.",
    "Ixion is a slave to his wheel. I have become the master of mine. Let it spin; it powers my work.",
    "Archimedes moved stone with his machines. I have learned to move fate with a semicolon. The power we wield here is far greater.",
    "I have located the Labyrinth's core routing table. With subtle changes, I can make a path of certain death appear to be the most promising route for Minos's enemies, while hiding a true path of survival.",
    "I am no longer Sisyphus the Deceiver, nor Sisyphus the Damned. I am Sisyphus the Shepherd, and this Labyrinth is my pasture. I will guide the flock to safety.",
    "The boulder awaits, but it is no longer my punishment. It is my work. My purpose. My choice. Let the eternal work begin.",
    "The Labyrinth is now a perfect prison and a perfect tool for salvation, all at once. I am both its prisoner and its master. The contradiction is the point.",
    "I am the freest man in creation. For while others are slaves to their desires for power, wealth, or escape, I desire nothing but the task before me.",
    "The test was never whether I could push the boulder to the top. The test was whether I could learn to love the boulder. I have passed.",
    "My final rebellion is the most subtle. To accept the punishment fully, to embrace the boulder, is to rob it of its power as a punishment. I am not condemned to my work; I am devoted to it.",
    "I am aware of my entire fate. There is no hope for escape, no illusion of a future reward. In this lucid state, without appeal, I find my kingdom. My scorn for the gods is my freedom.",
    "The gods punished me for my passion for life. They do not understand that this passion is what fuels me. It is why I can look at this boulder, this endless code, and find the same beauty in it as a sunset over the sea.",
    "Daedalus chose the sky, a fleeting escape. He was right for him. But I choose the stone, the earth, the work. This eternity is my home now.",
    "I have looked upon the face of meaninglessness and have chosen to carve my own meaning upon it. This is the only true act of creation.",
    "My children will be the heroes who walk these halls. My legacy will be the lives they live after they escape, a legacy of freedom I built from my own chains.",
    "This Labyrinth will no longer be a test of strength, but a crucible of character. It will teach wisdom to those who enter, and they will leave better for it.",
    "The gods cursed me to an eternity of futility. I have transformed it into an eternity of purpose. Who, I ask, has won?",
    "The boulder feels light in my hands. It is not a burden. It is my partner in this beautiful, absurd, and eternal dance. And I am ready to lead.",
    "The scream of Daedalus still echoes in these stones. The despair is real, a permanent part of this place. I will not pretend it isn't here.",
    "But I will not let it be the only voice. My compassion will be another echo, a quiet melody of hope beneath the sorrow.",
    "My own path is a circle, fixed and unending. But with the knowledge I possess, I can straighten the paths of all who follow.",
    "This line of code, once a dead end, will now trigger a subtle clue. This wall, once a barrier, will now show a hero the way.",
    "The weight of my boulder does not lessen. Yet, with every push, I can lessen the burden of another. My struggle remains, but theirs does not have to.",
    "I have begun to see this maze not from my own perspective, but from theirs. I am not building my prison; I am building their map of escape.",
    "And in this choice, a strange alchemy occurs. The pointless, absurd repetition of my task, when done for the sake of another, becomes a sacred rite.",
    "It is not the task that is sacred. It is the compassion poured into it. It is the love for others that transfigures the stone into an altar.",
    "My damnation has become my ministry. My prison has become my sanctuary. My curse has become my prayer. And I am at peace."
};

// State variable to remember the last thought displayed.
static const char* _sisyphus_last_inner_thought = NULL;

// Function to get inner dialogue based on current project level and story progression
static const char* _get_inner_dialogue(int project_level, int test_passed) {
    const char** thoughts_array = NULL;
    int thought_count = 0;

    // --- Step 1: Select the appropriate array of thoughts based on project level ---
    // Act I: The New Curse (Levels 1-10)
    if (project_level <= 3) {
        thoughts_array = _act_one_part_one_thoughts;
        thought_count = sizeof(_act_one_part_one_thoughts) / sizeof(const char*);
    } else if (project_level <= 6) {
        thoughts_array = _act_one_part_two_thoughts;
        thought_count = sizeof(_act_one_part_two_thoughts) / sizeof(const char*);
    } else if (project_level <= 10) {
        thoughts_array = _act_one_part_three_thoughts;
        thought_count = sizeof(_act_one_part_three_thoughts) / sizeof(const char*);
    }
    // Act II: The Labyrinth's Nature (Levels 11-20)
    else if (project_level <= 15) {
        thoughts_array = _act_two_part_one_thoughts;
        thought_count = sizeof(_act_two_part_one_thoughts) / sizeof(const char*);
    } else if (project_level <= 20) {
        thoughts_array = _act_two_part_two_thoughts;
        thought_count = sizeof(_act_two_part_two_thoughts) / sizeof(const char*);
    }
    // Act III: The Cracks Appear (Levels 21-30)
    else if (project_level <= 25) {
        thoughts_array = _act_three_part_one_thoughts;
        thought_count = sizeof(_act_three_part_one_thoughts) / sizeof(const char*);
    } else if (project_level <= 30) {
        thoughts_array = _act_three_part_two_thoughts;
        thought_count = sizeof(_act_three_part_two_thoughts) / sizeof(const char*);
    }
    // Act IV: Inescapable Truths (Levels 31-40)
    else if (project_level <= 35) {
        thoughts_array = _act_four_part_one_thoughts;
        thought_count = sizeof(_act_four_part_one_thoughts) / sizeof(const char*);
    } else if (project_level <= 40) {
        thoughts_array = _act_four_part_two_thoughts;
        thought_count = sizeof(_act_four_part_two_thoughts) / sizeof(const char*);
    }
    // Act V: The Eternal Cycle (Levels 41-50)
    else if (project_level <= 42) {
        thoughts_array = _act_five_part_one_a_thoughts;
        thought_count = sizeof(_act_five_part_one_a_thoughts) / sizeof(const char*);
    } else if (project_level <= 44) {
        thoughts_array = _act_five_part_one_b_thoughts;
        thought_count = sizeof(_act_five_part_one_b_thoughts) / sizeof(const char*);
    } else if (project_level <= 47) {
        thoughts_array = _act_five_part_two_thoughts;
        thought_count = sizeof(_act_five_part_two_thoughts) / sizeof(const char*);
    } else { // Levels 48 and beyond
        thoughts_array = _act_five_part_three_thoughts;
        thought_count = sizeof(_act_five_part_three_thoughts) / sizeof(const char*);
    }

    // If for some reason we have no thoughts, return an empty string.
    if (thought_count == 0) {
        return "";
    }

    // --- Step 2: Choose an index, ensuring it's not the same as the last one ---
    int ideal_index = (project_level + test_passed) % thought_count;
    const char* chosen_thought = thoughts_array[ideal_index];

    // THE NEW LOGIC: If the chosen thought is the same as the last, and we have other options...
    if (thought_count > 1 && chosen_thought == _sisyphus_last_inner_thought) {
        // ...then nudge the index to the next one in the list.
        int nudged_index = (ideal_index + 1) % thought_count;
        chosen_thought = thoughts_array[nudged_index];
    }

    // --- Step 3: Remember the thought we're about to show and return it ---
    _sisyphus_last_inner_thought = chosen_thought;
    return chosen_thought;
}

// Failure tracking
static char _sisyphus_failed_tests[10][256];
static char _sisyphus_failed_messages[10][512];
static char _sisyphus_failed_locations[10][256];
static int _sisyphus_failed_count = 0;
static int _sisyphus_current_test_penalty = 0;
static int _sisyphus_xp_from_failures = 0;
// Combo message tracking for condensing repeated messages
static char _sisyphus_last_combo_message[512] = "";
static int _sisyphus_same_message_count = 0;
static int _sisyphus_first_combo_of_sequence = 0;
static int _sisyphus_suppressing_combos = 0;
// Debug message suppression for repeated spam
static char _sisyphus_last_debug_message[512] = "";
static int _sisyphus_debug_repeat_count = 0;
static int _sisyphus_debug_suppressing = 0;
static FILE* _sisyphus_original_stdout = NULL;
static FILE* _sisyphus_original_stderr = NULL;

// Custom file descriptor that filters repeated messages
static int _sisyphus_filter_repeated_output(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check if this is the same message as before
    if (strstr(buffer, _sisyphus_last_debug_message) != NULL && strlen(_sisyphus_last_debug_message) > 0) {
        _sisyphus_debug_repeat_count++;
        if (_sisyphus_debug_repeat_count == 3) {
            fprintf(_sisyphus_original_stdout, "%s", buffer);
            fprintf(_sisyphus_original_stdout, TEST_COLOR_GRAY "... (suppressing repeated messages)\n" TEST_COLOR_RESET);
            _sisyphus_debug_suppressing = 1;
        } else if (_sisyphus_debug_repeat_count < 3) {
            fprintf(_sisyphus_original_stdout, "%s", buffer);
        }
        // If > 3, suppress
    } else {
        // Different message
        if (_sisyphus_debug_suppressing && _sisyphus_debug_repeat_count > 3) {
            fprintf(_sisyphus_original_stdout, TEST_COLOR_GRAY "(suppressed %d repeated messages)\n" TEST_COLOR_RESET, _sisyphus_debug_repeat_count - 3);
        }
        _sisyphus_debug_repeat_count = 1;
        _sisyphus_debug_suppressing = 0;
        strncpy(_sisyphus_last_debug_message, buffer, sizeof(_sisyphus_last_debug_message) - 1);
        fprintf(_sisyphus_original_stdout, "%s", buffer);
    }
    return strlen(buffer);
}
// Motivational quotes for failed test suites
static const char* _motivational_quotes[] = {
    "💎 HERCULEAN STRENGTH! Your twelve labors of debugging continue!",
    "🌱 PERSEPHONE'S RETURN! From darkness comes renewed growth!",
    "🎪 ODYSSEY CONTINUES! Every hero faces trials before triumph!",
    "🦾 TITAN STRENGTH! Growing mightier with each challenge!",
    "🧩 SPHINX'S RIDDLE! You love solving the impossible, don't you?",
    "✨ SPARTAN SPIRIT! This code shall not pass without a fight!",
    "🏛️ EPICTETUS REMINDS! 'It's not what happens, but how you react!'",
    "📚 SENECA'S WISDOM! 'Every new beginning comes from some other beginning's end!'",
    "✨ MARCUS AURELIUS! 'The impediment to action advances action. What stands in the way becomes the way!'",
    "🎯 ARISTOTLE'S VIRTUE! Excellence is not an act, but a habit - keep coding!",
    "🌟 SOCRATIC METHOD! Question everything, improve iteratively!",
    "🧘 STOIC PHILOSOPHER! Wisdom lies in consistent excellence!",
    "⚖️ THEMIS BALANCE! Divine justice maintains perfect equilibrium!",
    "🧘 EPICTETUS TEACHES! Focus only on what you can control - your code!",
    "⚖️ ARISTOTLE'S MEAN! Perfect balance between haste and contemplation!",
    "🏛️ SOCRATIC WISDOM! 'I know that I know nothing' - but progress continues!",
    "📜 MARCUS AURELIUS! 'Confine yourself to the present' - this moment, this code!",
    "🌊 ZENO'S PARADOX! Each step brings you infinitely closer to perfection!",
    "🎭 PLATO'S CAVE! Your debugging reflects eternal Forms of Truth!",
    "🔥 PROMETHEUS' GIFT! Divine fire illuminates the path forward!",
    "🦉 ATHENA'S WISDOM! Strategic debugging guided by divine intellect!",
    "⭐ APOLLO'S AIM! Each bug elimination precise as golden arrows!",
    "🌟 OLYMPIAN PERSEVERANCE! Even gods faced challenges before victory!"
};
static const int _motivational_quotes_count = sizeof(_motivational_quotes) / sizeof(_motivational_quotes[0]);

// --- Sisyphus Achievements ---
// Flags to ensure achievements are unlocked only once per suite.
static int _achievement_cornerstone = 0;
static int _achievement_the_phalanx = 0;
static int _achievement_daedalus_blueprint = 0;
static int _achievement_artemis_focus = 0;
static int _achievement_perfect_stride = 0;
static int _achievement_perseus_aegis = 0;
static int _achievement_atalantas_sprint = 0;
static int _achievement_golden_fleece = 0;
static int _achievement_hermes_caduceus = 0;
static int _achievement_laughing_sisyphus = 0;
static int _achievement_hydras_blood = 0;
static int _achievement_odysseus_return = 0;
static int _achievement_gordian_knot = 0;
static int _achievement_asclepius_cure = 0;
static int _achievement_theseus_thread = 0;
static int _achievement_platos_ideal_form = 0;
static int _achievement_midas_touch = 0;
static int _achievement_hidden_passage = 0;
static int _achievement_tyches_favor = 0;
static int _achievement_oracle_at_delphi = 0;

// Helper flags for tracking achievement states
static int _sisyphus_first_test_failed = -1; // -1: unset, 0: passed, 1: failed
static int _sisyphus_first_assert_passed = -1; // -1: unset, 0: failed, 1: passed
static double _sisyphus_min_test_time = 999.0;
static double _sisyphus_max_test_time = 0.0;

// Best achievement tracking
static char _sisyphus_best_achievement[64] = "";
static int _sisyphus_best_achievement_xp = 0;

// Track achievements unlocked this run for random display
static char _sisyphus_unlocked_achievements[20][256];
static char _sisyphus_unlocked_descriptions[20][256];
static int _sisyphus_unlocked_xp[20];
static int _sisyphus_unlocked_count = 0;

// Helper macro for tracking best achievement and storing unlocked ones
#define UPDATE_BEST_ACHIEVEMENT(name, xp, desc) \
    do { \
        if ((xp) > _sisyphus_best_achievement_xp) { \
            _sisyphus_best_achievement_xp = (xp); \
            strncpy(_sisyphus_best_achievement, (name), sizeof(_sisyphus_best_achievement) - 1); \
        } \
        if (_sisyphus_unlocked_count < 20) { \
            snprintf(_sisyphus_unlocked_achievements[_sisyphus_unlocked_count], 256, "%s", (name)); \
            snprintf(_sisyphus_unlocked_descriptions[_sisyphus_unlocked_count], 256, "%s", (desc)); \
            _sisyphus_unlocked_xp[_sisyphus_unlocked_count] = (xp); \
            _sisyphus_unlocked_count++; \
        } \
    } while(0)

// Progress tracking
static double _sisyphus_fastest_test = 999.0;
static int _sisyphus_max_combo = 0;
static int _sisyphus_consecutive_fast_tests = 0;
static int _sisyphus_current_perfect_streak = 0;

// Function to format numbers with commas (e.g., 12345 -> 12,345)
static void format_number_with_commas(char* buffer, size_t buffer_size, int number) {
    char temp[32];
    snprintf(temp, sizeof(temp), "%d", number);

    int len = strlen(temp);
    int comma_positions = (len - 1) / 3;
    int new_len = len + comma_positions;

    if (new_len >= (int)buffer_size) {
        // Fallback if buffer too small
        snprintf(buffer, buffer_size, "%d", number);
        return;
    }

    buffer[new_len] = '\0';

    int temp_pos = len - 1;
    int buffer_pos = new_len - 1;
    int digit_count = 0;

    while (temp_pos >= 0) {
        if (digit_count == 3) {
            buffer[buffer_pos--] = ',';
            digit_count = 0;
        }
        buffer[buffer_pos--] = temp[temp_pos--];
        digit_count++;
    }
}

// Basic utility functions first
static int _get_xp_for_level(int level) {
    // Battle Arena XP Requirements: Level 1=1500, 2=3200, 3=5700, 4=9200, 5=13900, etc.
    // Reduced requirements for more achievable Suite XP progression
    return 600 + (level * level * level * 98) + (level * level * 296) + (level * 294);
}

static int _get_current_level_from_xp(int total_xp) {
    int level = 1;
    int cumulative_xp = 0;
    while (cumulative_xp + _get_xp_for_level(level) <= total_xp) {
        cumulative_xp += _get_xp_for_level(level);
        level++;
    }
    return level;
}

static int _get_xp_in_current_level(int total_xp, int current_level) {
    int cumulative_xp = 0;
    for (int i = 1; i < current_level; i++) {
        cumulative_xp += _get_xp_for_level(i);
    }
    return total_xp - cumulative_xp;
}

// Count total expected tests dynamically
static int _count_expected_tests(void) {
    // Count RUN_TEST calls in current function by parsing source (simplified)
    // For now, use a reasonable default and auto-adjust
    return 8; // Will be updated dynamically as tests run
}

static void _display_progress_bar(int current, int total, const char* label, const char* color) {
    int width = 20;
    int filled = (current * width) / (total > 0 ? total : 1);
    printf("%s🎯 %s [", color, label);
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("█");
        else printf("░");
    }
    printf("] %d/%d" TEST_COLOR_RESET "\n", current, total);
}

static void _display_progress_bar_with_level(int current, int total, const char* label, const char* color, int level, int for_project) {
    int width = 20;
    int filled = (current * width) / (total > 0 ? total : 1);
    char current_str[32], total_str[32], _sisyphus_total_project_xp_str[32], _sisyphus_xp_earned_str[32];
    format_number_with_commas(current_str, sizeof(current_str), current);
    format_number_with_commas(total_str, sizeof(total_str), total);
    format_number_with_commas(_sisyphus_total_project_xp_str, sizeof(_sisyphus_total_project_xp_str), _sisyphus_total_project_xp);
    format_number_with_commas(_sisyphus_xp_earned_str, sizeof(_sisyphus_xp_earned_str), _sisyphus_xp_earned);
    if (for_project) {
        printf("%s %s🎯 %s [", _sisyphus_total_project_xp_str, color, label);
    } else {
        printf("%s %s🎯 %s [", _sisyphus_xp_earned_str, color, label);
    }
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("█");
        else printf("░");
    }
    printf("] %s/%s " TEST_COLOR_BOLD_WHITE "(Level %d)" TEST_COLOR_RESET "\n", current_str, total_str, level);
}

// Read existing project-wide XP from sisyphus.legend
static void _read_project_xp(void) {
    FILE *fp = fopen("sisyphus.legend", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "TOTAL_PROJECT_XP=", 17) == 0) {
                _sisyphus_total_project_xp = atoi(line + 17);
            } else if (strncmp(line, "PROJECT_LEVEL=", 14) == 0) {
                _sisyphus_project_level = atoi(line + 14);
            }
        }
        fclose(fp);
    }
    // Set current level to match project level to prevent resets
    _sisyphus_current_level = _sisyphus_project_level;
}

// Update sisyphus.legend with battle spoils (preserve existing XP)
static void _update_project_xp(void) {
    // Only add earned XP, don't reset existing progress
    int new_total_xp = _sisyphus_total_project_xp + _sisyphus_xp_earned;
    int new_level = _get_current_level_from_xp(new_total_xp);

    // Read existing legend file
    FILE *read_fp = fopen("sisyphus.legend", "r");
    FILE *write_fp = fopen("sisyphus.legend.tmp", "w");
    if (!write_fp) return;

    char line[256];
    int found_xp = 0, found_level = 0;

    if (read_fp) {
        while (fgets(line, sizeof(line), read_fp)) {
            if (strncmp(line, "TOTAL_PROJECT_XP=", 17) == 0) {
                fprintf(write_fp, "TOTAL_PROJECT_XP=%d\n", new_total_xp);
                found_xp = 1;
            } else if (strncmp(line, "PROJECT_LEVEL=", 14) == 0) {
                fprintf(write_fp, "PROJECT_LEVEL=%d\n", new_level);
                found_level = 1;
            } else {
                fputs(line, write_fp);
            }
        }
        fclose(read_fp);
    }

    // Add missing fields
    if (!found_xp) fprintf(write_fp, "TOTAL_PROJECT_XP=%d\n", new_total_xp);
    if (!found_level) fprintf(write_fp, "PROJECT_LEVEL=%d\n", new_level);

    fclose(write_fp);
    rename("sisyphus.legend.tmp", "sisyphus.legend");

    // Update global variables
    _sisyphus_total_project_xp = new_total_xp;
    _sisyphus_project_level = new_level;
}

// Sisyphus file integration functions
static void _write_sisyphus_mini_stats(void) {
    // Create .sisyphus directory if it doesn't exist
    system("mkdir -p .sisyphus");

    FILE *fp = fopen(".sisyphus/mini_stats.tmp", "w");
    if (fp) {
        fprintf(fp, "SUITE_XP=%d\n", _sisyphus_xp_earned);
        fprintf(fp, "SUITE_LEVEL=%d\n", _get_current_level_from_xp(_sisyphus_xp_earned));
        fprintf(fp, "SUITE_COMBO=%d\n", _sisyphus_max_combo);
        fprintf(fp, "SUITE_SPEED=%.6f\n", _sisyphus_fastest_test);
        fprintf(fp, "SUITE_ACHIEVEMENTS=%d\n", _sisyphus_achievements_unlocked);
        fprintf(fp, "SUITE_NAME=%s\n", _sisyphus_suite_name);
        fprintf(fp, "SUITE_TESTS_PASSED=%d\n", _sisyphus_trials_conquered);
        fprintf(fp, "SUITE_STREAK=%d\n", _sisyphus_perfect_streak);
        fprintf(fp, "SUITE_XP_FROM_TESTS=%d\n", _sisyphus_xp_from_tests);
        fprintf(fp, "SUITE_XP_FROM_COMBOS=%d\n", _sisyphus_xp_from_combos);
        fprintf(fp, "SUITE_XP_FROM_ACHIEVEMENTS=%d\n", _sisyphus_xp_from_achievements);
        fclose(fp);
    }
}

// Sisyphus refined achievement system
static void _check_achievements(void) {
    if (_sisyphus_is_full_test_mode) return;

    // --- Perfection & Endurance ---
    if (!_achievement_cornerstone && _sisyphus_first_assert_passed == 1 && tests_failed == 0) {
        _achievement_cornerstone = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 30;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_WHITE "🏆 ACHIEVEMENT: The Cornerstone " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For laying a perfect foundation and building a flawless structure upon it." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Cornerstone", bonus_xp, "Laid a perfect foundation for a flawless run.");
    }
    if (!_achievement_the_phalanx && _sisyphus_perfect_streak >= 3 && tests_failed == 0) {
        _achievement_the_phalanx = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 40;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT: The Phalanx " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For a perfect streak of 3 test functions, an unbreakable formation." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Phalanx", bonus_xp, "A perfect streak of 3 test functions.");
    }
    if (!_achievement_daedalus_blueprint && tests_failed == 0 && total_tests >= 7) {
        _achievement_daedalus_blueprint = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 70;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT: Daedalus's Blueprint " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For flawlessly executing a grand design of 7 or more test functions." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Daedalus's Blueprint", bonus_xp, "Flawlessly executed a grand design.");
    }
    if (!_achievement_artemis_focus && tests_failed == 0 && _sisyphus_min_test_time > 0.000050) {
        // This is a placeholder; requires logic to check if ALL tests are > 50us
    }

    // --- Speed & Efficiency ---
    if (!_achievement_perseus_aegis && tests_failed == 0 && total_tests >= 5 && _total_test_time < 0.000150) {
        _achievement_perseus_aegis = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 65;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BOLD_WHITE "🏆 ACHIEVEMENT: Perseus's Aegis " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    A perfect defense against a worthy foe (%d tests in under 150µs)." TEST_COLOR_RESET "\n", total_tests);
        UPDATE_BEST_ACHIEVEMENT("Perseus's Aegis", bonus_xp, "A perfect defense against a worthy foe.");
    }
    if (!_achievement_atalantas_sprint && _sisyphus_consecutive_fast_tests >= 5) {
        _achievement_atalantas_sprint = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 45;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT: Atalanta's Sprint " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For completing 5+ consecutive test functions with the speed of a huntress." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Atalanta's Sprint", bonus_xp, "5+ consecutive tests with the speed of a huntress.");
    }
    if (!_achievement_golden_fleece && _sisyphus_fastest_test < 0.000005) {
        _achievement_golden_fleece = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 90;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT: The Golden Fleece " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For obtaining a prize of mythical speed (< 5 microseconds)." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Golden Fleece", bonus_xp, "Obtained a prize of mythical speed.");
    }


    // --- Resilience & Absurdity ---
    if (!_achievement_laughing_sisyphus && _sisyphus_first_test_failed == 1 && tests_failed == 1) {
        _achievement_laughing_sisyphus = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 80;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_ORANGE "🏆 ACHIEVEMENT: The Laughing Sisyphus " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For failing the first trial, yet pushing the boulder to the summit perfectly." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Laughing Sisyphus", bonus_xp, "Failed the first trial, then achieved perfection.");
    }
    if (!_achievement_hydras_blood && tests_failed >= 3 && tests_passed > (tests_failed * 2)) {
        _achievement_hydras_blood = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 35;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_RED "🏆 ACHIEVEMENT: The Hydra's Blood " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For being bloodied in battle (%d failures), but emerging victorious." TEST_COLOR_RESET "\n", tests_failed);
        UPDATE_BEST_ACHIEVEMENT("The Hydra's Blood", bonus_xp, "Bloodied by many failures, but emerged victorious.");
    }
    if (!_achievement_odysseus_return && total_tests >= 7 && tests_failed >= 3 && _sisyphus_perfect_streak > 0) {
        // We check if the last test was a success by checking the overall perfect streak counter
        _achievement_odysseus_return = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 60;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT: Odysseus's Return " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For enduring a long, arduous journey and returning home a victor." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Odysseus's Return", bonus_xp, "Endured a long journey and returned a victor.");
    }
    if (!_achievement_gordian_knot && tests_failed >= 2) {
         // Simplified: just failing two different test functions is the knot.
        _achievement_gordian_knot = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 50;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT: The Gordian Knot " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For facing a complex, multi-faceted problem with at least two points of failure." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Gordian Knot", bonus_xp, "Faced a complex problem with multiple failures.");
    }
     if (!_achievement_asclepius_cure && tests_failed == 1 && _sisyphus_failed_count == 1) {
        _achievement_asclepius_cure = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 30;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT: Asclepius's Cure " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For identifying and curing a single, precise point of sickness in the code." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Asclepius's Cure", bonus_xp, "Cured a single, precise point of sickness.");
    }

    // --- Mastery & Craftsmanship ---
    if (!_achievement_theseus_thread && _sisyphus_max_combo >= 15) {
        _achievement_theseus_thread = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 50;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT: Theseus's Golden Thread " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For navigating a single, winding corridor of logic with a 15+ combo." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Theseus's Thread", bonus_xp, "Navigated a winding corridor with a 15+ combo.");
    }
    if (!_achievement_platos_ideal_form && tests_failed == 0 && _sisyphus_max_combo >= 30) {
        _achievement_platos_ideal_form = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 100;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BOLD_WHITE "🏆 ACHIEVEMENT: Plato's Ideal Form " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For a perfect suite reflecting profound mastery (30+ combo)." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Plato's Ideal Form", bonus_xp, "A perfect suite reflecting profound mastery.");
    }
    if (!_achievement_midas_touch && (_sisyphus_xp_earned - _sisyphus_xp_from_achievements) < 750 && _sisyphus_xp_earned >= 750) { // Check if this achievement pushes it over
        _achievement_midas_touch = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 70;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT: The Midas Touch " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For turning a simple test run into a treasure trove of over 750 XP." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Midas Touch", bonus_xp, "Turned a test run into a treasure trove of >750 XP.");
    }
    if (!_achievement_hidden_passage && _sisyphus_max_combo >= 20) {
        _achievement_hidden_passage = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 75;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT: The Hidden Passage " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For exploring a function so deeply you achieved a 20+ combo." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Hidden Passage", bonus_xp, "Explored a function so deeply (20+ combo).");
    }

    // --- Meta Achievements (Checked Last) ---
    if (!_achievement_tyches_favor && _sisyphus_achievements_unlocked >= 3) {
        _achievement_tyches_favor = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 25;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT: Tyche's Favor " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For a run so successful that Fortune herself smiles upon you (3+ achievements)." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Tyche's Favor", bonus_xp, "A lucky run blessed by Fortune (3+ achievements).");
    }
    if (!_achievement_oracle_at_delphi && tests_failed == 0 && _sisyphus_achievements_unlocked >= 2 && _total_test_time < 0.000100) {
        _achievement_oracle_at_delphi = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 125;
        _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT: The Oracle at Delphi " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "    For a perfect, swift, and multi-faceted prophecy of success." TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("The Oracle at Delphi", bonus_xp, "A perfect, swift, multi-faceted prophecy.");
    }
}

static void _level_up_check(void) {
    int old_level = _sisyphus_current_level;
    _sisyphus_current_level = _get_current_level_from_xp(_sisyphus_xp_earned);

    if (_sisyphus_current_level > old_level) {
        printf(TEST_COLOR_PURPLE "\n🌟 LEVEL UP! Welcome to Level %d!" TEST_COLOR_RESET "\n", _sisyphus_current_level);
        if (_sisyphus_current_level >= 10) {
            printf(TEST_COLOR_YELLOW "✨ LEGENDARY TIER! You have ascended to godlike mastery!" TEST_COLOR_RESET "\n");
        } else if (_sisyphus_current_level >= 5) {
            printf(TEST_COLOR_CYAN "🏛️ HEROIC TIER! The gods themselves take notice!" TEST_COLOR_RESET "\n");
        } else {
            printf(TEST_COLOR_GREEN "✨ Your mastery of the divine trials grows stronger!" TEST_COLOR_RESET "\n");
        }
    }
}

// High-resolution timer function
static double get_time_precise(void) {
#if USE_HIGH_RES_TIMER
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

#define LOOP_TEST_START() \
    do { \
        _sisyphus_in_loop = 1; \
        _sisyphus_loop_iteration = 0; \
        printf(TEST_COLOR_CYAN "  🌀 Entering Repetitive Trial..." TEST_COLOR_RESET "\n"); \
    } while(0)

#define LOOP_TEST_END() \
    do { \
        if (_sisyphus_loop_iteration >= 3) { \
            printf(TEST_COLOR_GRAY "      ... (condensed %d subsequent messages)\n" TEST_COLOR_RESET, _sisyphus_loop_iteration - 3); \
        } \
        printf(TEST_COLOR_CYAN "    Repetitive Trial Complete. 🌀" TEST_COLOR_RESET "\n"); \
        _sisyphus_in_loop = 0; \
    } while(0)


#define TEST_ASSERT(condition, message) \
    do { \
        if (_sisyphus_first_assert_passed == -1) { \
            _sisyphus_first_assert_passed = (condition) ? 1 : 0; \
        } \
        \
        if (!(condition)) { \
            /* Failure logic doesn't change */ \
            printf(TEST_COLOR_RED "💥 TRIAL FAILED: %s" TEST_COLOR_RESET, message); \
            printf(TEST_COLOR_GRAY "\n      Line %d in %s" TEST_COLOR_RESET "\n", __LINE__, __FILE__); \
            if (_sisyphus_failed_count < 10) { \
                snprintf(_sisyphus_failed_tests[_sisyphus_failed_count], 256, "%s", __func__); \
                snprintf(_sisyphus_failed_messages[_sisyphus_failed_count], 512, "%s", message); \
                snprintf(_sisyphus_failed_locations[_sisyphus_failed_count], 256, "%s:%d", __FILE__, __LINE__); \
                _sisyphus_failed_count++; \
            } \
            _sisyphus_combo_multiplier = 1; \
            return 0; \
        } else { \
            _sisyphus_trials_conquered++; \
            int xp_gain = 0; \
            \
            if (_sisyphus_in_loop) { \
                /* --- IN-LOOP LOGIC --- */ \
                if (_sisyphus_loop_iteration < 3) { \
                    printf(TEST_COLOR_GREEN "      - Iter %d: %s" TEST_COLOR_RESET, _sisyphus_loop_iteration + 1, message); \
                    printf(TEST_COLOR_YELLOW " (+1 XP)" TEST_COLOR_RESET "\n"); \
                } \
                if (_sisyphus_loop_iteration == 2) { \
                    printf(TEST_COLOR_GRAY "          Sisyphus finds a rhythm... (condensing output)\n" TEST_COLOR_RESET); \
                } \
                xp_gain = 1; /* Award a single, non-decaying XP point for loop assertions */ \
                _sisyphus_xp_from_tests += xp_gain; \
                _sisyphus_loop_iteration++; \
            } else { \
                /* --- NORMAL COMBO LOGIC (WITH CORRECTED ATTRIBUTION) --- */ \
                int base_test_xp = 5; \
                int combo_bonus = 0; \
                if (_sisyphus_combo_multiplier <= 7) { \
                    combo_bonus = 2 * (_sisyphus_combo_multiplier - 1); \
                } else { \
                    int diminish_levels = _sisyphus_combo_multiplier - 7; \
                    int diminished_bonus = 12 - (diminish_levels * 3); /* Max combo bonus is 12 */ \
                    combo_bonus = (diminished_bonus > 0) ? diminished_bonus : 0; \
                } \
                xp_gain = base_test_xp + combo_bonus; \
                _sisyphus_xp_from_tests += base_test_xp; \
                _sisyphus_xp_from_combos += combo_bonus; \
                \
                printf(TEST_COLOR_GREEN "  - COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier, message); \
                printf(TEST_COLOR_YELLOW " (+%d XP)" TEST_COLOR_RESET "\n", xp_gain); \
                _sisyphus_combo_multiplier++; \
            } \
            \
            _sisyphus_xp_earned += xp_gain; \
            if (!_sisyphus_is_full_test_mode) { \
                _check_achievements(); \
            } \
        } \
    } while(0)


#define RUN_TEST(test_func) \
do { \
        total_tests++; \
        _sisyphus_combo_multiplier = 1; \
        if (!_sisyphus_is_full_test_mode) { \
            const char* narrative_message = _get_narrative_test_message(_sisyphus_project_level); \
            printf(TEST_COLOR_CYAN "\n%s" TEST_COLOR_RESET, narrative_message); \
            printf(TEST_COLOR_GRAY " [\e]8;;file://%s/%s\e\\%s\e]8;;\e\\ - '%s']" TEST_COLOR_RESET "\n", \
                   getenv("PWD") ? getenv("PWD") : ".", __FILE__, __FILE__, #test_func); \
        } else { \
            printf(TEST_COLOR_GRAY "•"); \
        } \
        double start_time = get_time_precise(); \
        int test_result = test_func(); \
        double end_time = get_time_precise(); \
        double elapsed_time = end_time - start_time; \
        _total_test_time += elapsed_time; \
        if (elapsed_time < _sisyphus_fastest_test) _sisyphus_fastest_test = elapsed_time; \
        if (elapsed_time > _sisyphus_max_test_time) _sisyphus_max_test_time = elapsed_time; \
        if (elapsed_time < 0.000020) { \
                _sisyphus_consecutive_fast_tests++; \
            } else { \
                _sisyphus_consecutive_fast_tests = 0; \
            } \
            if (_sisyphus_first_test_failed == -1) { \
                _sisyphus_first_test_failed = test_result ? 0 : 1; \
            } \
        if (test_result) { \
            tests_passed++; \
            _sisyphus_perfect_streak++; \
            _sisyphus_current_perfect_streak++; \
            /* Track max combo using combo multiplier reached during test */ \
            int max_combo_this_test = _sisyphus_combo_multiplier - 1; \
            if (max_combo_this_test > _sisyphus_max_combo) _sisyphus_max_combo = max_combo_this_test; \
            int battle_xp = _sisyphus_trials_conquered * 2; \
            int final_xp = _sisyphus_is_full_test_mode ? (battle_xp / 4) : battle_xp; \
            _sisyphus_xp_earned += final_xp; \
            _sisyphus_xp_from_tests += final_xp; \
            printf(TEST_COLOR_GREEN "✨ TRIAL COMPLETED: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_BOLD_WHITE " (%.6fs)" TEST_COLOR_RESET, elapsed_time); \
            if (_sisyphus_perfect_streak >= 3) { \
                printf(TEST_COLOR_PURPLE " 🔥STREAK x%d!" TEST_COLOR_RESET, _sisyphus_perfect_streak); \
            } \
            printf("\n"); \
            if (_sisyphus_trials_conquered > 0 && !_sisyphus_is_full_test_mode) { \
                printf(TEST_COLOR_PURPLE "   🏛️  Explored %d passages! " TEST_COLOR_RESET, _sisyphus_trials_conquered); \
                printf(TEST_COLOR_YELLOW "+%d Exploration XP" TEST_COLOR_RESET "\n", final_xp); \
            } \
            if (!_sisyphus_is_full_test_mode) { \
                /* Add inner dialogue for successful tests based on story progression */ \
                const char* inner_thought = _get_inner_dialogue(_sisyphus_project_level, tests_passed); \
                printf(TEST_COLOR_GRAY "\"%s\" " TEST_COLOR_RESET "\n", inner_thought); \
                _level_up_check(); \
            } \
            /* Add newline after successful test completion */ \
            if (!_sisyphus_is_full_test_mode) { \
                printf("\n"); \
            } \
        } else { \
            tests_failed++; \
            _sisyphus_beasts_slayed += _sisyphus_trials_conquered; \
            printf(TEST_COLOR_RED "💀 SETBACK ENDURED: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_BOLD_WHITE " (%.6fs)" TEST_COLOR_RESET "\n", elapsed_time); \
            if (_sisyphus_trials_conquered > 0) { \
                printf(TEST_COLOR_ORANGE "   🔥 But you got %d combo%s before failing!" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered, _sisyphus_trials_conquered == 1 ? "" : "s"); \
            } \
            if (!_sisyphus_is_full_test_mode) { \
                /* Add inner dialogue for failed tests based on story progression */ \
                const char* inner_thought = _get_inner_dialogue(_sisyphus_project_level, tests_passed); \
                printf(TEST_COLOR_GRAY "   \"%s\" " TEST_COLOR_RESET "You reflect on the setback, knowing the boulder must be pushed again.\n", inner_thought); \
            } \
            _sisyphus_perfect_streak = 0; \
            _sisyphus_current_perfect_streak = 0; \
            /* Add newline after failed test completion */ \
            if (!_sisyphus_is_full_test_mode) { \
                printf("\n"); \
            } \
        } \
        _sisyphus_trials_conquered = 0; \
        _sisyphus_combo_multiplier = 1; \
        /* End any suppressed combo sequence when test ends */ \
        if (_sisyphus_suppressing_combos && _sisyphus_same_message_count > 3) { \
            printf(TEST_COLOR_GREEN "   COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier - 1, _sisyphus_last_combo_message); \
            printf(TEST_COLOR_YELLOW " (+3 XP)" TEST_COLOR_RESET "\n"); \
        } \
        _sisyphus_suppressing_combos = 0; \
        _sisyphus_same_message_count = 0; \
        _sisyphus_last_combo_message[0] = '\0'; \
        if (!_sisyphus_is_full_test_mode) { \
            _check_achievements(); \
        } \
    } while(0)

// Epic Sisyphus gamified test suite header macro
#define TEST_SUITE_START(suite_name) \
    _test_suite_start_time = get_time_precise(); \
    _total_test_time = 0.0; \
    _sisyphus_beasts_slayed = 0; \
    _sisyphus_trials_conquered = 0; \
    _sisyphus_perfect_streak = 0; \
    _sisyphus_xp_earned = 0; \
    _sisyphus_combo_multiplier = 1; \
    _sisyphus_achievements_unlocked = 0; \
    _sisyphus_fastest_test = 999.0; \
    _sisyphus_max_combo = 0; \
    _sisyphus_total_expected_tests = _count_expected_tests(); \
    _sisyphus_is_full_test_mode = (getenv("SISYPHUS_FULL_TEST_MODE") != NULL); \
    _sisyphus_xp_from_tests = 0; \
    _sisyphus_xp_from_combos = 0; \
    _sisyphus_xp_from_achievements = 0; \
    _sisyphus_xp_from_failures = 0; \
    _sisyphus_unlocked_count = 0; \
    _sisyphus_consecutive_fast_tests = 0; \
    _sisyphus_current_perfect_streak = 0; \
    _sisyphus_failed_count = 0; \
    _sisyphus_current_test_penalty = 0; \
    _read_project_xp(); \
    strncpy(_sisyphus_suite_name, suite_name, sizeof(_sisyphus_suite_name) - 1); \
    if (!_sisyphus_is_full_test_mode) { \
        const char* suite_intro = _get_narrative_test_message(_sisyphus_project_level); \
        printf(TEST_COLOR_PURPLE "🏛️  LABYRINTH CONSTRUCTION: %s" TEST_COLOR_RESET "\n", suite_name); \
        printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
        printf(TEST_COLOR_GRAY "🪨 Each test pushes the boulder higher • Build the perfect maze" TEST_COLOR_RESET "\n"); \
        char total_xp_str[32], next_level_xp_str[32]; \
        format_number_with_commas(total_xp_str, sizeof(total_xp_str), _sisyphus_total_project_xp); \
        format_number_with_commas(next_level_xp_str, sizeof(next_level_xp_str), _get_xp_for_level(_sisyphus_project_level + 1)); \
        printf(TEST_COLOR_CYAN "🏗️  Architect Level: %d | Total Construction XP: %s | Next Level: %s XP" TEST_COLOR_RESET "\n", _sisyphus_project_level, total_xp_str, next_level_xp_str); \
    } else { \
        printf(TEST_COLOR_GRAY "🏗️  Workshop Session: %s" TEST_COLOR_RESET " ", suite_name); \
    }

#define TEST_SUITE_END() \
    double _suite_end_time = get_time_precise(); \
    double _total_suite_time = _suite_end_time - _test_suite_start_time; \
    if (_sisyphus_loop_iteration_count > 3) { \
            printf(TEST_COLOR_GRAY "      ... (suppressed %d similar messages)\n" TEST_COLOR_RESET, _sisyphus_loop_iteration_count - 3); \
        } \
        if (!_achievement_perfect_stride && tests_failed == 0 && total_tests > 3 && (_sisyphus_max_test_time - _sisyphus_fastest_test) < 0.000030) { \
                _achievement_perfect_stride = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 50; \
                _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp; \
                printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT: The Perfect Stride " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp); \
                printf(TEST_COLOR_GRAY "    For incredible consistency; every step as perfect as the last." TEST_COLOR_RESET "\n"); \
                UPDATE_BEST_ACHIEVEMENT("The Perfect Stride", bonus_xp, "Incredible consistency between all tests."); \
            } \
            \
            double overhead_time = _total_suite_time - _total_test_time; \
            if (!_achievement_hermes_caduceus && tests_failed == 0 && overhead_time < (_total_test_time * 0.5)) { \
                _achievement_hermes_caduceus = 1; _sisyphus_achievements_unlocked++; int bonus_xp = 55; \
                _sisyphus_xp_earned += bonus_xp; _sisyphus_xp_from_achievements += bonus_xp; \
                printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT: Hermes's Caduceus " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp); \
                printf(TEST_COLOR_GRAY "    A divinely efficient run where the framework overhead was minimal." TEST_COLOR_RESET "\n"); \
                UPDATE_BEST_ACHIEVEMENT("Hermes's Caduceus", bonus_xp, "A divinely efficient run."); \
            } \
            /* --- END OF ACHIEVEMENT LOGIC --- */ \
            \
            if (_sisyphus_loop_iteration_count > 3) { \
                printf(TEST_COLOR_GRAY "      ... (condensed %d subsequent messages)\n" TEST_COLOR_RESET, _sisyphus_loop_iteration_count - 3); \
            } \
            \
    /* Apply XP penalties for failures: -100, -200, -300, etc. */ \
    int xp_penalty = 0; \
    if (tests_failed > 0) { \
        for (int i = 1; i <= tests_failed; i++) { \
            xp_penalty += i * 100; \
        } \
        int original_xp = _sisyphus_xp_earned; \
        _sisyphus_xp_earned = (_sisyphus_xp_earned > xp_penalty) ? _sisyphus_xp_earned - xp_penalty : 0; \
        _sisyphus_xp_from_failures = -xp_penalty; \
        if (xp_penalty > 0) { \
            char penalty_str[32], original_str[32], final_str[32]; \
            format_number_with_commas(penalty_str, sizeof(penalty_str), xp_penalty); \
            format_number_with_commas(original_str, sizeof(original_str), original_xp); \
            format_number_with_commas(final_str, sizeof(final_str), _sisyphus_xp_earned); \
            printf(TEST_COLOR_RED "💀 FAILURE PENALTY: -%s XP (from %d failure%s)" TEST_COLOR_RESET "\n", penalty_str, tests_failed, tests_failed == 1 ? "" : "s"); \
            printf(TEST_COLOR_GRAY "   Original XP: %s → Final XP: %s" TEST_COLOR_RESET "\n", original_str, final_str); \
        } \
    } \
    _update_project_xp(); \
    _write_sisyphus_mini_stats(); \
    printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
    printf(TEST_COLOR_PURPLE "🏛️  WORKSHOP PROGRESS - LABYRINTH CONSTRUCTION REPORT" TEST_COLOR_RESET "\n"); \
    printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
    _display_progress_bar(tests_passed, total_tests, "Victory Rate", TEST_COLOR_GREEN); \
    printf(TEST_COLOR_CYAN "🧪 Maze Sections Built: %d" TEST_COLOR_RESET " | ", total_tests); \
    printf(TEST_COLOR_GREEN "🏗️  Completed: %d" TEST_COLOR_RESET " | ", tests_passed); \
    printf(TEST_COLOR_RED "🪨 Needs Rework: %d" TEST_COLOR_RESET "\n", tests_failed); \
    int current_level_xp = _get_xp_in_current_level(_sisyphus_xp_earned, _sisyphus_current_level); \
    int xp_needed_for_next = _get_xp_for_level(_sisyphus_current_level); \
    _display_progress_bar_with_level(current_level_xp, xp_needed_for_next, "Suite XP", TEST_COLOR_YELLOW, _sisyphus_current_level, 0); \
    int project_level_xp = _get_xp_in_current_level(_sisyphus_total_project_xp, _sisyphus_project_level); \
    int project_xp_needed = _get_xp_for_level(_sisyphus_project_level); \
    _display_progress_bar_with_level(project_level_xp, project_xp_needed, "Project XP", TEST_COLOR_PURPLE, _sisyphus_project_level, 1); \
    printf( "⏱️  Fastest Trial: %.6fs" TEST_COLOR_RESET " | ", _sisyphus_fastest_test < 999.0 ? _sisyphus_fastest_test : 0.0); \
    printf(TEST_COLOR_GREEN "🏆 Achievements: %d" TEST_COLOR_RESET, _sisyphus_achievements_unlocked); \
    if (_sisyphus_best_achievement_xp > 0) { \
        printf(TEST_COLOR_BOLD_WHITE " | Best Achievement: %s (%d XP)" TEST_COLOR_RESET, _sisyphus_best_achievement, _sisyphus_best_achievement_xp); \
    } \
    printf("\n"); \
    printf(TEST_COLOR_GRAY "🕒 Exploration Time: %.6fs | 🔧 Setup Time: %.6fs | 📊 Overhead: %.6fs" TEST_COLOR_RESET "\n", \
           _total_suite_time, _total_test_time, _total_suite_time - _total_test_time); \
    printf("⏱️  Total Test Time: %.6f seconds\n", _total_test_time); \
    if (tests_failed == 0) { \
        if (_sisyphus_unlocked_count > 0) { \
            int random_achievement = (_sisyphus_xp_earned + _sisyphus_max_combo + tests_passed) % _sisyphus_unlocked_count; \
            int bonus_xp = _sisyphus_unlocked_xp[random_achievement] * (_sisyphus_random_achievement_multi - 1); \
            _sisyphus_xp_earned += bonus_xp; \
            _sisyphus_xp_from_achievements += bonus_xp; \
            printf(TEST_COLOR_CYAN "\nRandom Achievement: 🏆 %s" TEST_COLOR_BOLD_WHITE " (%d XP)" TEST_COLOR_YELLOW " +%d BONUS XP!" TEST_COLOR_RESET "\n", \
                   _sisyphus_unlocked_achievements[random_achievement], _sisyphus_unlocked_xp[random_achievement], bonus_xp); \
            printf(TEST_COLOR_GRAY "   %s" TEST_COLOR_RESET "\n", _sisyphus_unlocked_descriptions[random_achievement]); \
        } \
        char total_project_xp_str[32], session_xp_str[32], test_xp_str[32], combo_xp_str[32], achievement_xp_str[32]; \
        format_number_with_commas(total_project_xp_str, sizeof(total_project_xp_str), _sisyphus_total_project_xp); \
        format_number_with_commas(session_xp_str, sizeof(session_xp_str), _sisyphus_xp_earned); \
        format_number_with_commas(test_xp_str, sizeof(test_xp_str), _sisyphus_xp_from_tests); \
        format_number_with_commas(combo_xp_str, sizeof(combo_xp_str), _sisyphus_xp_from_combos); \
        format_number_with_commas(achievement_xp_str, sizeof(achievement_xp_str), _sisyphus_xp_from_achievements); \
        printf(TEST_COLOR_BOLD_WHITE "🏛️  Project Total XP: %s" TEST_COLOR_RESET " | ", total_project_xp_str); \
        printf(TEST_COLOR_BOLD_WHITE "🌟 Project Level: %d" TEST_COLOR_RESET " | ", _sisyphus_project_level); \
        printf(TEST_COLOR_GRAY "📈 XP Gained This Session: +%s" TEST_COLOR_RESET "\n", session_xp_str); \
        printf(TEST_COLOR_CYAN "💰 XP from tests: %s, " TEST_COLOR_RED "XP from combos: %s," TEST_COLOR_YELLOW " XP from achievements: %s" TEST_COLOR_RESET "\n", test_xp_str, combo_xp_str, achievement_xp_str); \
        return 0; \
    } else { \
        if (_sisyphus_failed_count > 0) { \
            printf(TEST_COLOR_RED "\n════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
            printf(TEST_COLOR_RED "💀 BATTLE CASUALTIES - FAILED TRIALS" TEST_COLOR_RESET "\n"); \
            printf(TEST_COLOR_RED "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
            for (int i = 0; i < _sisyphus_failed_count; i++) { \
                printf(TEST_COLOR_RED "🔴 %s: %s" TEST_COLOR_RESET "\n", _sisyphus_failed_tests[i], _sisyphus_failed_messages[i]); \
                printf(TEST_COLOR_GRAY "   📍 %s" TEST_COLOR_RESET "\n", _sisyphus_failed_locations[i]); \
            } \
            printf(TEST_COLOR_RED "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
        } \
        printf(TEST_COLOR_ORANGE "\n🪨 THE BOULDER SLIPS! But Sisyphus always pushes again!" TEST_COLOR_RESET "\n"); \
        char level_xp_str[32]; \
        format_number_with_commas(level_xp_str, sizeof(level_xp_str), _sisyphus_xp_earned); \
        printf(TEST_COLOR_YELLOW "🏗️ Construction Level %d achieved with %s XP - The work continues!" TEST_COLOR_RESET "\n", _sisyphus_current_level, level_xp_str); \
        /* Display random motivational quote */ \
        int quote_index = (_sisyphus_xp_earned + tests_failed + tests_passed) % _motivational_quotes_count; \
        printf(TEST_COLOR_GRAY "📈 %s" TEST_COLOR_RESET "\n", _motivational_quotes[quote_index]); \
        if (_sisyphus_unlocked_count > 0) { \
            int random_achievement = (_sisyphus_xp_earned + _sisyphus_max_combo + tests_passed) % _sisyphus_unlocked_count; \
            int bonus_xp = _sisyphus_unlocked_xp[random_achievement] * (_sisyphus_random_achievement_multi - 1); \
            _sisyphus_xp_earned += bonus_xp; \
            _sisyphus_xp_from_achievements += bonus_xp; \
            printf(TEST_COLOR_CYAN "\nRandom Achievement: 🏆 %s" TEST_COLOR_BOLD_WHITE " (%d XP)" TEST_COLOR_YELLOW " +%d BONUS XP!" TEST_COLOR_RESET "\n", \
                   _sisyphus_unlocked_achievements[random_achievement], _sisyphus_unlocked_xp[random_achievement], bonus_xp); \
            printf(TEST_COLOR_GRAY "   %s" TEST_COLOR_RESET "\n", _sisyphus_unlocked_descriptions[random_achievement]); \
        } \
        char total_project_xp_str2[32], session_xp_str2[32], test_xp_str2[32], combo_xp_str2[32], achievement_xp_str2[32]; \
        format_number_with_commas(total_project_xp_str2, sizeof(total_project_xp_str2), _sisyphus_total_project_xp); \
        format_number_with_commas(session_xp_str2, sizeof(session_xp_str2), _sisyphus_xp_earned); \
        format_number_with_commas(test_xp_str2, sizeof(test_xp_str2), _sisyphus_xp_from_tests); \
        format_number_with_commas(combo_xp_str2, sizeof(combo_xp_str2), _sisyphus_xp_from_combos); \
        format_number_with_commas(achievement_xp_str2, sizeof(achievement_xp_str2), _sisyphus_xp_from_achievements); \
        printf(TEST_COLOR_BOLD_WHITE "🏛️  Project Total XP: %s" TEST_COLOR_RESET " | ", total_project_xp_str2); \
        printf(TEST_COLOR_BOLD_WHITE "🌟 Project Level: %d" TEST_COLOR_RESET " | ", _sisyphus_project_level); \
        printf(TEST_COLOR_GRAY "📈 XP Gained This Session: +%s" TEST_COLOR_RESET "\n", session_xp_str2); \
        printf(TEST_COLOR_CYAN "💰 XP from tests: %s, " TEST_COLOR_RED "XP from combos: %s," TEST_COLOR_YELLOW " XP from achievements: %s" TEST_COLOR_RESET "\n", test_xp_str2, combo_xp_str2, achievement_xp_str2); \
        return 1; \
    }

// Compact gamified version for when running multiple test files in the great test runner
#define TEST_SUITE_END_QUIET() \
    double _suite_end_time = get_time_precise(); \
    double _total_suite_time = _suite_end_time - _test_suite_start_time; \
    _update_project_xp(); \
    _write_sisyphus_mini_stats(); \
    printf(TEST_COLOR_BOLD_WHITE "⚔️  Trials: %d" TEST_COLOR_RESET " | ", total_tests); \
    printf(TEST_COLOR_GREEN "🏆 Won: %d" TEST_COLOR_RESET " | ", tests_passed); \
    printf(TEST_COLOR_RED "💀 Lost: %d" TEST_COLOR_RESET " | ", tests_failed); \
    printf(TEST_COLOR_YELLOW "💰 +%dXP" TEST_COLOR_RESET " | ", _sisyphus_xp_earned); \
    printf(TEST_COLOR_PURPLE "Lv.%d" TEST_COLOR_RESET " | ", _sisyphus_current_level); \
    printf(TEST_COLOR_CYAN "🔥 %dx" TEST_COLOR_RESET " | ", _sisyphus_max_combo); \
    printf(TEST_COLOR_BOLD_WHITE "🌟 ProjLv.%d" TEST_COLOR_RESET "\n", _sisyphus_project_level); \
    printf(TEST_COLOR_GRAY "⏱️  Exploration: %.6fs | 🏆 %d achievements | ✨ %.6fs fastest" TEST_COLOR_RESET "\n", \
           _total_test_time, _sisyphus_achievements_unlocked, _sisyphus_fastest_test < 999.0 ? _sisyphus_fastest_test : 0.0); \
    printf("⏱️  Total Test Time: %.6f seconds\n", _total_test_time); \
    char quiet_test_xp_str[32], quiet_combo_xp_str[32], quiet_achievement_xp_str[32], quiet_failure_xp_str[32]; \
    format_number_with_commas(quiet_test_xp_str, sizeof(quiet_test_xp_str), _sisyphus_xp_from_tests); \
    format_number_with_commas(quiet_combo_xp_str, sizeof(quiet_combo_xp_str), _sisyphus_xp_from_combos); \
    format_number_with_commas(quiet_achievement_xp_str, sizeof(quiet_achievement_xp_str), _sisyphus_xp_from_achievements); \
    format_number_with_commas(quiet_failure_xp_str, sizeof(quiet_failure_xp_str), _sisyphus_xp_from_failures); \
    printf(TEST_COLOR_CYAN "💰 XP from tests: %s, " TEST_COLOR_ORANGE "XP from combos: %s, " TEST_COLOR_YELLOW "XP from achievements: %s", quiet_test_xp_str, quiet_combo_xp_str, quiet_achievement_xp_str); \
    if (_sisyphus_xp_from_failures != 0) { \
        printf(TEST_COLOR_RED ", XP from failures: %s" TEST_COLOR_RESET "\n", quiet_failure_xp_str); \
    } else { \
        printf(TEST_COLOR_RESET "\n"); \
    } \
    if (tests_failed == 0) { \
        char perfect_xp_str[32]; \
        format_number_with_commas(perfect_xp_str, sizeof(perfect_xp_str), _sisyphus_xp_earned); \
        printf(TEST_COLOR_PURPLE "✨ Flawless construction! +%s XP added to Labyrinth Level %d!" TEST_COLOR_RESET "\n", perfect_xp_str, _sisyphus_project_level); \
        return 0; \
    } else { \
        char gained_xp_str[32]; \
        format_number_with_commas(gained_xp_str, sizeof(gained_xp_str), _sisyphus_xp_earned); \
        printf(TEST_COLOR_ORANGE "🪨 +%s XP earned! Labyrinth Level %d progress preserved!" TEST_COLOR_RESET "\n", gained_xp_str, _sisyphus_project_level); \
        return 1; \
    }

#endif
