# PicaSim

This contains the complete source, including (custom) dependencies, for PicaSim flight simulator: https://rowlhouse.co.uk/PicaSim/ 

It also contains tools and build infrastructure for Windows, Linux, macOS, Android and iOS.

I have tried to make sure that credit/licences etc are indicated correctly - please let me know of any errors so that I can correct them.

Danny Chapman - picasimulator@gmail.com

### User Data Location

User settings and custom content are stored in platform-specific locations (not in the application directory):

| Platform | Location |
|----------|----------|
| Windows | `%APPDATA%\Rowlhouse\PicaSim\` |
| Linux | `~/.local/share/Rowlhouse/PicaSim/` |
| macOS | `~/Library/Application Support/Rowlhouse/PicaSim/` |
| Android | Internal app storage (managed automatically) |

Within this directory:
- `UserSettings/` - User preferences and saved configurations (settings.xml, custom controllers, etc.)
- `UserData/` - User-created content (custom aeroplanes, panoramas, etc.)

## Keyboard Shortcuts

### Flight Controls
| Key | Action |
|-----|--------|
| F11 / F / Alt+Enter | Toggle fullscreen |
| Escape | Back/exit menus and dialogs |
| R | Reset/relaunch the plane |
| C | Cycle camera view |
| M | Walkabout mode |
| P | Pause/unpause |
| T | Toggle slow motion (freefly mode only) |
| Right Shift | Cycle controller rates |
| +/= | Zoom in |
| - | Zoom out |
| U | Toggle UI visibility |
| Z | Toggle zoom view |
| S | Save screenshot |
| L | Reload aeroplane (re-reads settings) |
| Back | Return to menu (or relaunch, depending on settings) |

### VR controls
| Key | Action |
|-----|--------|
| V | Calibrate headset look direction (press when looking forwards) |
| B / N | Rotate the default look direction |
| H | Reset the default look direction |

### Debug Keys
| Key | Action |
|-----|--------|
| K | Cycle render preference |
| G | Cycle centre of mass display |
| W | Toggle terrain wireframe |
| 9/0 | Decrease/increase aerofoil debug info |
| 7/8 | Decrease/increase wind streamers |

### Paused Mode (freefly only)
| Key | Action |
|-----|--------|
| NumPad 4/6 | Move plane left/right (or rotate with Ctrl) |
| NumPad 2/8 | Move plane back/forward (or pitch with Ctrl) |
| NumPad 7/9 | Move plane down/up (or roll with Ctrl) |

## Build System

The project uses CMake with dependencies built from git submodules. vcpkg is used only for glad and OpenXR on Windows. Linux builds use system OpenGL headers directly.

### Prerequisites (Windows)

- **Visual Studio 2022** (Community edition is free) - provides MSVC compiler and MSBuild
- **CMake 3.20+** - install standalone or use the one bundled with Visual Studio
- **Git** - for cloning the repo and submodules
- **vcpkg** - provides glad and OpenXR (see setup below)

#### First-time Setup

```bash
# Initialise git submodules (SDL2, SDL2_net, OpenAL-Soft, GLM, imgui, stb)
git submodule update --init --recursive

# Clone vcpkg (provides glad and OpenXR)
git clone https://github.com/microsoft/vcpkg.git C:/vcpkg
C:/vcpkg/bootstrap-vcpkg.bat

# Set environment variable (restart terminal after this)
setx VCPKG_ROOT C:/vcpkg
```

### Dependencies

Most dependencies are built from source via git submodules in `third_party/`:

- **SDL2** - windowing, input, platform abstraction
- **SDL2_net** - networking
- **OpenAL-Soft** - 3D positional audio
- **GLM** - math library
- **imgui** - UI (docking branch)
- **stb** - image loading

vcpkg provides desktop-only packages (defined in `vcpkg.json`):

- **glad** - OpenGL loader
- **OpenXR** - VR support (optional)

### Compiling

The project uses CMake presets for building. Use VS Code's F7 to build, or from the command line:

```bash
# Configure (once per platform)
cmake --preset windows-x64

# Build Debug
cmake --build --preset windows-x64-debug

# Build Release
cmake --build --preset windows-x64-release

# Clean
cmake --build --preset windows-x64-debug --target clean

# Or delete the build directory for a full clean
rm -rf build
```

### Running

The application must be run from the `data/` directory so it can find game assets:

```bash
# From project root
cd data && ../build/windows-x64/Debug/PicaSim.exe
```

### Release Packaging (Windows)

Build release first, then install:

```bash
cmake --build --preset windows-x64-release
cmake --install build/windows-x64 --config Release
```

This creates a standalone distribution in `dist/PicaSim-X.Y.Z/` (version extracted from VERSIONS.txt).

### Linux Build

#### Prerequisites

- **GCC or Clang** (C++17 support)
- **CMake 3.20+**
- **Ninja** build system
- Development libraries (install via your package manager)

```bash
# Ubuntu/Debian
sudo apt install cmake ninja-build g++ \
  libgl-dev libgles-dev libegl-dev \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev \
  libxfixes-dev libxss-dev libxkbcommon-dev \
  libasound2-dev libpulse-dev libdbus-1-dev \
  libwayland-dev libpipewire-0.3-dev rsync

# Fedora/RHEL (untested)
sudo dnf install cmake ninja-build gcc-c++ \
  mesa-libGL-devel mesa-libEGL-devel \
  libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXi-devel \
  libXfixes-devel libXScrnSaver-devel libxkbcommon-devel \
  alsa-lib-devel pulseaudio-libs-devel dbus-devel \
  wayland-devel pipewire-devel rsync

# Arch (untested)
sudo pacman -S cmake ninja gcc mesa libx11 libxext libxrandr libxcursor \
  libxi libxfixes libxss libxkbcommon alsa-lib libpulse dbus \
  wayland pipewire rsync
```

**Note:** vcpkg is NOT required for Linux builds.

#### Building

```bash
# Initialise submodules (first time only)
git submodule update --init --recursive

# x86_64
cmake --preset linux-x64
cmake --build --preset linux-x64-debug      # Debug
cmake --build --preset linux-x64-release     # Release

# ARM64 (aarch64)
cmake --preset linux-arm64
cmake --build --preset linux-arm64-debug     # Debug
cmake --build --preset linux-arm64-release   # Release

# Run (must be from data/ directory)
cd data && ../build/linux-x64/Debug/PicaSim
```

#### Linux Distribution (AppImage)

Create a portable AppImage that runs on most Linux distributions:

```bash
# Build release first
cmake --build --preset linux-x64-release   # or linux-arm64-release

# Create AppImage (downloads appimagetool automatically if needed)
./linux_create_appimage.sh build/linux-x64 dist
```

Output: `dist/PicaSim-X.Y.Z-x86_64.AppImage` (or `aarch64` on ARM).

### macOS Build

#### Prerequisites

- **Xcode Command Line Tools** (or full Xcode)
- **CMake 3.20+**
- **vcpkg** - provides glad

```bash
# First-time setup
git submodule update --init --recursive
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
export VCPKG_ROOT=~/vcpkg
```

#### Building

```bash
VCPKG_ROOT=~/vcpkg cmake --preset macos-arm64
VCPKG_ROOT=~/vcpkg cmake --build --preset macos-arm64-debug     # Debug
VCPKG_ROOT=~/vcpkg cmake --build --preset macos-arm64-release   # Release

# Run (must be from data/ directory)
cd data && ../build/macos-arm64/Debug/PicaSim
```

#### macOS Distribution

The full pipeline to create a signed, notarized DMG:

```bash
# 1. Build and install
VCPKG_ROOT=~/vcpkg cmake --build --preset macos-arm64-release
cmake --install build/macos-arm64 --config Release

# 2. Create .app bundle (copies icon from resources/PicaSim.icns)
./macos_create_app_bundle.sh dist/PicaSim-*/ dist

# 3. Sign, create DMG, notarize and staple (calls macos_create_dmg.sh internally)
#    Requires: PICASIM_MACOS_CERT env var, notarytool credentials stored in keychain
#    One-time setup: xcrun notarytool store-credentials "picasim-profile"
PICASIM_MACOS_CERT="Developer ID Application: ..." ./macos_notarize.sh
```

Output: `dist/PicaSim-X.Y.Z.dmg` (signed, notarized, stapled).

If you only need an unsigned DMG (no Apple Developer account):
```bash
./macos_create_app_bundle.sh dist/PicaSim-*/ dist
./macos_create_dmg.sh dist/PicaSim.app dist
```

### iOS Build

#### Prerequisites

- **Xcode** (full install, not just CLI tools)
- **Apple Developer account** (for device deployment and TestFlight)
- **CMake 3.20+**

#### Building

```bash
cmake --preset ios-device
cmake --build --preset ios-device-debug

# Open in Xcode for device deployment
open build/ios-device/PicaSim.xcodeproj
```

#### iOS Distribution (TestFlight)

```bash
# Create archive with dSYM and open in Xcode Organizer for upload
./ios_archive.sh
```

**Important:** Do not use Xcode's Product > Archive — CMake hardcodes build paths that prevent dSYMs from being included in the archive. `ios_archive.sh` works around this.

### Icon Generation

All platform icons (Windows `.ico`, macOS `.icns`, Android, iOS) are generated from source images in `resources/AndroidIcon/`:

```bash
pip install Pillow
python3 resources/generate_icons.py
```

### Building the Installer (Windows)

To create a Windows installer (.exe), you need Inno Setup:

#### Prerequisites

- **Inno Setup 6** - Download from https://jrsoftware.org/isinfo.php
- Complete the Release Packaging step above first

#### Building

Option 1: Via CMake target (if Inno Setup is in PATH or standard location):
```bash
cmake --build build/windows-x64 --target installer
```

Option 2: Run Inno Setup directly:
```bash
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /DMyAppVersion=1.0.0 installer\PicaSim.iss
```

#### Output

The installer is created at `dist/PicaSim-X.Y.Z-Setup.exe`. Running it will:
- Install PicaSim to the chosen location (default: `C:\Program Files\PicaSim`)
- Create Start Menu shortcuts
- Optionally create a Desktop shortcut
- Register an uninstaller

**VS Code**: F7 to build. Use F5 to debug - the `.vscode/launch.json` is configured with the correct working directory.

### Android Build

#### Prerequisites

- **Android SDK** with platform API 36
- **Android NDK r28** (28.2.13676358, the version pinned in `android/app/build.gradle`) - install via Android Studio's SDK Manager, or let Gradle download it
- **Java 17** - required by Gradle 8.11 and the Android Gradle Plugin 8.10
- **Git submodules** initialised (SDL2, SDL2_net, openal-soft, imgui, stb, glm live in `third_party/`)

```bash
# Initialise submodules (one-time, after cloning)
git submodule update --init --recursive
```

Set the `ANDROID_HOME` environment variable if not already set (Android Studio usually sets this):

```bash
# Windows
setx ANDROID_HOME "%LOCALAPPDATA%\Android\Sdk"

# Linux/macOS
export ANDROID_HOME=~/Android/Sdk
```

#### Building the APK

From the project root:

```bash
# Debug build (arm64-v8a + x86_64)
cd android
gradlew.bat assembleDebug        # Windows
./gradlew assembleDebug           # Linux/macOS (set python to python3 and use jdk temurin 17)
```

The APK is output to `android/app/build/outputs/apk/debug/app-debug.apk`.

#### Installing on a Device

1. Enable **Developer Options** on your Android device (Settings > About Phone > tap Build Number 7 times)
2. Enable **USB Debugging** in Developer Options
3. Connect the device via USB and accept the debugging prompt

Then install via adb:

```bash
adb install android/app/build/outputs/apk/debug/app-debug.apk
```

Or build and install in one step:

```bash
cd android
gradlew.bat installDebug          # Windows
./gradlew installDebug             # Linux/macOS

gradlew.bat installRelease
gradlew.bat bundleRelease # full release bundle in android\app\build\outputs\bundle\release


```

#### Viewing Logs

Filter logcat to PicaSim output:

```bash
adb logcat -s SDL:* PicaSim:* OpenAL:*
```

#### Notes

- The APK bundles all game assets from `data/` — on first launch they are extracted to internal storage, which takes a moment
- Supported ABIs: `arm64-v8a` (most modern phones) and `x86_64` (emulators)
- The Android build uses libraries from `third_party/` submodules rather than vcpkg

### Directory Structure

```
PicaSim2/
├── android/                  # Android Gradle project
├── ios/                      # iOS assets (icons, LaunchScreen, Info.plist)
├── build/                    # Build output (gitignored)
│   └── <preset-name>/        # One dir per platform preset
├── data/                     # Working directory for running
│   ├── SystemData/           # Read-only game assets (committed)
│   ├── SystemSettings/       # Read-only presets (committed)
│   └── Menus/                # Menu assets (committed)
├── resources/                # App icons, related assets and generate_icons.py
├── source/                   # Source code
│   ├── Framework/            # Reusable engine components
│   ├── PicaSim/              # Application code
│   ├── Platform/             # Platform abstraction (SDL2, Android, VR)
│   ├── Heightfield/          # Terrain rendering
│   ├── bullet-2.81/          # Physics (Bullet)
│   └── tinyxml/              # XML parsing
├── third_party/              # Git submodules (SDL2, OpenAL-Soft, etc.)
└── tools/                    # Build/asset helper scripts
```

### Compilation Defines

- `BT_NO_PROFILE` - Disables Bullet physics profiling

# Notes on the licence

PicaSim is licensed under the **PolyForm Noncommercial License 1.0.0** (see LICENSE.txt). This licence covers PicaSim source code and some of the data it uses.

Third party packages and assets have their own licences which need to be abided by.

These are covered by PicaSim's licence:

Under Source
- Framework: Contains fairly generic code on which PicaSim is built
- Heightfield: Runtime refinement for rendering a heightfield, based on a paper by Lindstrom + Pascucci. It was good in its day, but I would not recommend it now!
- MapTrace: Stand-alone helper application for creating a heightfield by tracing contours (very old!). It has been removed from the repository, but is available in the project's history if ever needed.
- PicaSim: All the application code
- Platform: Platform abstraction layer (SDL2, Android, VR)

Under Data
- Menus and Fonts contain UI resources
- SystemSettings contains the "front end" for data - so typically things that are presented to the user, and perhaps modifyable
- SystemData contains the "back end" for data - typically the low-level representations.
-- All text and XML files here (but not .ac and .png etc) can be used under PicaSim's licence.

The following need to be treated differently:

Under Data
- Images and model files come from various sources (ranging from having been created by me, to provided by others) and have been authorised for use with PicaSim
- They can therefore be used in direct derivatives of PicaSim
- You will need to request permissions directly from me/the original author to use them in another project.

These are not covered by PicaSim's licence - they have their own:

Under Source
- bullet-2.81: Bullet physics library (source is under the zlib licence). There may be some modifications from the original.
- tinyxml: Under the zlib licence

Under third_party (git submodules)
- SDL2, SDL2_net: zlib licence
- OpenAL-Soft: LGPL licence
- GLM: MIT licence
- imgui: MIT licence
- stb: MIT/public domain

