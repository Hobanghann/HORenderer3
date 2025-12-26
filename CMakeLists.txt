##########################################################
# CMake settings
##########################################################

cmake_minimum_required(VERSION 3.20)
include(FetchContent)
set(MAIN_PROJECT_NAME HORenderer)

project(${MAIN_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

option(ENABLE_CLANG_TIDY "Build only what is necessary for clang-tidy" OFF)

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

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Clang: careful, do NOT use -Weverything (extremely noisy)
        target_compile_options(
            warning_flags
            INTERFACE
            -Werror -Wall -Wextra
            -Wconversion -Wsign-conversion
            -Wshadow -Wundef
            -Wunreachable-code
            -Wstrict-aliasing
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wcast-qual
            -Wcast-align
            -fms-extensions
            -Woverloaded-virtual
            -Wmissing-prototypes
            -Wtautological-compare
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-gnu-anonymous-struct    # allow anonymous struct/union
            -Wno-double-promotion       # allow float to double promotion
        )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")        
    message(FATAL_ERROR
        "GCC is not supported by HORenderer due to its restrictions on "
        "anonymous aggregates with non-POD members. "
        "Please use Clang or MSVC instead."
    )
else()
    message(WARNING
        "Unknown compiler '${CMAKE_CXX_COMPILER_ID}'. "
        "HORenderer officially supports only MSVC and Clang. "
        "Compilation may work, but is not tested."
    )
endif ()

##########################################################
# Set module's Name
##########################################################
set(PLATFORMS_NAME platforms)
set(THIRD_PARTY_NAME lib)
set(CORE_NAME core)
set(RESOURCE_NAME resource)

##########################################################
# Process modules
##########################################################
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/platforms)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/resource)

##########################################################
# Process tests
##########################################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
