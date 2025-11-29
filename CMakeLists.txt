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
if (MSVC)
    add_compile_options(
        /W4           # strong warnings
        /WX           # warnings as errors
        /permissive-  # strict C++ mode
        /Zc:__cplusplus
        /Zc:preprocessor
    )

else ()
    # Common flags for GCC/Clang
    add_compile_options(
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
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(
            -Wuseless-cast
        )

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Clang: careful, do NOT use -Weverything (extremely noisy)
        add_compile_options(
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
