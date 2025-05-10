cmake_minimum_required( VERSION 3.8 )
project( WndAPI
         LANGUAGES CXX
)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
    message( FATAL_ERROR "In-tree builds are not supported. Run CMake from a separate directory: cmake -B build" )
endif()

if( WIN32 )
    set( CMAKE_CXX_STANDARD 20 )
    set( CMAKE_CXX_STANDARD_REQUIRED ON )

    set( WndAPI_SRC
        src/main.cpp
        src/wnd.cpp
    )

    list( APPEND WndAPI_SRC src/pch.cpp )
    add_executable( WndAPI WIN32 ${WndAPI_SRC})
    target_precompile_headers( WndAPI PRIVATE src/pch.h )

    target_link_libraries( WndAPI
        d3d11
        dxgi
    )

    if( MSVC )
        set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )

        target_compile_options( WndAPI PRIVATE 
            /wd28251        # Suppress warning C28251
            
            # Compiler/build optimizations
            /MP             # Enable multi-processor compilation
            /Ot             # Optimize for speed
            /fp:fast        # Fast floating-point model
        )
    endif()

    target_compile_definitions( WndAPI PRIVATE 
        $<$<CONFIG:Release>:NDEBUG>
    )
else()
    message( FATAL_ERROR "This project is specifically designed for Windows. It cannot be built on ${CMAKE_SYSTEM_NAME}." )
endif()