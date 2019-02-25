cmake_minimum_required (VERSION 3.10)
project (luadbg VERSION 1.0.0)

find_package(CXLua)

execute_process(COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/generate_inl.sh ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" "include/*.inl" "include/*.lua")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

add_library(luadbg
  ${SOURCES_HEADER_PUB}
  src/debugger.cpp
)

target_include_directories(luadbg
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(luadbg
  PUBLIC CXLua::CXLua
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(luadbg luadbg "" lib/cmake/luadbg)
