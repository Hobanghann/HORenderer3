##########################################################
# CMake settings
##########################################################

cmake_minimum_required(VERSION 3.4)
include(FetchContent)
set(MAIN_PROJECT_NAME HORenderer)

project(${MAIN_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

##########################################################
# Set Compiler Options
##########################################################
add_library(warning_flags INTERFACE)

if (MSVC)
    target_compile_options(
        warning_flags
        INTERFACE
        /W4               # strong warnings
        /WX               # warnings as errors
        /permissive-      # strict C++ mode
        /Zc:__cplusplus
        /Zc:preprocessor
        /wd4201           # allow anonymous struct, union
        /wd4723           # disable: potential divide by zero (false positive)
    )

else ()
    # Common flags for GCC/Clang
    target_compile_options(
        warning_flags
        INTERFACE
        -Werror -Wall -Wextra -Wpedantic
        -Wconversion -Wsign-conversion
        -Wshadow -Wundef
        -Wunreachable-code
        -Wstrict-aliasing
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wcast-qual
        -Wcast-align
        -fms-extensions # allow anonymous struct, union
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(
            warning_flags
            INTERFACE
            -Wuseless-cast            
        )

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Clang: careful, do NOT use -Weverything (extremely noisy)
        target_compile_options(
            warning_flags
            INTERFACE
            -Wfloat-equal
            -Woverloaded-virtual
            -Wmissing-prototypes
            -Wtautological-compare
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
        )
    endif ()
endif ()

##########################################################
# Set module's Name
##########################################################
set(PLATFORMS_NAME platforms)
set(THIRD_PARTY_NAME lib)
set(CORE_NAME core)

##########################################################
# Process modules
##########################################################
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platforms)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)

##########################################################
# Process tests
##########################################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
