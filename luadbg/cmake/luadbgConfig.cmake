if(NOT TARGET luadbg)
    find_package(CXLua)
    include("${CMAKE_CURRENT_LIST_DIR}/luadbgTargets.cmake")
endif()