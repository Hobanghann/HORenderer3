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
