# Daedalus

A high-performance C library providing linear algebra operations, data structures, and utility functions for game development, scientific computing, and real-time applications.

## Overview

Daedalus is a comprehensive math and utility library written in C that offers efficient implementations of vector mathematics, matrix operations, spatial data structures, and common programming utilities. The library is designed with performance in mind, featuring optimized algorithms like Quake's fast square root and providing both 2D and 3D mathematical operations.

## Features

### Linear Algebra
- **Vector Operations**: Complete 2D, 3D, and 4D vector mathematics
  - Addition, subtraction, scaling, normalization
  - Dot product, cross product, distance calculations
  - Vector limiting and directional operations
- **Matrix Operations**: Full 4x4 matrix support
  - Matrix multiplication, rotation, translation
  - Projection matrix creation
  - Matrix-vector transformations
- **Optimized Algorithms**: Fast square root using Quake's inverse square root algorithm

### Data Structures
- **Dynamic Arrays**: Resizable arrays with type safety
- **Linked Lists**: Generic linked list implementation with named nodes
- **Quad Trees**: Spatial partitioning for efficient collision detection and spatial queries

### Physics and Simulation
- **Kinematic Bodies**: 2D physics objects with position, velocity, acceleration, and mass
- **Spatial Partitioning**: Quad tree implementation for efficient spatial queries

### String Utilities
- **String Builder**: Efficient string construction and manipulation
- **File Operations**: Read entire files into strings
- **Safe String Operations**: Secure string handling with bounds checking

### Utility Functions
- **Mathematical Macros**: MIN, MAX, MAP, RANDF for common operations
- **Constants**: Mathematical constants like PI
- **Type Definitions**: Clean, consistent type naming
