cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME AncientGreece)
project(${PROJECT_NAME})

# Function to preserve source tree hierarchy of project
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

# Write demo-config.h
message("Generating header file: ${CMAKE_BINARY_DIR}/demo-config.h")
set(CONSOLE_ENABLED_ON_RELEASE 1 CACHE BOOL "Enable console on debug builds")
configure_file(demo-config.h.in "${CMAKE_BINARY_DIR}/demo-config.h")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build_utilities")

# Find assimp
find_package(assimp REQUIRED)
# Find Lua 
find_package(Lua REQUIRED)

# Find SDL2
find_package(SDL2 REQUIRED COMPONENTS main)

# OSX specific packages
if(NOT WIN32)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(OpenGL REQUIRED)    
endif()

# Find glm
set(GLM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/extern/glm-0.9.9.3")

# Find Nlohmann Json
set(JSON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/extern/json-3.5.0")

# Find RapidXML
set(XML_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/extern/rapidxml-1.13")

# Find robin-map
set(ROBIN_MAP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/extern/robin-map/include")

# Define executable target
include_directories(${assimp_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL_MIXER_INCLUDE_DIRS} ${SDL2main_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${CMAKE_BINARY_DIR} ${GLM_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${ROBIN_MAP_INCLUDE_DIR} ${XML_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_DIR
        "*.h"
        "*.cpp"
)    
add_executable(${PROJECT_NAME} ${SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ${assimp_LIBRARIES} ${SDL2_LIBS} ${SDL2_IMAGE_LIBRARIES} ${SDL_MIXER_LIBRARIES} ${OPENGL_LIBRARIES} ${LUA_LIBRARIES})

assign_source_group(${SOURCE_DIR})

# Copy DLLs to output folder on Windows
if(WIN32)		
    foreach(DLL ${assimp_DLLS} ${SDL2_DLLS} ${LUA_DLLS})		
		message("Copying ${DLL} to output folder")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endforeach()
	
endif()

# Enable highest warning levels + treated as errors
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
else(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)
