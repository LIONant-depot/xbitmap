# The library
cmake_minimum_required(VERSION 3.10)

#
# Populate xerr
#
FetchContent_Declare(
  xerr
  GIT_REPOSITORY https://github.com/LIONant-depot/xerr.git
  GIT_TAG        main
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/../dependencies/xerr"  # Adjusted to go up one level
)

FetchContent_GetProperties(xerr)
if(NOT xerr_POPULATED)
  message(STATUS "Populating xerr...")
  FetchContent_MakeAvailable(xerr)
  
  # Include xerr's CMakeLists.txt
  add_subdirectory("../../dependencies/xerr/build/dependency" "${CMAKE_CURRENT_BINARY_DIR}/xerr")
endif()

#
# Define as an interface library only if not already defined
#
if(NOT TARGET xbitmap)
  add_library(xbitmap INTERFACE)
  target_include_directories(xbitmap INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
endif()