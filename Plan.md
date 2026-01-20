# PicaSim Migration Plan: Marmalade SDK to SDL2 + OpenGL

## Overview

Migrate PicaSim from the discontinued Marmalade SDK to SDL2 with OpenGL, targeting Windows first, then Android, then iOS/macOS/Linux.

## Confirmed Framework Stack

| Component | Library | Rationale |
|-----------|---------|-----------|
| Window/Input/Platform | SDL2 | Cross-platform, mature, supports all targets |
| Math Types | GLM | Industry standard, replaces CIwFVec3/CIwFMat/etc. |
| 3D Audio | OpenAL-Soft | Native 3D positioning, Doppler support |
| Texture Loading | stb_image | Header-only, lightweight |
| UI System | Dear ImGui | Fast iteration, can enhance later |
| Networking | SDL2_net | Consistent with SDL2 stack |

## Architecture Assessment

### What Can Stay (60%+ of codebase)
- **Bullet Physics** - Already platform-independent
- **TinyXML** - Already platform-independent
- **Core game logic** - Aeroplane, AeroplanePhysics, AeroplaneGraphics, Environment, Challenges, AI controllers
- **Shader system** - GLSL shaders are portable (ES 2.0 compatible)
- **Most of Framework layer** - Camera, Viewport, Entity, RenderObject, ParticleEngine

### What Needs Replacement

| Marmalade API | Replacement | Files Affected |
|---------------|-------------|----------------|
| EGL/IwGL context | SDL2 + OpenGL | Graphics.cpp |
| s3eSound callbacks | OpenAL-Soft | AudioManager.cpp |
| s3ePointer/Keyboard | SDL2 events | HumanController.cpp |
| s3eSocket | SDL2_net | ConnectionListener.cpp, IncomingConnection.cpp |
| s3eTimer | SDL_GetTicks64 | Main.cpp, throughout |
| s3eFile | C++ filesystem + SDL paths | GameSettings.cpp, throughout |
| s3eDevice | SDL2 + preprocessor | FrameworkSettings.h |
| CIwFVec3/CIwFMat/etc. | GLM types | Helpers.h |
| IwUI menus | Dear ImGui | All files in Menus/ |
| IwAssert | Standard assert | Throughout |

---

## Implementation Phases

### Phase 0: Build System Setup
**Goal**: Create CMake build system parallel to existing .mkb files

**Tasks**:
1. Create root `CMakeLists.txt` with Windows configuration
2. Set up vcpkg for dependency management (SDL2, GLM, OpenAL-Soft, ImGui)
3. Create `source/Platform/` directory for abstraction layer
4. Configure Debug/Release builds

**New files**:
```
CMakeLists.txt
cmake/FindSDL2.cmake
cmake/Platform.cmake
source/Platform/Platform.h
source/Platform/PlatformSDL.cpp
```

---

### Phase 1: Core Platform Layer
**Goal**: Replace fundamental Marmalade types and utilities

**Tasks**:
1. **Math types** - Create GLM wrappers in `Helpers.h` matching existing API
   - `Vector2/3/4` -> `glm::vec2/3/4`
   - `Transform` -> Custom class wrapping `glm::mat4`
   - `Quat` -> `glm::quat`
   - `Colour` -> Custom RGBA class

2. **Timer** - Replace `s3eTimerGetMs()` with `SDL_GetTicks64()`

3. **File I/O** - Replace `s3eFile*` with C++ filesystem + SDL paths

4. **Assert/Debug** - Replace `IwAssert()` with standard assert

**Files to modify**:
- `source/Framework/Helpers.h` - Math type wrappers
- `source/Framework/Trace.h` - Logging

**Verification**: Unit tests comparing GLM math output with reference values

---

### Phase 2: Graphics System
**Goal**: Replace Marmalade graphics initialization and rendering

**Tasks**:
1. **Window/Context** - Replace EGL init with SDL2 window + OpenGL context
2. **Swap buffers** - Replace `IwGxSwapBuffers()` with `SDL_GL_SwapWindow()`
3. **Clear/Flush** - Replace `IwGxClear()`/`IwGxFlush()` with raw GL calls
4. **Texture loading** - Replace `CIwTexture` with stb_image + raw GL
5. **Platform settings** - Replace `s3eDeviceOSID` with preprocessor macros

**Files to modify**:
- `source/Framework/Graphics.h/cpp` - Major rewrite of init/shutdown
- `source/Framework/FrameworkSettings.h/cpp` - Platform detection
- `source/PicaSim/Main.cpp` - Entry point (complete rewrite)

**Verification**: Render a skybox and simple geometry, compare with reference screenshots

---

### Phase 3: Input System
**Goal**: Replace all Marmalade input APIs

**Tasks**:
1. **Touch/Mouse** - Replace `s3ePointer*` with SDL2 mouse/touch events
2. **Keyboard** - Replace `s3eKeyboard*` with SDL2 keyboard state
3. **Joystick/Gamepad** - Consolidate Joystick/Gamepad extensions into SDL2 game controller API
4. **Accelerometer** - Use SDL2 sensor API for mobile

**Files to modify**:
- `source/PicaSim/HumanController.cpp` - Primary input handling
- `source/PicaSim/PicaJoystick.h/cpp` - Joystick abstraction
- Remove `source/Joystick/` and `source/Gamepad/` extensions

**Verification**: Test keyboard flight controls, joystick input, touch simulation

---

### Phase 4: Audio System
**Goal**: Replace Marmalade audio with OpenAL

**Tasks**:
1. **Audio manager rewrite** - Replace callback-based s3eSound with OpenAL
2. **Sound loading** - Load RAW files into OpenAL buffers
3. **3D positioning** - Use OpenAL source positioning
4. **Doppler effect** - Use OpenAL's built-in Doppler
5. **Volume/frequency control** - Map to OpenAL gain/pitch

**Files to modify**:
- `source/Framework/AudioManager.h/cpp` - Complete rewrite

**Verification**: A/B test engine sounds, verify 3D audio positioning works

---

### Phase 5: UI System
**Goal**: Replace IwUI menu system with Dear ImGui

**Tasks**:
1. **ImGui integration** - Add ImGui to render pipeline
2. **Start menu** - Aircraft/scenario selection
3. **Settings menu** - All game settings (controls, graphics, audio)
4. **File browser** - Load/save scenarios
5. **In-game overlays** - HUD elements, windsock, variometer
6. **Dialogs** - Confirmation dialogs, help screens

**Files to modify**:
- All files in `source/PicaSim/Menus/` - Rewrite for ImGui
- `source/Framework/ButtonOverlay.h/cpp` - Touch buttons

**Verification**: Navigate all menu paths, verify settings persistence

---

### Phase 6: Networking
**Goal**: Replace Marmalade sockets with SDL2_net

**Tasks**:
1. **Socket abstraction** - Replace `s3eSocket*` with SDL2_net TCPsocket
2. **Connection listener** - Port server socket code
3. **Client connections** - Port client handling code

**Files to modify**:
- `source/PicaSim/ConnectionListener.h/cpp`
- `source/PicaSim/IncomingConnection.h/cpp`

**Verification**: Test multiplayer connection between two instances

---

### Phase 7: Platform Polish
**Goal**: Handle remaining platform-specific APIs

**Tasks**:
1. **Device info** - CPU cores, RAM via SDL2
2. **Screen DPI** - Replace dpi extension with `SDL_GetDisplayDPI()`
3. **URL opening** - Platform-specific shell commands
4. **App lifecycle** - Handle pause/resume events

**Files to modify**:
- `source/dpi/` - Remove, use SDL2
- Various platform-specific code paths

---

### Phase 8: Testing & Polish
**Goal**: Comprehensive testing of all features

**Test matrix**:
- [ ] All 40+ aircraft fly correctly
- [ ] All terrain types render correctly
- [ ] All 4 wind types work (smooth, gusty, turbulent, thermals)
- [ ] All challenge modes (Race, Duration, Limbo, FreeFly)
- [ ] All camera modes (Pilot, Chase, Ground, Zoom)
- [ ] All launch modes (Normal, Bungee, Aerotow, Tether)
- [ ] AI pilots work correctly
- [ ] Multiplayer networking
- [ ] 3D spatial audio with Doppler
- [ ] Joystick/gamepad input
- [ ] Settings save/load correctly

---

### Phase 9: Android Port (After Windows)
**Goal**: Port to Android (priority mobile platform)

**Tasks**:
- SDL2 Android backend integration
- Touch control tuning for on-screen joysticks
- Accelerometer input via SDL_Sensor
- App lifecycle (pause/resume/background)
- APK packaging and Play Store preparation
- Test on multiple device sizes/DPIs

---

### Phase 10: iOS and Desktop Ports
**Goal**: Complete platform support

**iOS tasks**:
- SDL2 iOS backend integration
- Touch/accelerometer input
- App Store requirements and IPA packaging

**macOS/Linux tasks**:
- SDL2 builds for macOS and Linux
- Test keyboard/joystick input
- Package as .app (macOS) and AppImage/deb (Linux)

---

## Critical Files Reference

| File | Migration Complexity | Notes |
|------|---------------------|-------|
| `source/PicaSim/Main.cpp` | Complete rewrite | SDL2 entry point |
| `source/Framework/Graphics.cpp` | Major changes | EGL -> SDL2/GL |
| `source/Framework/AudioManager.cpp` | Complete rewrite | s3eSound -> OpenAL |
| `source/Framework/Helpers.h` | Moderate changes | Math type wrappers |
| `source/Framework/FrameworkSettings.h` | Moderate changes | Platform detection |
| `source/PicaSim/HumanController.cpp` | Major changes | All input handling |
| `source/PicaSim/Menus/*.cpp` | Complete rewrite | IwUI -> ImGui |
| `source/PicaSim/ConnectionListener.cpp` | Moderate changes | s3eSocket -> SDL2_net |

---

## Features Preserved

All existing features will be preserved:
- 40+ aircraft with realistic physics
- Multiple terrain environments with heightmaps
- Wind simulation (smooth, gusty, turbulent, thermals)
- Challenge modes with scoring
- Multiplayer networking
- 3D spatial audio
- Multiple camera modes
- AI pilots
- Various launch modes
- Physical controller support
- Touch controls (mobile)
- Accelerometer input (mobile)

---

## Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Audio system complexity | High | Prototype OpenAL early, test thoroughly |
| UI system scope | High | Start with minimal functional UI |
| Touch feel on mobile | Medium | Early Android testing |
| Shader compatibility | Low | Existing ES 2.0 shaders should work |
| Joystick variations | Medium | SDL2 game controller database helps |
