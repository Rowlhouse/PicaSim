# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Claude's goal

The goal for Claude is to help develop PicaSim across Windows, Android and iOS.

The original project has been ported from Marmalade SDK to SDL2 + OpenGL. Windows and Android ports are complete. iOS is not yet started.

The original project is owned by me, so I have all rights to do this.

The intention is to not change any existing behaviour significantly.

## Building

Always use CMake presets for desktop:
- Debug: `cmake --build --preset windows-x64-debug`
- Release: `cmake --build --preset windows-x64-release`

For Android:
- `cd android && gradlew.bat assembleDebug`

Always build in debug when building automatically, if just asked to "build". Requests to build in release will always be explicit.

## Git Commits (instruction for Claude/AI)

Never volunteer to commit changes. Only create commits when explicitly requested by the user.

## Project Overview

PicaSim is a cross-platform R/C flight simulator built with C++. It simulates radio-controlled aircraft with realistic physics, multiple aircraft types (40+), and various environments. The project targets Windows, Android, and iOS.

**Current stack**: SDL2 (window/input), OpenGL/GLES2 (rendering), OpenAL-Soft (audio), GLM (math), Bullet Physics (physics), Dear ImGui (UI).

**Note**: The project was migrated from Marmalade SDK (no longer commercially available) to SDL2 + OpenGL. Most dependencies are built from git submodules in `third_party/`; vcpkg provides only glad and OpenXR on desktop.

## Architecture

### Core Classes

**PicaSim** (`source/PicaSim/PicaSim.h`) - Singleton orchestrator managing the game loop, camera, aeroplanes, and UI overlays. Handles mode transitions (ground/aeroplane/chase/walk) and game status (flying/paused).

**Aeroplane** (`source/PicaSim/Aeroplane.h`) - Container class that intentionally separates graphics and physics:
- `AeroplaneGraphics` - 3D rendering (can exist without physics for networked/animated planes)
- `AeroplanePhysics` - Flight dynamics simulation (can exist without graphics for AI/headless)
- This separation enables AI pilots, network multiplayer, and non-graphical simulations

**Controller hierarchy**:
- `Controller` - Abstract base for all input
- `HumanController` - Player input from touch/joystick
- `AIController*` variants - AI pilots (Glider, Powered, Tug)

**Environment** - Wind simulation, thermals, and terrain interaction

**Challenge** - Race/duration/limbo game modes with gates and scoring

### Framework Layer

`source/Framework/` contains reusable game engine components:
- `Graphics`, `RenderManager` - Rendering pipeline
- `Camera` - Multiple camera modes
- `AudioManager` - 3D positional audio via OpenAL-Soft
- `EntityManager` - Entity lifecycle management
- `ParticleEngine` - Particle effects
- `ShaderManager` - GPU shader handling

`source/Platform/` contains platform abstraction:
- `S3ECompat.h` - Marmalade API compatibility shims (SDL2 implementations)
- `Input.cpp/h` - Unified input handling (keyboard, mouse, touch, gamepad)
- `imgui_impl_sdl2.cpp/h` - ImGui SDL2 backend
- `AndroidAssets.cpp/h` - APK asset extraction to internal storage
- `VRManager.cpp/h`, `OpenXRRuntime.cpp/h` - VR support via OpenXR
- `FontRenderer.cpp/h` - Bitmap font rendering for in-game overlay text

### Physics

Uses Bullet Physics (`source/bullet-2.81/`) for rigid body dynamics. Custom aerodynamics code in `AeroplanePhysics.cpp` calculates lift, drag, and control surface effects using aerofoil definitions.

### Data Organization

All data lives in `data/` directory. The application runs with `data/` as the working directory.

**data/SystemData/** - Low-level data representations (read-only, committed):
- `Aeroplanes/` - Aircraft XML definitions + AC3D models (.ac)
- `Aerofoils/` - Airfoil profiles (Symmetric, SemiSymmetric, Reflex)
- `Panoramas/` - Skybox environments with heightmaps
- `Audio/` - Sound effects (22050/44100Hz raw files)

**data/SystemSettings/** - User-facing configuration (read-only, committed):
- `Aeroplane/` - Aircraft selection presets
- `Challenge/` - Race/challenge definitions
- `Controller/` - Input configurations
- `Environment/` - Weather and terrain settings
- `Options/` - Quality presets (Low/Standard/High)

**data/UserData/** - User-created content (gitignored, created at runtime):
- Custom aircraft, scenarios, etc.

**data/UserSettings/** - User preferences (gitignored, created at runtime):
- `settings.xml` - User's saved settings

All configuration uses XML parsed via tinyxml (`source/tinyxml/`).

### Platform Abstractions

- `source/Platform/S3ECompat.h` - Compatibility layer replacing Marmalade s3e* APIs with SDL2
- `source/Platform/Input.cpp` - SDL2-based input (keyboard, mouse, touch, gamepad)

## Migration Status

The Windows desktop port and Android port are **complete**. iOS remains to be done.

### Completed

**Windows Desktop**
- CMake with git submodules + vcpkg (glad, OpenXR)
- SDL2 window creation and OpenGL context via glad
- Full rendering pipeline: ShaderManager, FrameBufferObject, skybox, particles
- Math types (GLM replacing CIwFVec3/CIwFMat/etc.)
- VR support via OpenXR

**Android**
- Gradle 8.5 + CMake build, APK packaging with bundled assets
- GLES2 rendering (shader-based, no fixed-function pipeline)
- Asset extraction from APK to internal storage on first launch
- DPI-aware UI scaling, drag-to-scroll, safe area insets
- Supported ABIs: arm64-v8a, x86_64

**Audio (OpenAL-Soft)**
- Complete 3D positional audio with distance attenuation
- 32 sound channels with proper source management
- Doppler effect, volume ramping, frequency scaling

**UI (Dear ImGui)**
- All menus migrated from IwUI to ImGui
- SDL2 platform backend (`source/Platform/imgui_impl_sdl2.cpp`)
- Unified style system with font scaling
- Bitmap font renderer for in-game overlay text
- Scrollable tab strips for mobile

**Input (SDL2)**
- Keyboard, mouse/touch, gamepad (SDL2 game controller API)

**Networking (SDL2_net)**
- TCP server on port 7777 for remote aircraft control

### To Do

**iOS**
- Needs view controller, SDL2 integration, GLES2 rendering
- SDL2 handles most cross-platform work; mainly needs app scaffolding

## Licensing

PicaSim source is licensed under the **PolyForm Noncommercial License 1.0.0**. Third-party components have separate licenses:
- Bullet Physics: zlib license
- tinyxml: zlib license
- SDL2, SDL2_net: zlib license
- OpenAL-Soft: LGPL
- GLM, imgui: MIT license
- stb: MIT/public domain
- 3D models and images: authorized for PicaSim derivatives only; other uses require permission

## Read
 - README.md
 - CODE_STYLE.md
 - ParallaxPanorama.md

