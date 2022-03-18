cmake_minimum_required(VERSION 3.0)
project(modules VERSION 1.0.0)

# add_definitions(-w) # close warning specify the C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "======== BUILD modules ========")


include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

aux_source_directory(src VIDEOSRCS)


if (${PLATFORM} STREQUAL "hisi")
  include_directories(hisi/include/)
  include_directories(hisi/include/IVE)
  include_directories(hisi/include/Utils)
  aux_source_directory(hisi/src/IVE VIDEOSRCS)
  aux_source_directory(hisi/src/Utils VIDEOSRCS)
elseif (${PLATFORM} STREQUAL "rk" OR ${PLATFORM} STREQUAL "rkpro")
  # add_subdirectory(rk_mpp)
  list(APPEND
        BACKEND_LIB
        #ffmpeg
        avformat avcodec avdevice avfilter  avutil   swresample swscale
        drm
        dl
        x264
        x265
        
        #rock_chip
        rockchip_mpp
        )


elseif (${PLATFORM} STREQUAL "RV")
  
endif()

add_library(video_input ${VIDEOSRCS})
target_link_libraries(video_input
    ${OPENCV_LIBS}
    ${BACKEND_LIB}
)