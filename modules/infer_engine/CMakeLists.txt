cmake_minimum_required(VERSION 3.0.0)
project(infer_engine)

message(STATUS "======== BUILD infer_engine ========")
message(STATUS "PLATFORM: ${PLATFORM}")

# ================ 三方库 ================
# OpenCV
list(
    APPEND
    OPENCV_LIBS
    opencv_core
    opencv_imgproc
)

# 后端
set(BACKEND_LIB "")
if (${PLATFORM} STREQUAL "x86" OR ${PLATFORM} STREQUAL "rk")
    set(BACKEND_LIB "tengine-lite-static")
elseif(${PLATFORM} STREQUAL "hisi")
    list(APPEND BACKEND_LIB 
        mpi
        ive
        md
        nnie
        hdmi
        isp
        VoiceEngine
        securec
        upvqe
        dnvqe)
elseif(${PLATFORM} STREQUAL "rv" OR  ${PLATFORM} STREQUAL "rkpro")
    set(BACKEND_LIB "rknn_api")
endif()

# 编译库
include_directories(include)
aux_source_directory(src SRCS)
aux_source_directory(src/engine SRCS)

add_library(infer_engine ${SRCS})
target_link_libraries(infer_engine
    ${OPENCV_LIBS}
    ${BACKEND_LIB}
)