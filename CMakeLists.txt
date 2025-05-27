cmake_minimum_required(VERSION 3.8)

# Project declaration
project( WndAPI 
         LANGUAGES CXX
)

# Prevent in-source builds
if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
    message( FATAL_ERROR "In-tree builds are not supported. Run CMake from a separate directory: cmake -B build" )
endif()

# Global compile settings
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Only build on Windows
if( NOT WIN32 )
    message( FATAL_ERROR "This project is specifically designed for Windows. It cannot be built on ${CMAKE_SYSTEM_NAME}." )
endif()

# ImGui sources
file( GLOB IMGUI_SOURCES
      ${CMAKE_SOURCE_DIR}/imgui/*.cpp
)

# Application sources
set( APP_SOURCES
     ${CMAKE_SOURCE_DIR}/src/d3d.cpp
     ${CMAKE_SOURCE_DIR}/src/main.cpp
     ${CMAKE_SOURCE_DIR}/src/pch.cpp
     ${CMAKE_SOURCE_DIR}/src/wnd.cpp
)

# Executable target
add_executable( WndAPI
    ${IMGUI_SOURCES}
    ${APP_SOURCES}
)

# MSVC-specific settings: runtime library and optimization flags
if( MSVC )
    target_compile_options( WndAPI 
        PRIVATE
            /MP            # parallel builds
            /W3            # warning level 3
            /permissive-   # standards conformance
            /Zc:inline     # inline semantics
            /fp:fast       # fast floating-point
    )

    target_compile_options( WndAPI 
        PRIVATE
            $<$<CONFIG:Debug>:/MDd>   # dynamic CRT debug
            $<$<CONFIG:Debug>:/Od>    # no optimization
            $<$<CONFIG:Debug>:/Zi>    # debug info
            $<$<CONFIG:Debug>:/RTC1>  # runtime checks
    )

    target_compile_options( WndAPI 
        PRIVATE
            $<$<CONFIG:Release>:/MD>   # dynamic CRT release
            $<$<CONFIG:Release>:/O2>   # optimization level 2
            $<$<CONFIG:Release>:/Ob2>  # inline expansion
            $<$<CONFIG:Release>:/Oi>   # intrinsics
            $<$<CONFIG:Release>:/GL>   # whole program optimization
    )
endif()

# Precompiled header usage
target_precompile_headers( WndAPI 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/pch.h
)

# Include directories
target_include_directories( WndAPI 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/imgui
        ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries( WndAPI
    PRIVATE
        d3d11
        dxgi
)

# Definitions
target_compile_definitions( WndAPI 
    PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

# Windows subsystem
set_target_properties( WndAPI 
    PROPERTIES
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "WndAPI"
)

# Export compile commands for tooling
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

message( STATUS "Building WndAPI (${CMAKE_BUILD_TYPE}) on ${CMAKE_SYSTEM_NAME}" )