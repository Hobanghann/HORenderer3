message("\n\n\n######## Start to process ${RENDERER_NAME} module. ########\n\n\n")

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
add_library(${RENDERER_NAME} STATIC ${ALL_FILES})

################################################################################
# Set target properties
################################################################################
target_link_libraries(${RENDERER_NAME} PRIVATE warning_flags PUBLIC ${THIRD_PARTY_NAME} ${CORE_NAME} ${RESOURCE_NAME} ${VIRTUAL_GPU_NAME})
target_include_directories(${RENDERER_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

message("\n\n\n######## Processing ${RENDERER_NAME} module finished. ########\n\n\n")
