cmake_minimum_required (VERSION 3.10)
project (luadbg VERSION 1.0.0 LANGUAGES C CXX)

# find_package(CXLua)
#find_package(Lua CONFIG)
find_package(EZIO)

execute_process(COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/generate_inl.sh ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp" "src/*.c")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )
file(GLOB_RECURSE  SOURCES_HEADER_PUB "include/*.h" "include/*.inl" "include/*.lua")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

add_library(luadbg SHARED
  ${SOURCES_HEADER_PUB}
  ${SOURCES_HEADER_PRI}
  src/debugger.cpp
  src/lua_cjson.c
  src/strbuf.c
  src/lua_net.cpp
  src/net_thread_queue.cpp
)
set_source_files_properties(lua_cjson.c PROPERTIES LANGUAGE C )
set_source_files_properties(strbuf.c PROPERTIES LANGUAGE C )

target_compile_definitions(luadbg PUBLIC -DUSE_LUA_AS_PROXY)
target_compile_definitions(luadbg PUBLIC -DVERSION="${PROJECT_VERSION}")
target_compile_definitions(luadbg PUBLIC -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1)
target_include_directories(luadbg
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../internals/lua-cmake/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(luadbg
  # PUBLIC CXLua::CXLua
  PUBLIC 
    #Lua::Lua
     EZIO::EZIO
)

include(cmake/HelperMacroFunction.cmake)
export_library_macro(luadbg luadbg "" lib/cmake/luadbg)


set_target_properties(luadbg PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET luadbg
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>/luadbg.dll ${CMAKE_CURRENT_LIST_DIR}/../test
)