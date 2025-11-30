message("\n\n\n######## Start to process ${CORE_NAME} module. ########\n\n\n")

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
add_library(${CORE_NAME} STATIC ${ALL_FILES})

################################################################################
# Set target properties
################################################################################
target_include_directories(${CORE_NAME}
PUBLIC
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_SOURCE_DIR}
	)
target_link_libraries(${CORE_NAME} PUBLIC warning_flags ${THIRD_PARTY_NAME})
message("\n\n\n######## Processing ${CORE_NAME} module finished. ########\n\n\n")
