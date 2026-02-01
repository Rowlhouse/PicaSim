# PicaSim CMake Build System
# Migrating from Marmalade SDK to SDL2 + OpenGL
cmake_minimum_required(VERSION 3.20)
project(PicaSim VERSION 1.0.0 LANGUAGES CXX C)

# Read version from VERSIONS.txt (single source of truth for version number)
# Mark as configure dependency so CMake reconfigures when version changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/VERSIONS.txt)
if(EXISTS ${CMAKE_SOURCE_DIR}/VERSIONS.txt)
    file(STRINGS ${CMAKE_SOURCE_DIR}/VERSIONS.txt VERSION_LINE LIMIT_COUNT 1)
    string(REGEX MATCH "Version ([0-9]+\\.[0-9]+\\.[0-9]+)" _ ${VERSION_LINE})
    if(CMAKE_MATCH_1)
        set(PICASIM_VERSION ${CMAKE_MATCH_1})
    else()
        set(PICASIM_VERSION "1.0.0")
    endif()
else()
    set(PICASIM_VERSION "1.0.0")
endif()
# Create versioned directory name
set(PICASIM_VERSION_DIR "PicaSim-${PICASIM_VERSION}")
message(STATUS "PicaSim version: ${PICASIM_VERSION} (install dir: ${PICASIM_VERSION_DIR})")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# --------------------------------------------------------------------------
# AI generated fix for MacOS
# --------------------------------------------------------------------------
if(APPLE)
    add_compile_options(-include "${CMAKE_SOURCE_DIR}/source/Platform/GLCompat.h")
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    add_compile_options(-Wno-deprecated-declarations)
endif()

list(REMOVE_ITEM PICASIM_SOURCES
    ${CMAKE_SOURCE_DIR}/source/PicaSim/ConnectionListener.cpp
    ${CMAKE_SOURCE_DIR}/source/PicaSim/IncomingConnection.cpp
    ${CMAKE_SOURCE_DIR}/source/PicaSim/ConnectionListener.h
    ${CMAKE_SOURCE_DIR}/source/PicaSim/IncomingConnection.h
)


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

# VR Support (optional, requires OpenXR - not available on macOS)
if(PICASIM_PLATFORM STREQUAL "MACOS")
    set(PICASIM_ENABLE_VR OFF CACHE BOOL "Enable VR headset support (requires OpenXR - not available on macOS)" FORCE)
else()
    option(PICASIM_ENABLE_VR "Enable VR headset support (requires OpenXR)" ON)
endif()

if(PICASIM_ENABLE_VR)
    find_package(OpenXR CONFIG REQUIRED)
    add_compile_definitions(PICASIM_VR_SUPPORT=1)
    message(STATUS "VR support: ENABLED")
else()
    message(STATUS "VR support: DISABLED (use -DPICASIM_ENABLE_VR=ON to enable)")
endif()

# GLM for math types
find_package(glm CONFIG REQUIRED)

# OpenAL for audio
find_package(OpenAL CONFIG REQUIRED)

# OpenGL
# Prefer the platform OpenGL framework on macOS instead of any Brew GLVND/Mesa libs
if(APPLE)
    set(OpenGL_GL_PREFERENCE LEGACY)
endif()
find_package(OpenGL REQUIRED)
set(PICASIM_OPENGL_LIB OpenGL::GL)
if(APPLE AND TARGET OpenGL::GL)
    set(PICASIM_OPENGL_LIB OpenGL::GL)
    set(PICASIM_APPLE_OPENGL_TBD "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL.tbd")
    if(NOT EXISTS "${PICASIM_APPLE_OPENGL_TBD}")
        set(PICASIM_APPLE_OPENGL_TBD "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL.tbd")
    endif()
    set_target_properties(OpenGL::GL PROPERTIES
        IMPORTED_LOCATION "${PICASIM_APPLE_OPENGL_TBD}"
        INTERFACE_INCLUDE_DIRECTORIES ""
        INTERFACE_LINK_OPTIONS "-framework;OpenGL")
endif()

# --------------------------------------------------------------------------
# Third-party libraries
# --------------------------------------------------------------------------

# TinyXML (compiled from source)
file(GLOB TINYXML_SOURCES CONFIGURE_DEPENDS source/tinyxml/*.cpp)
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
file(GLOB FRAMEWORK_SOURCES CONFIGURE_DEPENDS source/Framework/*.cpp)
file(GLOB FRAMEWORK_HEADERS CONFIGURE_DEPENDS source/Framework/*.h)

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
    ${PICASIM_OPENGL_LIB}
    imgui::imgui
)
if(WIN32)
    target_link_libraries(Framework PUBLIC glad::glad)
endif()

# Link OpenAL for AudioManager
target_link_libraries(Framework PUBLIC OpenAL::OpenAL)

# --------------------------------------------------------------------------
# Heightfield library
# --------------------------------------------------------------------------
file(GLOB HEIGHTFIELD_SOURCES CONFIGURE_DEPENDS source/Heightfield/*.cpp)

add_library(Heightfield STATIC ${HEIGHTFIELD_SOURCES})
target_include_directories(Heightfield PUBLIC source/Heightfield)
target_link_libraries(Heightfield PUBLIC Framework)

# --------------------------------------------------------------------------
# Platform abstraction layer (new SDL2-based implementation)
# --------------------------------------------------------------------------
file(GLOB PLATFORM_SOURCES CONFIGURE_DEPENDS source/Platform/*.cpp)
file(GLOB PLATFORM_HEADERS CONFIGURE_DEPENDS source/Platform/*.h)

# Exclude VR support files on macOS (OpenXR not available)
if(PICASIM_PLATFORM STREQUAL "MACOS")
    list(REMOVE_ITEM PLATFORM_SOURCES
        ${CMAKE_SOURCE_DIR}/source/Platform/OpenXRRuntime.cpp
        ${CMAKE_SOURCE_DIR}/source/Platform/VRManager.cpp
    )
endif()

add_library(Platform STATIC ${PLATFORM_SOURCES} ${PLATFORM_HEADERS})
target_include_directories(Platform PUBLIC
    source/Platform
    ${STB_INCLUDE_DIRS}
    ${BULLET_SOURCE_DIR}
)
target_link_libraries(Platform PUBLIC
    SDL2::SDL2
    glm::glm
    ${PICASIM_OPENGL_LIB}
    imgui::imgui
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(Platform PUBLIC glad::glad)
    target_link_libraries(Platform PRIVATE shlwapi)
endif()

# VR support (OpenXR)
if(PICASIM_ENABLE_VR)
    target_link_libraries(Platform PUBLIC OpenXR::openxr_loader)
endif()

# --------------------------------------------------------------------------
# PicaSim game code
# --------------------------------------------------------------------------
file(GLOB PICASIM_SOURCES CONFIGURE_DEPENDS source/PicaSim/*.cpp)
file(GLOB PICASIM_HEADERS CONFIGURE_DEPENDS source/PicaSim/*.h)
file(GLOB PICASIM_LANGUAGE_SOURCES CONFIGURE_DEPENDS source/PicaSim/Languages/*.cpp)
file(GLOB PICASIM_MENU_SOURCES CONFIGURE_DEPENDS source/PicaSim/Menus/*.cpp)

# --------------------------------------------------------------------------
# Main executable
# --------------------------------------------------------------------------
add_executable(PicaSim
    ${PICASIM_SOURCES}
    ${PICASIM_HEADERS}
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
find_package(SDL2_net CONFIG QUIET)

if(TARGET SDL2_net::SDL2_net)
  target_link_libraries(PicaSim PRIVATE SDL2_net::SDL2_net)
elseif(TARGET SDL2_net::SDL2_net-static)
  target_link_libraries(PicaSim PRIVATE SDL2_net::SDL2_net-static)
else()
  message(WARNING "SDL2_net not found: network features will not link")
endif()

# Link OpenAL
target_link_libraries(PicaSim PRIVATE OpenAL::OpenAL)

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

# Copy VERSIONS.txt to data directory for runtime access
add_custom_command(TARGET PicaSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/VERSIONS.txt
        ${CMAKE_SOURCE_DIR}/data/VERSIONS.txt
    COMMENT "Copying VERSIONS.txt to data directory"
)

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
# Always install to dist/<version_dir> (e.g., dist/PicaSim-1_0_0)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist/${PICASIM_VERSION_DIR}" CACHE PATH "" FORCE)

# Clean the install directory first to prevent stale files
install(CODE "
    message(STATUS \"Cleaning install directory: \${CMAKE_INSTALL_PREFIX}\")
    file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}\")
")

install(TARGETS PicaSim
    RUNTIME DESTINATION .
)

# Install data directories
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemData
    DESTINATION .
    REGEX "Aeroplanes/Ahi" EXCLUDE
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
    REGEX "Aeroplane/Ahi" EXCLUDE
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/Menus
    DESTINATION .
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/Fonts
    DESTINATION .
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemData/Icons
    DESTINATION SystemData
)

# Install LICENSE and VERSIONS files
install(FILES
    ${CMAKE_SOURCE_DIR}/LICENSE.txt
    ${CMAKE_SOURCE_DIR}/VERSIONS.txt
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
# macOS App Bundle and DMG targets
# --------------------------------------------------------------------------
if(APPLE AND NOT IOS)
    # Create macOS .app bundle from installed distribution
    add_custom_target(macos-app-bundle
        COMMAND bash ${CMAKE_SOURCE_DIR}/macos_create_app_bundle.sh
            ${CMAKE_INSTALL_PREFIX}
            dist
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating macOS .app bundle"
    )
    add_dependencies(macos-app-bundle PicaSim)
    
    # Create macOS .dmg (disk image) for distribution
    add_custom_target(macos-dmg
        COMMAND bash ${CMAKE_SOURCE_DIR}/macos_create_dmg.sh
            dist/PicaSim.app
            dist
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating macOS .dmg for distribution"
    )
    add_dependencies(macos-dmg macos-app-bundle)
    
    message(STATUS "macOS targets available:")
    message(STATUS "  cmake --build <preset> --target macos-app-bundle  (create .app)")
    message(STATUS "  cmake --build <preset> --target macos-dmg         (create .dmg)")
endif()

# --------------------------------------------------------------------------
# Installer target (Windows only, requires Inno Setup)
# --------------------------------------------------------------------------
if(WIN32)
    find_program(ISCC_EXECUTABLE NAMES iscc ISCC
        PATHS "C:/Program Files (x86)/Inno Setup 6"
              "C:/Program Files/Inno Setup 6"
    )
    if(ISCC_EXECUTABLE)
        add_custom_target(installer
            COMMAND ${ISCC_EXECUTABLE}
                /DMyAppVersion=${PICASIM_VERSION}
                ${CMAKE_SOURCE_DIR}/installer/PicaSim.iss
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/installer
            COMMENT "Building installer with Inno Setup"
        )
        add_dependencies(installer PicaSim)
        message(STATUS "Inno Setup found: ${ISCC_EXECUTABLE}")
    else()
        message(STATUS "Inno Setup not found - installer target disabled")
    endif()
endif()

# --------------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------------
message(STATUS "")
message(STATUS "PicaSim Configuration Summary:")
message(STATUS "  Platform: ${PICASIM_PLATFORM}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  OpenAL Audio: ON")
if(PICASIM_ENABLE_VR)
    message(STATUS "  VR Support: ON (OpenXR)")
else()
    message(STATUS "  VR Support: OFF")
endif()
message(STATUS "")
