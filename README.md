# Ex3B - Generic Markov Chain Implementation

## 🎯 Project Overview

This project is part of the **Hebrew University of Jerusalem** Computer Science curriculum - a comprehensive C programming exercise that demonstrates the implementation of a **generic Markov Chain library**. The project showcases advanced C programming concepts including generic programming, data structures, and memory management.

## 📋 Assignment Details

- **Course**: C/C++ Programming Workshop
- **Assignment**: Exercise 3 Part B - Generic Programming
- **Institution**: Hebrew University of Jerusalem, School of Engineering and Computer Science
- **Submission Deadline**: Wednesday, May 28, 2025, 22:00

## 🏗️ Project Structure

### Core Files

```
ex3b/
├── markov_chain.h          # Generic Markov Chain header with structs and function declarations
├── markov_chain.c          # Generic Markov Chain implementation
├── linked_list.h           # Linked list data structure header
├── linked_list.c           # Linked list implementation
├── tweets_generator.c      # Application 1: Tweet generation using strings
├── snakes_and_ladders.c    # Application 2: Game paths using Cell structures
└── Makefile               # Build system for both applications
```

## 🚀 Key Features

### 1. **Generic Data Structure Design**
- Implemented a fully generic Markov Chain that works with any data type
- Uses function pointers for type-specific operations (print, compare, copy, free)
- Demonstrates advanced C programming with `void*` pointers and function callbacks

### 2. **Memory Management Excellence**
- Dynamic memory allocation and deallocation
- Valgrind-clean implementation (no memory leaks)
- Proper error handling for allocation failures

### 3. **Dual Application Showcase**

#### A. Tweet Generator (`tweets_generator`)
- Generates random tweets based on text corpus analysis
- Works with string data using the generic Markov Chain
- Demonstrates text processing and natural language generation

#### B. Snakes and Ladders Path Generator (`snakes_and_ladders`)
- Simulates game board paths using Cell structures
- Shows generic library usage with custom data types
- Implements game logic with ladders, snakes, and dice mechanics

## 🛠️ Technical Implementation

### Generic Programming Approach

The project transforms a string-specific Markov Chain into a fully generic implementation:

```c
typedef struct MarkovChain {
    LinkedList *database;
    
    // Function pointers for generic operations
    print_func print_func;      // How to print the data type
    comp_func comp_func;        // How to compare two data elements
    free_data free_data;        // How to free the data type
    copy_func copy_func;        // How to copy the data type
    is_last is_last;           // How to determine sequence endings
} MarkovChain;
```

### Key Data Structures

- **MarkovNode**: Contains generic data and frequency list of next possible states
- **MarkovNodeFrequency**: Tracks transition frequencies between states
- **LinkedList**: Dynamic storage for the Markov Chain database

## 📊 Applications Demonstrated

### 1. Text Generation (Tweets)
```bash
./tweets_generator <seed> <num_tweets> <corpus_file>
```
- Learns from text corpus
- Generates coherent text sequences
- Handles sentence boundaries (periods)

### 2. Game Path Simulation (Snakes & Ladders)
```bash
./snakes_and_ladders <seed> <num_paths>
```
- Simulates board game mechanics
- Generates random valid game paths
- Handles special transitions (snakes/ladders)

## 🔧 Build System

The project includes a comprehensive Makefile with two targets:

```bash
# Build tweet generator
make tweets_generator

# Build snakes and ladders simulator
make snakes_and_ladders
```

## 🎯 Learning Objectives Achieved

- **Generic Programming**: Implementing data structures that work with any type
- **Function Pointers**: Using callbacks for type-specific operations
- **Memory Management**: Dynamic allocation, error handling, and cleanup
- **Modular Design**: Separating generic library from specific applications
- **Build Systems**: Using Make for project compilation

## 🧪 Testing & Quality Assurance

- Valgrind-verified memory management
- Automated testing with university test suite
- Error handling for edge cases and allocation failures
- Code style compliance checking

## 💡 Key Programming Concepts

1. **Abstraction**: Generic data structures hiding implementation details
2. **Encapsulation**: Clean separation between library and applications
3. **Polymorphism**: Function pointers enabling type-specific behavior
4. **Resource Management**: RAII-style memory handling in C

## 🎖️ Technical Achievements

- Successfully abstracted string-specific code to work with any data type
- Implemented clean separation of concerns between library and applications
- Demonstrated understanding of advanced C programming concepts
- Created robust, production-quality code with proper error handling

---

*This project demonstrates advanced C programming skills including generic programming, memory management, and modular design principles. It serves as an excellent example of transforming specific implementations into reusable, generic libraries.*
