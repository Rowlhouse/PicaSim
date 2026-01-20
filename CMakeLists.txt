# PicaSim CMake Build System
# Migrating from Marmalade SDK to SDL2 + OpenGL
cmake_minimum_required(VERSION 3.20)
project(PicaSim VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(PICASIM_BUILD_TESTS "Build unit tests" OFF)
option(PICASIM_USE_OPENAL "Use OpenAL for audio (vs SDL_mixer)" ON)

# Platform detection
if(WIN32)
    set(PICASIM_PLATFORM "WINDOWS")
    add_compile_definitions(PICASIM_WINDOWS=1)
elseif(APPLE)
    if(IOS)
        set(PICASIM_PLATFORM "IOS")
        add_compile_definitions(PICASIM_IOS=1)
    else()
        set(PICASIM_PLATFORM "MACOS")
        add_compile_definitions(PICASIM_MACOS=1)
    endif()
elseif(ANDROID)
    set(PICASIM_PLATFORM "ANDROID")
    add_compile_definitions(PICASIM_ANDROID=1)
elseif(UNIX)
    set(PICASIM_PLATFORM "LINUX")
    add_compile_definitions(PICASIM_LINUX=1)
endif()

message(STATUS "Building PicaSim for platform: ${PICASIM_PLATFORM}")

# Compile definitions matching original Marmalade build
add_compile_definitions(
    BT_NO_PROFILE          # Disable Bullet physics profiling
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Find required packages
# SDL2
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_net CONFIG QUIET)

# GLM for math types
find_package(glm CONFIG REQUIRED)

# OpenAL for audio
if(PICASIM_USE_OPENAL)
    find_package(OpenAL CONFIG REQUIRED)
    add_compile_definitions(PICASIM_USE_OPENAL=1)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# --------------------------------------------------------------------------
# Third-party libraries
# --------------------------------------------------------------------------

# TinyXML (compiled from source)
set(TINYXML_SOURCES
    source/tinyxml/tinystr.cpp
    source/tinyxml/tinyxml.cpp
    source/tinyxml/tinyxmlerror.cpp
    source/tinyxml/tinyxmlparser.cpp
)
add_library(tinyxml STATIC ${TINYXML_SOURCES})
target_include_directories(tinyxml PUBLIC source/tinyxml)

# Bullet Physics
# Note: We use the existing Bullet source in the repo (located in source/bullet-2.81/Bullet/)
set(BULLET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/bullet-2.81/Bullet)

# Manual Bullet build - collect source files
file(GLOB_RECURSE BULLET_SOURCES
    ${BULLET_SOURCE_DIR}/BulletCollision/*.cpp
    ${BULLET_SOURCE_DIR}/BulletDynamics/*.cpp
    ${BULLET_SOURCE_DIR}/BulletSoftBody/*.cpp
    ${BULLET_SOURCE_DIR}/LinearMath/*.cpp
)
add_library(BulletPhysics STATIC ${BULLET_SOURCES})
target_include_directories(BulletPhysics PUBLIC ${BULLET_SOURCE_DIR})
target_compile_definitions(BulletPhysics PRIVATE BT_NO_PROFILE)

# stb libraries (header-only, via vcpkg)
find_path(STB_INCLUDE_DIRS "stb_image.h")

# GLAD for OpenGL function loading on Windows
if(WIN32)
    find_package(glad CONFIG REQUIRED)
endif()

# Dear ImGui
find_package(imgui CONFIG REQUIRED)

# --------------------------------------------------------------------------
# Framework library
# --------------------------------------------------------------------------
set(FRAMEWORK_SOURCES
    source/Framework/AudioManager.cpp
    source/Framework/ButtonOverlay.cpp
    source/Framework/Camera.cpp
    source/Framework/DebugRenderer.cpp
    source/Framework/Entity.cpp
    source/Framework/EntityManager.cpp
    source/Framework/FrameBufferObject.cpp
    source/Framework/FrameworkSettings.cpp
    source/Framework/Graphics.cpp
    source/Framework/Helpers.cpp
    source/Framework/HelpersXML.cpp
    source/Framework/MemoryCheck.cpp
    source/Framework/ParticleEmitter.cpp
    source/Framework/ParticleEngine.cpp
    source/Framework/Profile.cpp
    source/Framework/RenderManager.cpp
    source/Framework/ShaderManager.cpp
    source/Framework/Shaders.cpp
    source/Framework/Skybox.cpp
    source/Framework/Trace.cpp
    source/Framework/Viewport.cpp
)


set(FRAMEWORK_HEADERS
    source/Framework/AudioManager.h
    source/Framework/ButtonOverlay.h
    source/Framework/Camera.h
    source/Framework/DebugRenderer.h
    source/Framework/Entity.h
    source/Framework/EntityManager.h
    source/Framework/FrameBufferObject.h
    source/Framework/Framework.h
    source/Framework/FrameworkSettings.h
    source/Framework/Graphics.h
    source/Framework/Helpers.h
    source/Framework/HelpersXML.h
    source/Framework/LoadingScreenHelper.h
    source/Framework/MemoryCheck.h
    source/Framework/ParticleEmitter.h
    source/Framework/ParticleEngine.h
    source/Framework/Profile.h
    source/Framework/RenderManager.h
    source/Framework/RenderObject.h
    source/Framework/RenderOverlayObject.h
    source/Framework/ShaderManager.h
    source/Framework/Shaders.h
    source/Framework/Skybox.h
    source/Framework/Trace.h
    source/Framework/Viewport.h
)

add_library(Framework STATIC ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS})
target_include_directories(Framework PUBLIC
    source/Framework
    source/tinyxml
    source/Heightfield
    source/Platform
    ${BULLET_SOURCE_DIR}
    ${STB_INCLUDE_DIRS}
)

target_link_libraries(Framework PUBLIC
    tinyxml
    Platform
    BulletPhysics
    glm::glm
    OpenGL::GL
    imgui::imgui
)
if(WIN32)
    target_link_libraries(Framework PUBLIC glad::glad)
endif()

# Link OpenAL for AudioManager
if(PICASIM_USE_OPENAL)
    target_link_libraries(Framework PUBLIC OpenAL::OpenAL)
endif()

# --------------------------------------------------------------------------
# Heightfield library
# --------------------------------------------------------------------------
set(HEIGHTFIELD_SOURCES
    source/Heightfield/HeightfieldBuilder.cpp
    source/Heightfield/HeightfieldGenerator.cpp
    source/Heightfield/HeightfieldRuntime.cpp
)

add_library(Heightfield STATIC ${HEIGHTFIELD_SOURCES})
target_include_directories(Heightfield PUBLIC source/Heightfield)
target_link_libraries(Heightfield PUBLIC Framework)

# --------------------------------------------------------------------------
# Platform abstraction layer (new SDL2-based implementation)
# --------------------------------------------------------------------------
set(PLATFORM_SOURCES
    source/Platform/PlatformSDL.cpp
    source/Platform/Window.cpp
    source/Platform/Texture.cpp
    source/Platform/Input.cpp
    source/Platform/FontRenderer.cpp
    source/Platform/imgui_impl_sdl2.cpp
)

set(PLATFORM_HEADERS
    source/Platform/Platform.h
    source/Platform/Window.h
    source/Platform/Texture.h
    source/Platform/Input.h
    source/Platform/FontRenderer.h
    source/Platform/S3ECompat.h
)

add_library(Platform STATIC ${PLATFORM_SOURCES} ${PLATFORM_HEADERS})
target_include_directories(Platform PUBLIC
    source/Platform
    ${STB_INCLUDE_DIRS}
    ${BULLET_SOURCE_DIR}
)
target_link_libraries(Platform PUBLIC
    SDL2::SDL2
    glm::glm
    OpenGL::GL
    imgui::imgui
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(Platform PUBLIC glad::glad)
    target_link_libraries(Platform PRIVATE shlwapi)
endif()

# --------------------------------------------------------------------------
# PicaSim game code
# --------------------------------------------------------------------------
set(PICASIM_SOURCES
    source/PicaSim/3ds.cpp
    source/PicaSim/Accelerometer.cpp
    source/PicaSim/Aerofoil.cpp
    source/PicaSim/AerofoilDefinition.cpp
    source/PicaSim/Aeroplane.cpp
    source/PicaSim/AeroplaneGraphics.cpp
    source/PicaSim/AeroplanePhysics.cpp
    source/PicaSim/AIControllerGlider.cpp
    source/PicaSim/AIControllerPowered.cpp
    source/PicaSim/AIControllerTug.cpp
    source/PicaSim/Challenge.cpp
    source/PicaSim/ChallengeDuration.cpp
    source/PicaSim/ChallengeFreeFly.cpp
    source/PicaSim/ChallengeLimbo.cpp
    source/PicaSim/ChallengeRace.cpp
    source/PicaSim/ConnectionListener.cpp
    source/PicaSim/Environment.cpp
    source/PicaSim/Fuselage.cpp
    source/PicaSim/GameSettings.cpp
    source/PicaSim/Gate.cpp
    source/PicaSim/Gyro.cpp
    source/PicaSim/HumanController.cpp
    source/PicaSim/IncomingConnection.cpp
    source/PicaSim/JetEngine.cpp
    source/PicaSim/Main.cpp
    source/PicaSim/NetworkController.cpp
    source/PicaSim/ObjectEditingOverlay.cpp
    source/PicaSim/Observer.cpp
    source/PicaSim/PicaJoystick.cpp
    source/PicaSim/PicaSim.cpp
    source/PicaSim/PicaStrings.cpp
    source/PicaSim/PropellerEngine.cpp
    source/PicaSim/RenderModel.cpp
    source/PicaSim/Rope.cpp
    source/PicaSim/Runway.cpp
    source/PicaSim/SimpleObject.cpp
    source/PicaSim/SkyGrid.cpp
    source/PicaSim/Terrain.cpp
    source/PicaSim/ThermalManager.cpp
    source/PicaSim/VersionChecker.cpp
    source/PicaSim/Wheel.cpp
    source/PicaSim/WindsockOverlay.cpp
    source/PicaSim/Wing.cpp
    source/PicaSim/ac3d.cpp
)

# Language files
set(PICASIM_LANGUAGE_SOURCES
    source/PicaSim/Languages/English.cpp
    source/PicaSim/Languages/French.cpp
    source/PicaSim/Languages/German.cpp
    source/PicaSim/Languages/Portugese.cpp
)

# Menu files (will be replaced with ImGui)
set(PICASIM_MENU_SOURCES
    source/PicaSim/Menus/FileMenu.cpp
    source/PicaSim/Menus/HelpMenu.cpp
    source/PicaSim/Menus/LoadingScreen.cpp
    source/PicaSim/Menus/Menu.cpp
    source/PicaSim/Menus/PicaDialog.cpp
    source/PicaSim/Menus/SettingsMenu.cpp
    source/PicaSim/Menus/SettingsWidgets.cpp
    source/PicaSim/Menus/StartMenu.cpp
    source/PicaSim/Menus/UIHelpers.cpp
    source/PicaSim/Menus/WhatsNewMenu.cpp
)

# --------------------------------------------------------------------------
# Main executable
# --------------------------------------------------------------------------
add_executable(PicaSim
    ${PICASIM_SOURCES}
    ${PICASIM_LANGUAGE_SOURCES}
    ${PICASIM_MENU_SOURCES}
)

target_include_directories(PicaSim PRIVATE
    source/PicaSim
    source/PicaSim/Menus
    source/PicaSim/Languages
    source/Framework
    source/Heightfield
    source/tinyxml
    source/Platform
)

# Define SDL_MAIN_HANDLED so SDL doesn't redefine main to SDL_main
target_compile_definitions(PicaSim PRIVATE SDL_MAIN_HANDLED)

# Link libraries
target_link_libraries(PicaSim PRIVATE
    Framework
    Heightfield
    Platform
    tinyxml
    SDL2::SDL2
    glm::glm
    OpenGL::GL
)

# Link Bullet Physics
if(TARGET Bullet3Common)
    # CMake-built Bullet
    target_link_libraries(PicaSim PRIVATE
        BulletDynamics
        BulletCollision
        BulletSoftBody
        LinearMath
    )
else()
    # Manual Bullet build
    target_link_libraries(PicaSim PRIVATE BulletPhysics)
endif()

target_include_directories(PicaSim PRIVATE ${BULLET_SOURCE_DIR})

# Link SDL2_net if available
if(TARGET SDL2_net::SDL2_net)
    target_link_libraries(PicaSim PRIVATE SDL2_net::SDL2_net)
endif()

# Link OpenAL
if(PICASIM_USE_OPENAL)
    target_link_libraries(PicaSim PRIVATE OpenAL::OpenAL)
endif()

# Windows-specific settings
if(WIN32)
    # Build as console app for now (shows console window but simplifies entry point)
    # TODO: Add proper WinMain wrapper for Release builds to hide console
    set_target_properties(PicaSim PROPERTIES
        WIN32_EXECUTABLE FALSE
    )

    # Copy SDL2 DLLs to output directory
    add_custom_command(TARGET PicaSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:PicaSim>
    )
endif()

# --------------------------------------------------------------------------
# Run target (convenience for running from data directory)
# --------------------------------------------------------------------------
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/data
            $<TARGET_FILE:PicaSim>
    DEPENDS PicaSim
    COMMENT "Running PicaSim from data directory"
    USES_TERMINAL
)

# --------------------------------------------------------------------------
# Install rules (for release packaging)
# --------------------------------------------------------------------------
install(TARGETS PicaSim
    RUNTIME DESTINATION .
)

# Install data directories
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemData
    DESTINATION .
    REGEX "Aeroplanes/TestWing" EXCLUDE
    REGEX "Aerofoils" EXCLUDE
)
# Aerofoils - only include XML files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemData/Aerofoils
    DESTINATION SystemData
    FILES_MATCHING PATTERN "*.xml"
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemSettings
    DESTINATION .
    REGEX "Environment/Flat\\.xml$" EXCLUDE
    REGEX "Aeroplane/TestWing" EXCLUDE
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/Menus
    DESTINATION .
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/Fonts
    DESTINATION .
)

# Create empty user directories
# install(DIRECTORY DESTINATION UserData)
# install(DIRECTORY DESTINATION UserSettings)

# Install LICENSE file
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt
    DESTINATION .
)

# Install DLLs on Windows - copy all from vcpkg bin directory
if(WIN32)
    file(GLOB VCPKG_DLLS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
    install(FILES ${VCPKG_DLLS}
        DESTINATION .
    )
endif()

# --------------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------------
message(STATUS "")
message(STATUS "PicaSim Configuration Summary:")
message(STATUS "  Platform: ${PICASIM_PLATFORM}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  OpenAL Audio: ${PICASIM_USE_OPENAL}")
message(STATUS "")
