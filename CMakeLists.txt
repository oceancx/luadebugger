# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (debugger_all)
include(cmake/cotire.cmake)
include(cmake/HelperMacroFunction.cmake)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

add_subdirectory(luadbg)
add_subdirectory(vscode)
add_subdirectory(test)