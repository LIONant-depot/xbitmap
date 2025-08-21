cmake_minimum_required(VERSION 3.10)
project(xbitmap_unit_test LANGUAGES CXX)

# Set a variable with the target name (visible in subdirectories)
set(TARGET_PROJECT "xbitmap_unit_test")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Removes the ZERO_CHECK project in the solution
set(CMAKE_SUPPRESS_REGENERATION true)

# Disable installation to avoid cmake_install.cmake
set(CMAKE_SKIP_INSTALL_RULES true)

# Limit build configurations to Debug and Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available configuration types" FORCE)

# Initialize a list to track components.
# Using a blank set is fine, CMake list commands will initialize it.
set(COMPONENT_REGISTRY "" )

# Include FetchContent module
include(FetchContent)

# Define the main executable
# This is important to be first so we cann add the target_links to it...
add_executable(xbitmap_unit_test "../source/unit_test/main.cpp"
                                "../source/unit_test/xbitmap_unittest.h"
                                "../source/unit_test/xcolor_unittest.h"
)

#
# Organize the project a little
#
source_group("unit_test" FILES "../source/unit_test/xbitmap_unittest.h" 
                               "../source/unit_test/xcolor_unittest.h"
)

source_group("" FILES "../source/unit_test/main.cpp"
)

#
# Populate requirements for xbitmap
#
add_subdirectory("dependency" "${CMAKE_CURRENT_BINARY_DIR}/xbitmap")

#
# Set the source file into the project as well as organize them in the IDE
#
get_property(COMPONENT_REGISTRY GLOBAL PROPERTY COMPONENT_REGISTRY)
message(STATUS "Processing component registry: ${COMPONENT_REGISTRY}")
foreach(COMPONENT_ID ${COMPONENT_REGISTRY})
    get_property(GROUP GLOBAL PROPERTY ${COMPONENT_ID}_GROUP)
    get_property(FILES GLOBAL PROPERTY ${COMPONENT_ID}_FILES)
    get_property(INCLUDES GLOBAL PROPERTY ${COMPONENT_ID}_INCLUDES)

    if(GROUP AND FILES)
        target_sources(${TARGET_PROJECT} PRIVATE ${FILES})
        target_include_directories(${TARGET_PROJECT} PRIVATE ${INCLUDES})
        source_group("${GROUP}" FILES ${FILES})
    else()
        message(WARNING "Component ${COMPONENT_ID} missing GROUP or FILES properties")
    endif()
endforeach()

#
# Set xbitmap_unit_test as the startup project in Visual Studio
#
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT xbitmap_unit_test)