message("\n\n\n######## Start to process ${PLATFORMS_NAME} module. ########\n\n\n")

################################################################################
# Check Platform
################################################################################

# Windows
if (WIN32)
  set(PLATFORM_FOLDER windows)
# Apple
elseif(APPLE)
  message(FATAL_ERROR "This project only supports windows environment currently.")
  set(PLATFORM_FOLDER apple)
# Linux
elseif(UNIX AND NOT APPLE)
  message(FATAL_ERROR "This project only supports windows environment currently.")
  set(PLATFORM_FOLDER linux)
endif()

################################################################################
# Read source files
################################################################################
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_FOLDER}/*.cc)
list(FILTER SOURCES EXCLUDE REGEX "/main\\.cc$")
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_FOLDER}/*.h)

set(ALL_FILES ${SOURCES} ${HEADERS})

################################################################################
# Repackiging source files
################################################################################
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

################################################################################
# Add library target
################################################################################
add_library(${PLATFORMS_NAME} STATIC ${ALL_FILES})

################################################################################
# Set target properties
################################################################################
target_link_libraries(
  ${PLATFORMS_NAME}
  PRIVATE
  warning_flags
  PUBLIC
  ${THIRD_PARTY_NAME}
  ${CORE_NAME}
  ${RESOURCE_NAME}
  ${VIRTUAL_GPU_NAME}
  ${RENDERER_NAME}
  ${CUSTOM_RENDERERS_NAME}
)
target_include_directories(
  ${PLATFORMS_NAME}
  PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

################################################################################
# Add executable target
################################################################################
# Windows
if (WIN32)
  add_executable(${MAIN_PROJECT_NAME} WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_FOLDER}/main.cc)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${MAIN_PROJECT_NAME})
  # Apple
elseif(APPLE)
  message(FATAL_ERROR "This project only supports windows environment currently.")
  add_executable(${MAIN_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_FOLDER}/main.cc)
# Linux
elseif(UNIX AND NOT APPLE)
  message(FATAL_ERROR "This project only supports windows environment currently.")
  add_executable(${MAIN_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_FOLDER}/main.cc)
endif()

################################################################################
# Set target properties
################################################################################

target_link_libraries(
  ${MAIN_PROJECT_NAME}
  PRIVATE
  warning_flags
  PUBLIC  
  ${THIRD_PARTY_NAME}
  ${CORE_NAME}
  ${RESOURCE_NAME}
  ${VIRTUAL_GPU_NAME}
  ${RENDERER_NAME}
  ${CUSTOM_RENDERERS_NAME}
  ${PLATFORMS_NAME}
)

target_include_directories(
  ${MAIN_PROJECT_NAME}
  PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
message("\n\n\n######## Processing ${PLATFORMS_NAME} module finished. ########\n\n\n")
