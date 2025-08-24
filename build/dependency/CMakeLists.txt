include(${CMAKE_BINARY_DIR}/_deps/xcmake_tools/Common.cmake)

# Fetch xerr
FetchAndPopulate("https://github.com/LIONant-depot/xerr.git" "main")
DefineInterfaceComponent(xbitmap "dependencies/xcore"
  "source/xbitmap.h"
  "source/xbitmap.cpp"
  "source/xcolor.h"
  "README.md"
)