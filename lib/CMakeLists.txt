message("\n\n\n######## Start to process ${THIRD_PARTY_NAME} module. ########\n\n\n")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
##########################################################
# Set external libraries name
##########################################################
set(ASSIMP_NAME assimp)
set(IMAGE_LOADER_NAME stb)
set(INI_PARSER_NAME inicpp)

##########################################################
# Download and fetch external libraries
##########################################################

#Declare download targets

FetchContent_Declare(${ASSIMP_NAME}
GIT_REPOSITORY https://github.com/assimp/assimp.git
GIT_TAG master)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(${IMAGE_LOADER_NAME}
GIT_REPOSITORY https://github.com/nothings/stb.git
GIT_TAG master)

FetchContent_Declare(${INI_PARSER_NAME}
GIT_REPOSITORY https://github.com/Rookfighter/inifile-cpp.git
GIT_TAG main)

# FetchContent

message ("#### Start to Download " ${ASSIMP_NAME} "... ####")
FetchContent_MakeAvailable(${ASSIMP_NAME})
message ("#### Downloading " ${ASSIMP_NAME} " finished. ####")

message ("#### Start to Download " ${IMAGE_LOADER_NAME} "... ####")
FetchContent_Populate(${IMAGE_LOADER_NAME})
message ("#### Downloading " ${IMAGE_LOADER_NAME} " finished. ####")

add_library(${IMAGE_LOADER_NAME} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/${IMAGE_LOADER_NAME}.cc)
target_include_directories(${IMAGE_LOADER_NAME} PRIVATE ${${IMAGE_LOADER_NAME}_SOURCE_DIR})

message ("#### Start to Download " ${INI_PARSER_NAME} "... ####")
FetchContent_MakeAvailable(${INI_PARSER_NAME})
message ("#### Downloading " ${INI_PARSER_NAME} " finished ####")

##########################################################
# Add target for raw external libraries
##########################################################
add_library(half INTERFACE)
target_include_directories(half INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
# Set library properties
################################################################################

# Assimp targets
if (TARGET ${ASSIMP_NAME})
    set_target_properties(${ASSIMP_NAME} PROPERTIES FOLDER "ThirdParty/assimp")
endif()

if (TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "ThirdParty/assimp")
endif()

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "ThirdParty/assimp")
endif()

# stb
if (TARGET ${IMAGE_LOADER_NAME})
    set_target_properties(${IMAGE_LOADER_NAME} PROPERTIES FOLDER "ThirdParty/stb")
endif()

# ini parser
if (TARGET ${INI_PARSER_NAME})
    set_target_properties(${INI_PARSER_NAME} PROPERTIES FOLDER "ThirdParty/ini")
endif()

# half
if (TARGET half)
    set_target_properties(half PROPERTIES FOLDER "ThirdParty/half")
endif()

################################################################################
# Add library target
################################################################################
add_library(${THIRD_PARTY_NAME} INTERFACE)
target_link_libraries(${THIRD_PARTY_NAME} INTERFACE ${ASSIMP_NAME} ${IMAGE_LOADER_NAME} inicpp half)
target_include_directories(
  ${THIRD_PARTY_NAME}
  INTERFACE
  ${${ASSIMP_NAME}_SOURCE_DIR}/include
  ${${IMAGE_LOADER_NAME}_SOURCE_DIR}
  ${${INI_PARSER_NAME}_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

message("\n\n\n######## Processing ${THIRD_PARTY_NAME} finished. ########\n\n\n")
