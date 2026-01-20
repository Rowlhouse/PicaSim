# Platform.cmake - Platform detection and configuration helpers

# Detect the current platform
function(picasim_detect_platform)
    if(WIN32)
        set(PICASIM_PLATFORM_NAME "Windows" PARENT_SCOPE)
        set(PICASIM_PLATFORM_WINDOWS TRUE PARENT_SCOPE)
    elseif(APPLE)
        if(IOS)
            set(PICASIM_PLATFORM_NAME "iOS" PARENT_SCOPE)
            set(PICASIM_PLATFORM_IOS TRUE PARENT_SCOPE)
        else()
            set(PICASIM_PLATFORM_NAME "macOS" PARENT_SCOPE)
            set(PICASIM_PLATFORM_MACOS TRUE PARENT_SCOPE)
        endif()
    elseif(ANDROID)
        set(PICASIM_PLATFORM_NAME "Android" PARENT_SCOPE)
        set(PICASIM_PLATFORM_ANDROID TRUE PARENT_SCOPE)
    elseif(UNIX)
        set(PICASIM_PLATFORM_NAME "Linux" PARENT_SCOPE)
        set(PICASIM_PLATFORM_LINUX TRUE PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endfunction()

# Configure platform-specific compile definitions
function(picasim_configure_platform_definitions target)
    if(PICASIM_PLATFORM_WINDOWS)
        target_compile_definitions(${target} PRIVATE
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            _CRT_SECURE_NO_WARNINGS
        )
    endif()

    if(PICASIM_PLATFORM_ANDROID OR PICASIM_PLATFORM_IOS)
        target_compile_definitions(${target} PRIVATE
            PICASIM_MOBILE=1
        )
    endif()
endfunction()

# Set up platform-specific compiler flags
function(picasim_configure_compiler_flags target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4           # Warning level 4
            /MP           # Multi-processor compilation
            $<$<CONFIG:Release>:/O2>
            $<$<CONFIG:Debug>:/Od /Zi>
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Debug>:-O0 -g>
        )
    endif()
endfunction()
