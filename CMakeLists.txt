cmake_minimum_required(VERSION 3.20)
project(zwidget VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Include directories
include_directories(include)

# Source files
set(ZWIDGET_SOURCES
    include/zwidget/modules/window.cpp
	include/zwidget/modules/render/d2d/context.cpp
	src/main.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${ZWIDGET_SOURCES})

# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
        /Zc:__cplusplus
    >
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wno-c++98-compat
        -Wno-pre-c++17-compat
    >
)

# Link Windows libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        user32
        gdi32
        winmm
        dwmapi
    )
endif()

# Optional: Build as library
option(ZWIDGET_BUILD_LIBRARY "Build zwidget as a library" OFF)

if(ZWIDGET_BUILD_LIBRARY)
    add_library(zwidget_lib STATIC ${ZWIDGET_SOURCES})
    target_include_directories(zwidget_lib PUBLIC include)
    target_compile_options(zwidget_lib PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    )
    if(WIN32)
        target_link_libraries(zwidget_lib PUBLIC
            user32 gdi32 winmm dwmapi
        )
    endif()
endif()

# Optional: Examples
option(ZWIDGET_BUILD_EXAMPLES "Build examples" ON)

if(ZWIDGET_BUILD_EXAMPLES)
    # Add example projects here later
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/zwidget
    DESTINATION include
)