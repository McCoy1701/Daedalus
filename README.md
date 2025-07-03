# 🏗️ Daedalus Library - Math, Data Structures, and Game Dev Utils
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![Greek Mythology](https://img.shields.io/badge/Powered%20by-Labyrinthine%20Logic-blue.svg)](https://en.wikipedia.org/wiki/Daedalus)
[![Performance](https://img.shields.io/badge/Fast%20Math-Carmack%20Approved-red.svg)](#)
[![MIDAS Toolchain](https://img.shields.io/badge/MIDAS%20Toolchain-Master%20Crafted-gold.svg)](#)

*High-Performance Linear Algebra and Mathematical Foundations for Divine Applications*

> *"I am the architect of impossibilities, the craftsman of precision. My tools transcend mortal limitations."* - Daedalus, Master of the Labyrinth

## What is This Madness?

**The Daedalus Library** is a master-crafted collection of high-performance mathematical tools and data structures that form the foundation of the **MIDAS Toolchain**. Born from the forge of divine geometry and tempered in the fires of real-time computation, these are the building blocks that outlast empires.

This isn't just another math library. This is **precision incarnate**—where John Carmack's legendary fast inverse square root meets the geometric perfection of ancient Greek mathematics, where every function is crafted with the obsessive attention to detail that built the original Labyrinth.

## Installation

### 🔧 Native Development (System-Wide)

```sh
git clone https://github.com/your-repo/daedalus.git
cd daedalus
make shared          # Build shared library
sudo make install    # Install to /usr/lib/ and /usr/include/
```

**Then, in your C projects:**

```c
#include <Daedalus.h>  // System-wide header
// Link with: gcc your_code.c -lDaedalus
```

### 🌐 Emscripten Web Projects

```sh
make EM              # Creates bin/libDaedalus.a static library
```

**Copy these files to your Emscripten project:**

`bin/libDaedalus.a` → your lib/ directory
`include/Daedalus.h` → your include/ directory

**Then compile with:**

```sh
emcc your_code.c lib/libDaedalus.a -I include/ -o output.html
```

### 🧹 Cleanup & Maintenance

```sh
make clean           # Remove all build artifacts
sudo make uninstall  # Remove system installation
```

### 🎮 Sample Transformations

**Before Daedalus (Dangerous C)**


```c
char* name = malloc(256);           // No error checking
strcpy(name, "Player");             // Buffer overflow risk  
strcat(name, " Character");         // More overflow risk
printf("Created: %s\n", name);      // No structured logging
free(name);                         // Manual cleanup required
```

**After Daedalus (Master-Crafted)**

```c
dString_t* name = d_InitString();
d_AppendString(name, "Player", 0);
d_AppendString(name, " Character", 0);  
d_LogInfoF("Created: %s", d_PeekString(name));
d_DestroyString(name);              // Automatic, safe cleanup
```

**The Result:** Code that doesn't just work—code that serves with the wisdom of the master architect himself.

## 🏛️ The Philosophy: Divine Craftsmanship

In the words of the master himself: *"Tools that work without their creator truly serve."* Every function in Daedalus is designed to operate with mechanical precision, requiring no intervention once deployed. These are not merely utilities—they are **architectural elements** for building digital worlds.

The Daedalus approach emphasizes:
- **🎯 Geometric Perfection**: Every calculation optimized for precision and performance
- **🔧 Master-Crafted APIs**: Interfaces designed by someone who understands the weight of abstraction
- **⚡ Lightning Performance**: Using techniques that would make ancient gods jealous
- **🏗️ Foundational Stability**: Building blocks that support entire digital civilizations
- **🌐 Modern Deployment**: C code that runs beautifully on the web through Emscripten

## 🎯 The MIDAS Connection

Daedalus serves as the mathematical foundation of the **MIDAS Collective**:

- **🧠 Metis**: Provides wisdom through scattered consciousness in the linter
- **⚡ Ixion**: Guards against memory corruption with eternal vigilance  
- **🏗️ Daedalus**: Crafts the mathematical tools that power everything
- **📐 Archimedes**: Builds user interfaces with geometric precision
- **🪨 Sisyphus**: Tests everything with persistent, joyful iteration

*Together, these tools transform the eternal struggle of programming into acts of digital divinity.*

## 🏗️ Built for the Modern Age

While honoring the classical principles of mathematical precision, Daedalus embraces the modern reality of web deployment. Every function is designed to compile efficiently to WebAssembly, bringing desktop-class mathematical performance to browser applications.

---

"From the forge of necessity comes tools of transcendence. These functions will outlive their creator, serving digital architects for generations yet to come." - The Testament of Daedalus
