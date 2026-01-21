# PicaSim CMake Build System
# Migrating from Marmalade SDK to SDL2 + OpenGL
cmake_minimum_required(VERSION 3.20)
project(PicaSim VERSION 1.0.0 LANGUAGES CXX C)

# Read version from VERSIONS.txt (single source of truth for version number)
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
# Create directory-safe version (1.0.0 -> 1_0_0)
string(REPLACE "." "_" PICASIM_VERSION_DIR "PicaSim-${PICASIM_VERSION}")
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
find_package(OpenAL CONFIG REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

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
    OpenGL::GL
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
if(TARGET SDL2_net::SDL2_net)
    target_link_libraries(PicaSim PRIVATE SDL2_net::SDL2_net)
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
message(STATUS "")
