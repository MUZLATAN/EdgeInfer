cmake_minimum_required(VERSION 3.0)

include(cmake/${PLATFORM}.cmake)
include(cmake/dependencies.cmake)

# C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_VERBOSE_MAKEFILE ON)

project(AIHUBINFER VERSION 1.0)





#编译子模块
add_subdirectory(modules)
include_directories(${CMAKE_CURRENT_LIST_DIR}/modules/infer_engine/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/modules/video_engine/include)
list(APPEND MODULE_LIBS infer_engine video_input) 

#编译目标文件
add_executable(main src/main.cpp )
target_link_libraries(main  ${MODULE_LIBS} ${DEP_LIBS})