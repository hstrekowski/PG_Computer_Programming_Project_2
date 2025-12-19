# 👊 Street Justice: SDL2 Beat 'em Up

> **A high-performance, retro-style 2.5D arcade engine built with C++ and SDL2, following strict low-level programming constraints.**
>
> Developed with **C++** and **SDL2/SDL_ttf**. Featuring a modular architecture, dynamic camera system, and a custom implementation that avoids the C++ Standard Template Library (STL).

---

## 📖 About The Project

**Street Justice** is a beat 'em up arcade game engine developed as a core programming project. It focuses on simulating urban combat in a 2.5D perspective, where the player moves along a deep street environment that extends far beyond the window's boundaries.

The codebase is engineered with **Clean Code principles**. All "magic numbers" have been eliminated and moved to a centralized configuration file. The program manages memory and strings using low-level C-style techniques, providing total control over performance and resource allocation.

### Technical Constraints & Highlights:

- **No STL Usage:** The project strictly avoids `std::string`, `std::vector`, and `std::iostream`. It relies on character arrays (`char[]`), `sprintf`, and manual SDL structure management.
- **Viewport & Sidebar Split:** The window is divided into a dedicated **Game Viewport** and an **Information Sidebar**, allowing for independent rendering of the game world and the UI.
- **2.5D Mechanics:** Players can move "into" the depth of the street. A specialized foot-based collision system allows the character's upper body to overlap the background naturally.
- **Dynamic Camera:** A smooth horizontal scrolling system that tracks the player's center within a world spanning 2400 pixels.

---

## 🚀 Features & Mechanics

### 🛣️ 2.5D Perspective & Clamping

The road is not just a flat line. The player moves within an area defined by the horizon (`FLOOR_TOP`) and safety margins:

- **Foot-based Collision:** Movement boundaries are calculated based on the character's feet, creating a realistic sense of depth.
- **Road Margin:** The character stops at a precise distance from the background edge, preventing visual clipping with the sky.

### 📼 Real-time Information Sidebar

All game data is rendered in a dedicated UI panel with an automatic layout:

- **Dual Timers:** Independent tracking of the Total Session Time (since launch) and the current Stage Time (reset via the 'N' key).
- **Auto-Centering Engine:** Sidebar text is dynamically centered based on the pixel width of the generated texture for a professional look.
- **Requirements Tracker:** A live list of implemented technical requirements is displayed directly in the UI for grading transparency.

### 🎮 Hybrid Control System

The engine supports two input standards simultaneously:

1.  **Classic Arcade:** Arrow Keys.
2.  **Modern PC Standard:** WSAD Keys.

---

## 📂 Codebase Architecture

The project structure is modular, separating logic from configuration and data:

- **`main.cpp`** – Entry point. Initializes the Game Loop and handles FPS capping.
- **`game.cpp`** – Engine Core. Implements movement logic, camera updates, and SDL event management.
- **`game.h`** – Central Class Definition. Contains prototypes for rendering and utility methods.
- **`player.h`** – Lightweight `Player` structure storing physical state (position, speed, dimensions).
- **`constants.h`** – Global Configuration. Contains all `#define` constants (colors, UI positions, world bounds). **Zero magic numbers in the logic files.**

---

## 🛠️ Build & Installation

### Prerequisites

You need a GCC compiler (MinGW for Windows) and the development libraries for SDL2 and SDL2_ttf.

### Compilation

Use the following command in your terminal to compile all modules together:

```bash
g++ main.cpp game.cpp -o street_justice -lSDL2 -lSDL2_ttf
```
