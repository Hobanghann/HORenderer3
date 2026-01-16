message("\n\n\n######## Start to process ${CUSTOM_RENDERERS_NAME} module. ########\n\n\n")

################################################################################
# Read files
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
add_library(${CUSTOM_RENDERERS_NAME} STATIC ${ALL_FILES})

################################################################################
# Set target properties
################################################################################
target_link_libraries(${CUSTOM_RENDERERS_NAME} PUBLIC ${THIRD_PARTY_NAME} ${CORE_NAME} ${RESOURCE_NAME} ${VIRTUAL_GPU_NAME} ${RENDERER_NAME})
target_include_directories(${CUSTOM_RENDERERS_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${CUSTOM_RENDERERS_NAME} PRIVATE warning_flags_for_release)

message("\n\n\n######## Processing ${CUSTOM_RENDERERS_NAME} module finished. ########\n\n\n")