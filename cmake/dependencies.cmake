# 根据平台增删三方库
set(DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${PLATFORM}")


list(
    APPEND
    DEP_LIBS
    opencv_imgcodecs
    opencv_imgproc
    opencv_core
    opencv_highgui
    dl
    zlib
    libpng
    libjpeg-turbo
    pthread
)


if ("${PLATFORM}" STREQUAL "rk"  )
    add_definitions(-DPNG_ARM_NEON_OPT=2)
    include_directories(${DEPENDENCIES}/rk_mpp/include/rockchip
                        ${DEPENDENCIES}/opencv/include
                        ${DEPENDENCIES}/ffmpeg/include)

    link_directories(${DEPENDENCIES}/rk_mpp/lib
                        ${DEPENDENCIES}/opencv/lib
                        ${DEPENDENCIES}/ffmpeg/lib
                        ${DEPENDENCIES}/x264/lib
                        ${DEPENDENCIES}/zlib/lib
                        )
elseif ("${PLATFORM}" STREQUAL "hisi")
    include_directories(${DEPENDENCIES}/SDL/include/SDL/)
elseif ("${PLATFORM}" STREQUAL "rv" OR "${PLATFORM}" STREQUAL "rkpro")
    add_definitions(-DPNG_ARM_NEON_OPT=2)
    include_directories(
                        ${DEPENDENCIES}/rk_mpp/include/rockchip
                        ${DEPENDENCIES}/ffmpeg/include
                        ${DEPENDENCIES}/opencv/include
                        ${DEPENDENCIES}/rknn/include
                        )

    link_directories(
                        ${DEPENDENCIES}/opencv/lib
                        ${DEPENDENCIES}/ffmpeg/lib
                        ${DEPENDENCIES}/rk_mpp/lib
                        ${DEPENDENCIES}/rknn/lib64
                        )
endif()