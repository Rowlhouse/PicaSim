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

# Compile definitions matching original Marmalade build
add_compile_definitions(
    BT_NO_PROFILE          # Disable Bullet physics profiling
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# --------------------------------------------------------------------------
# Dependencies: built from third_party/ submodules on all platforms.
# vcpkg provides only GLAD (Windows) and OpenXR (optional VR, Windows).
# --------------------------------------------------------------------------

# Verify submodules are initialized
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/SDL2/CMakeLists.txt)
    message(FATAL_ERROR "Git submodules not initialized. Run: git submodule update --init")
endif()

# --- Platform-specific build options ---
if(ANDROID)
    # Force VR off on Android
    set(PICASIM_ENABLE_VR OFF CACHE BOOL "" FORCE)

    # 16KB page size alignment required for Android 15+ (API 35)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,max-page-size=16384 -Wl,-z,common-page-size=16384")

    # Android: shared libraries (required for JNI System.loadLibrary)
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(LIBTYPE "SHARED" CACHE STRING "" FORCE)
    set(ALSOFT_BACKEND_OPENSL ON CACHE BOOL "" FORCE)
else()
    # Desktop: static libraries (no DLL management)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(LIBTYPE "STATIC" CACHE STRING "" FORCE)
endif()

# --- Common build options ---
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)

# --- Build dependencies from submodules ---
add_subdirectory(third_party/SDL2)
add_subdirectory(third_party/openal-soft)
add_subdirectory(third_party/glm)

# Convenience variable for SDL2 target (shared on Android, static on desktop)
if(ANDROID)
    set(SDL2_LIB SDL2)
else()
    set(SDL2_LIB SDL2-static)
endif()

# SDL2_net: manual build (avoids its CMakeLists.txt find_package(SDL2) issues)
add_library(SDL2_net STATIC
    third_party/SDL2_net/SDLnet.c
    third_party/SDL2_net/SDLnetTCP.c
    third_party/SDL2_net/SDLnetUDP.c
    third_party/SDL2_net/SDLnetselect.c
)
target_include_directories(SDL2_net PUBLIC third_party/SDL2_net)
target_link_libraries(SDL2_net PUBLIC ${SDL2_LIB})
if(WIN32)
    target_link_libraries(SDL2_net PRIVATE ws2_32 iphlpapi)
endif()

# stb (header-only)
set(STB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)

# Dear ImGui (manual static build)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
add_library(imgui_lib STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
if(ANDROID)
    target_compile_definitions(imgui_lib PUBLIC IMGUI_IMPL_OPENGL_ES2)
    target_link_libraries(imgui_lib PUBLIC GLESv2 ${SDL2_LIB})
else()
    target_link_libraries(imgui_lib PUBLIC ${SDL2_LIB})
endif()
add_library(imgui::imgui ALIAS imgui_lib)

# --- Desktop-only packages (vcpkg) ---
if(NOT ANDROID)
    find_package(OpenGL REQUIRED)

    if(WIN32)
        find_package(glad CONFIG REQUIRED)
    endif()

    # VR Support (optional, requires OpenXR)
    option(PICASIM_ENABLE_VR "Enable VR headset support (requires OpenXR)" ON)
    if(PICASIM_ENABLE_VR)
        find_package(OpenXR CONFIG REQUIRED)
        add_compile_definitions(PICASIM_VR_SUPPORT=1)
        message(STATUS "VR support: ENABLED")
    else()
        message(STATUS "VR support: DISABLED (use -DPICASIM_ENABLE_VR=ON to enable)")
    endif()
endif()

# --------------------------------------------------------------------------
# Third-party libraries (always built from source)
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
if(NOT MSVC)
    # Suppress warnings in third-party Bullet code
    target_compile_options(BulletPhysics PRIVATE -w)
endif()

# --------------------------------------------------------------------------
# Framework library
# --------------------------------------------------------------------------
file(GLOB FRAMEWORK_SOURCES CONFIGURE_DEPENDS source/Framework/*.cpp)
file(GLOB FRAMEWORK_HEADERS CONFIGURE_DEPENDS source/Framework/*.h)

add_library(Framework STATIC ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS})
if(NOT MSVC)
    # Suppress dangling-else warnings from TRACE macros
    target_compile_options(Framework PRIVATE -Wno-dangling-else)
endif()
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
    glm::glm-header-only
    imgui::imgui
    OpenAL::OpenAL
)
if(ANDROID)
    target_link_libraries(Framework PUBLIC GLESv2)
else()
    target_link_libraries(Framework PUBLIC OpenGL::GL)
    if(WIN32)
        target_link_libraries(Framework PUBLIC glad::glad)
    endif()
endif()

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
    ${SDL2_LIB}
    glm::glm-header-only
    imgui::imgui
    OpenAL::OpenAL
)
if(ANDROID)
    target_link_libraries(Platform PUBLIC GLESv2 EGL log android)
else()
    target_link_libraries(Platform PUBLIC OpenGL::GL)
    if(WIN32)
        target_link_libraries(Platform PUBLIC glad::glad)
        target_link_libraries(Platform PRIVATE shlwapi)
    endif()
    if(PICASIM_ENABLE_VR)
        target_link_libraries(Platform PUBLIC OpenXR::openxr_loader)
    endif()
endif()

# --------------------------------------------------------------------------
# PicaSim game code
# --------------------------------------------------------------------------
file(GLOB PICASIM_SOURCES CONFIGURE_DEPENDS source/PicaSim/*.cpp)
file(GLOB PICASIM_HEADERS CONFIGURE_DEPENDS source/PicaSim/*.h)
file(GLOB PICASIM_LANGUAGE_SOURCES CONFIGURE_DEPENDS source/PicaSim/Languages/*.cpp)
file(GLOB PICASIM_MENU_SOURCES CONFIGURE_DEPENDS source/PicaSim/Menus/*.cpp)

# --------------------------------------------------------------------------
# Main target
# --------------------------------------------------------------------------
if(ANDROID)
    # Android: build as shared library loaded by SDLActivity via JNI
    add_library(PicaSim SHARED
        ${PICASIM_SOURCES}
        ${PICASIM_HEADERS}
        ${PICASIM_LANGUAGE_SOURCES}
        ${PICASIM_MENU_SOURCES}
    )
else()
    set(WIN32_RESOURCES "")
    if(WIN32)
        set(WIN32_RESOURCES ${CMAKE_SOURCE_DIR}/resources/PicaSim.rc)
    endif()
    add_executable(PicaSim
        ${PICASIM_SOURCES}
        ${PICASIM_HEADERS}
        ${PICASIM_LANGUAGE_SOURCES}
        ${PICASIM_MENU_SOURCES}
        ${WIN32_RESOURCES}
    )
endif()

if(NOT MSVC)
    # Suppress dangling-else warnings from TRACE macros
    target_compile_options(PicaSim PRIVATE -Wno-dangling-else)
endif()

target_include_directories(PicaSim PRIVATE
    source/PicaSim
    source/PicaSim/Menus
    source/PicaSim/Languages
    source/Framework
    source/Heightfield
    source/tinyxml
    source/Platform
)

if(NOT ANDROID)
    # Define SDL_MAIN_HANDLED so SDL doesn't redefine main to SDL_main
    # On Android, SDL must own main() for proper JNI lifecycle
    target_compile_definitions(PicaSim PRIVATE SDL_MAIN_HANDLED)
endif()

# Link libraries
target_link_libraries(PicaSim PRIVATE
    Framework
    Heightfield
    Platform
    tinyxml
    ${SDL2_LIB}
    glm::glm-header-only
    BulletPhysics
    OpenAL::OpenAL
    SDL2_net
)
if(ANDROID)
    target_link_libraries(PicaSim PRIVATE SDL2main GLESv2 EGL log android)
else()
    target_link_libraries(PicaSim PRIVATE OpenGL::GL)
endif()

target_include_directories(PicaSim PRIVATE ${BULLET_SOURCE_DIR})

# Windows-specific settings
if(WIN32)
    # Build as console app for now (shows console window but simplifies entry point)
    # TODO: Add proper WinMain wrapper for Release builds to hide console
    set_target_properties(PicaSim PROPERTIES
        WIN32_EXECUTABLE FALSE
    )

endif()

# Copy VERSIONS.txt to data directory for runtime access
if(NOT ANDROID)
    add_custom_command(TARGET PicaSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/VERSIONS.txt
            ${CMAKE_SOURCE_DIR}/data/VERSIONS.txt
        COMMENT "Copying VERSIONS.txt to data directory"
    )
endif()

# Generate asset manifest for Android APK asset extraction
if(ANDROID)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_FOUND)
        add_custom_command(TARGET PicaSim PRE_BUILD
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/generate_asset_manifest.py
                ${CMAKE_SOURCE_DIR}/data
                ${CMAKE_SOURCE_DIR}/data/assets_manifest.txt
                ${CMAKE_SOURCE_DIR}/packaging_excludes.txt
            COMMENT "Generating asset manifest for Android"
        )
    endif()
endif()

# --------------------------------------------------------------------------
# Run target (convenience for running from data directory)
# --------------------------------------------------------------------------
if(NOT ANDROID)
    add_custom_target(run
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/data
                $<TARGET_FILE:PicaSim>
        DEPENDS PicaSim
        COMMENT "Running PicaSim from data directory"
        USES_TERMINAL
    )
endif()

# --------------------------------------------------------------------------
# Install rules (for release packaging)
# --------------------------------------------------------------------------
if(NOT ANDROID)
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

    # Parse packaging_excludes.txt for install rules
    set(_excludes_file "${CMAKE_SOURCE_DIR}/packaging_excludes.txt")
    set(_sd_excludes "")
    set(_ss_excludes "")
    set(_xml_only_dirs "")
    if(EXISTS ${_excludes_file})
        file(STRINGS ${_excludes_file} _exclude_lines)
        foreach(_line IN LISTS _exclude_lines)
            # Skip comments and blank lines
            string(STRIP "${_line}" _line)
            if(_line STREQUAL "" OR _line MATCHES "^#")
                continue()
            endif()
            # xml-only: prefix
            if(_line MATCHES "^xml-only:(.*)")
                list(APPEND _xml_only_dirs "${CMAKE_MATCH_1}")
                continue()
            endif()
            # Plain path exclusion - route to correct install command
            if(_line MATCHES "^SystemData/(.*)")
                set(_pattern "${CMAKE_MATCH_1}")
                string(REPLACE "." "\\." _pattern "${_pattern}")
                if(_line MATCHES "\\.[a-zA-Z]+$")
                    string(APPEND _pattern "$")
                endif()
                list(APPEND _sd_excludes REGEX "${_pattern}" EXCLUDE)
            elseif(_line MATCHES "^SystemSettings/(.*)")
                set(_pattern "${CMAKE_MATCH_1}")
                string(REPLACE "." "\\." _pattern "${_pattern}")
                if(_line MATCHES "\\.[a-zA-Z]+$")
                    string(APPEND _pattern "$")
                endif()
                list(APPEND _ss_excludes REGEX "${_pattern}" EXCLUDE)
            endif()
        endforeach()
    endif()

    # Install SystemData (with exclusions and xml-only dirs fully excluded here)
    set(_sd_full_excludes ${_sd_excludes})
    foreach(_xo IN LISTS _xml_only_dirs)
        if(_xo MATCHES "^SystemData/(.*)")
            list(APPEND _sd_full_excludes REGEX "${CMAKE_MATCH_1}" EXCLUDE)
        endif()
    endforeach()
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemData
        DESTINATION .
        ${_sd_full_excludes}
    )
    # Re-install xml-only dirs with FILES_MATCHING
    foreach(_xo IN LISTS _xml_only_dirs)
        if(_xo MATCHES "^SystemData/(.*)")
            set(_xo_subdir "${CMAKE_MATCH_1}")
            install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemData/${_xo_subdir}
                DESTINATION SystemData
                FILES_MATCHING PATTERN "*.xml"
            )
        endif()
    endforeach()

    # Install SystemSettings (with exclusions)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/SystemSettings
        DESTINATION .
        ${_ss_excludes}
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

    # Install vcpkg DLLs on Windows (GLAD, OpenXR)
    if(WIN32)
        file(GLOB VCPKG_DLLS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
        install(FILES ${VCPKG_DLLS}
            DESTINATION .
        )
    endif()
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
