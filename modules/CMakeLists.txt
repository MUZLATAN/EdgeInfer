cmake_minimum_required(VERSION 3.0)
project(modules VERSION 1.0.0)

# add_definitions(-w) # close warning specify the C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "======== BUILD modules ========")



# 依赖infer_engine
add_subdirectory(infer_engine)
add_subdirectory(video_engine)




# if (${PLATFORM} STREQUAL "hisi")
#   include_directories(hisi/include/)
#   include_directories(hisi/include/IVE)
#   include_directories(hisi/include/Utils)
#   aux_source_directory(hisi/src/IVE SRCS)
#   aux_source_directory(hisi/src/Utils SRCS)
# elseif (${PLATFORM} STREQUAL "rk" OR ${PLATFORM} STREQUAL "rkpro")
#   add_subdirectory(rk_mpp)

# elseif (${PLATFORM} STREQUAL "RV")
  
# endif()



