cmake_minimum_required(VERSION 3.10)
project(xbmp_unit_test LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Removes the ZERO_CHECK project in the solution
set(CMAKE_SUPPRESS_REGENERATION true)

# Disable installation to avoid cmake_install.cmake
set(CMAKE_SKIP_INSTALL_RULES true)

# Limit build configurations to Debug and Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available configuration types" FORCE)

# Include FetchContent module
include(FetchContent)

# Define the main executable
# This is important to be first so we cann add the target_links to it...
add_executable(xbmp_unit_test "../source/unit_test/main.cpp"
                              "../source/xbitmap.h"
                              "../source/xbitmap.cpp"
                              "../source/xcolor.h"
                              "../source/unit_test/xbitmap_unittest.h"
                              "../source/unit_test/xcolor_unittest.h"
                              "../readme.md"
)

#
# Organize the project a little
#
source_group("unit_test" FILES "../source/unit_test/xbitmap_unittest.h" 
                               "../source/unit_test/xcolor_unittest.h"
)

source_group("xbitmap" FILES "../source/xcolor.h"
                             "../source/xbitmap.cpp"
                             "../source/xbitmap.h"
)

source_group("" FILES "../readme.md"
                      "../source/unit_test/main.cpp"
)

#
# Populate requirements for xbitmap
#
add_subdirectory("dependency" "${CMAKE_CURRENT_BINARY_DIR}/xbitmap")

# add dependencies
target_link_libraries(xbmp_unit_test PRIVATE xerr )

# Set xbmp_unit_test as the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT xbmp_unit_test)