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

  # Define xbitmap as a proper static library
  add_library(xbitmap INTERFACE)

  # determine the root of our library
  if("${TARGET_PROJECT}" STREQUAL "xbitmap_unit_test")
    set(ROOT "../..")
  else()
    set(ROOT "../../dependencies/xbitmap")
  endif()

  # set the include path for xbitmap
  set(XBITMAP_INCLUDE_LIST "${ROOT}")

  # set the files required for xbitmap
  set(XBITMAP_FILES_LIST
    "${ROOT}/source/xbitmap.h"
    "${ROOT}/source/xbitmap.cpp"
    "${ROOT}/source/xcolor.h"
    "${ROOT}/README.md"
  )

  # Set global properties
  set_property(GLOBAL PROPERTY XBITMAP_GROUP "dependencies/xcore/xbitmap")
  set_property(GLOBAL PROPERTY XBITMAP_FILES "${XBITMAP_FILES_LIST}")
  set_property(GLOBAL PROPERTY XBITMAP_INCLUDES "${XBITMAP_INCLUDE_LIST}")
  set_property(GLOBAL APPEND PROPERTY COMPONENT_REGISTRY "XBITMAP")

endif()