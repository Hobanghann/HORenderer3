message("\n\n\n######## Start to process ${CORE_NAME} module. ########\n\n\n")
################################################################################
# define variables
################################################################################
set(HO_PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
set(HO_ASSET_ROOT "${CMAKE_SOURCE_DIR}/assets")

set(TIDY_EXCLUDE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/io/resource_importer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/io/resource_importer.h
)

################################################################################
# Read source files
################################################################################
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(ALL_FILES ${SOURCES} ${HEADERS})

# if (ENABLE_CLANG_TIDY)
#   foreach(exclude_file IN LISTS TIDY_EXCLUDE_FILES)
#     list(FILTER ALL_FILES EXCLUDE REGEX "${exclude_file}$")
#   endforeach()
# endif()

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
if (ENABLE_CLANG_TIDY)
  target_include_directories(
    ${CORE_NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}
	)
  target_link_libraries(${CORE_NAME} PRIVATE warning_flags PUBLIC ${THIRD_PARTY_NAME})

else()
  target_include_directories(
    ${CORE_NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}
	)
  target_link_libraries(${CORE_NAME} PRIVATE warning_flags PUBLIC ${THIRD_PARTY_NAME})
endif()

target_compile_definitions(
  ${CORE_NAME} 
  PUBLIC
  HO_PROJECT_ROOT="${HO_PROJECT_ROOT}"
  HO_ASSET_ROOT="${HO_ASSET_ROOT}"
)

message("\n\n\n######## Processing ${CORE_NAME} module finished. ########\n\n\n")
