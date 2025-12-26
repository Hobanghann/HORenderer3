message("\n\n\n######## Start to process ${RESOURCE_NAME} module. ########\n\n\n")
################################################################################
# define variables
################################################################################
set(HO_PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
set(HO_ASSET_ROOT "${CMAKE_SOURCE_DIR}/assets")

################################################################################
# Read source files
################################################################################
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(ALL_FILES ${SOURCES} ${HEADERS})

################################################################################
# Repackiging source files
################################################################################
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

################################################################################
# Add library target
################################################################################
add_library(${RESOURCE_NAME} STATIC ${ALL_FILES})

################################################################################
# Set target properties
################################################################################
target_link_libraries(${RESOURCE_NAME} PRIVATE warning_flags PUBLIC ${THIRD_PARTY_NAME} ${CORE_NAME})
target_include_directories(${RESOURCE_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

message("\n\n\n######## Processing ${RESOURCE_NAME} module finished. ########\n\n\n")
