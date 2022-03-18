

# 当前目录
CUR_PATH=`pwd`
# 输出根目录
LIB_PATH="dependencies"

# Release or Debug
BUILD_TYPE="Release"

# 输出平台目录
HISI_LIB_PATH="hisi"
RK_LIB_PATH="rk"
RKPRO_LIB_PATH="rkpro"
RV_LIB_PATH="rv"

# 源码存放目录
SRC_PATH="${CUR_PATH}/thirdparty_src"


declare -A LIB_URLS=( 
    ["opencv"]="https://gitee.com/mirrors/opencv.git" 
    ["ffmpeg"]="https://ffmpeg.org/releases/ffmpeg-3.4.9.tar.gz" 
    ["rk_mpp"]="https://github.com/rockchip-linux/mpp.git"
    ["tengine"]="https://github.com/OAID/Tengine/archive/refs/tags/lite-v1.5.tar.gz"

    ["rknn"]="https://github.com/rockchip-linux/rknpu.git"
)



# ================ x264 ================
build_x264()
{
    cd ${SRC_PATH}/x264

    if [[ ${BUILD_TYPE} == "Release" ]];
    then
        ./configure --host=${CONFIGURE_HOST} --prefix=${INSTALL_DIR}/x264 --cross-prefix=${COMPILER_PREFIX} --disable-asm --enable-static --disable-opencl
    else
        ./configure --enable-debug --host=${CONFIGURE_HOST} --prefix=${INSTALL_DIR}/x264 --cross-prefix=${COMPILER_PREFIX} --disable-asm --enable-static --disable-opencl
    fi

    make -j$(nproc) & make install-lib-static
    cd ${CUR_PATH}
}

# ================ x265 ================
build_x265()
{
    cd ${SRC_PATH}/x265
    cd build/arm-linux/
    
    if [[ ${BUILD_PLATFORM} == "rv" ]]; then
        touch rv1126.cmake
        sed -i '1c set(CROSS_COMPILE_ARM 1)' rv1126.cmake
        sed -i '2c set(CMAKE_SYSTEM_NAME Linux)' rv1126.cmake
        sed -i '3c set(CMAKE_SYSTEM_PROCESSOR armv7l)' rv1126.cmake
        sed -i '4c set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)' rv1126.cmake
        sed -i '5c set(CMAKE_C_COMPILER arm-linux-gnueabihf-g++)' rv1126.cmake
        sed -i '6c set(CMAKE_FIND_ROOT_PATH /opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/)' rv1126.cmake

        mkdir -p build
        cd build
        cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=../rv1126.cmake -G "Unix Makefiles" -DENABLE_SHARED=OFF -DENABLE_CLI=OFF -DENABLE_TESTS=OFF -DENABLE_ASSEMBLY=OFF -DENABLE_PIC=ON -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/x265 ../../../source
        make -j$(nproc) & make install
    fi

    cd ${CUR_PATH}
}

build_rk_mpp()
{   cd ${SRC_PATH}/mpp
    mkdir -p build 
    cd ./build
    if [ -f CMakeCache.txt ]; then
        make clean
        rm CMakeCache.txt
    fi
    sed -i "483s%^%# %g" ../CMakeLists.txt
    sed -i "69i\set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})"  ../CMakeLists.txt
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/rk_mpp ${COMPILER} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DRKPLATFORM=ON -DHAVE_DRM=ON  -DBUILD_TEST=OFF -DENABLE_STATIC=ON -DENABLE_SHARED=OFF -DCMAKE_CXX_FLAGS="-I/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/include/drm -L/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/lib" -DCMAKE_INSTALL_BINDIR="bin" -DBUILD_TEST=ON ..
    make -j$(nproc) & make install

    if [ -d ${INSTALL_DIR}/ffmpeg ];
    then
        cp mpp/lib*.so* ${INSTALL_DIR}/ffmpeg/lib/
    fi

    cd ..
    cp mpp/base/inc/* ${INSTALL_DIR}/rk_mpp/include/rockchip
    cp mpp/codec/inc/* ${INSTALL_DIR}/rk_mpp/include/rockchip
    cp mpp/common/* ${INSTALL_DIR}/rk_mpp/include/rockchip
    cp mpp/hal/inc/* ${INSTALL_DIR}/rk_mpp/include/rockchip
    cp mpp/inc/* ${INSTALL_DIR}/rk_mpp/include/rockchip
    cp osal/inc/* ${INSTALL_DIR}/rk_mpp/include/rockchip
    cp utils/utils.h ${INSTALL_DIR}/rk_mpp/include/rockchip

    cd ${CUR_PATH}
}


build_opencv()
{
    cd ${SRC_PATH}/opencv
    if [ -d build ]; then
        rm -rf build
    fi

    sed -i '130c #  if defined(PNG_ARM_NEON) && (defined(__ARM_NEON__) || defined(__ARM_NEON)) && \\' 3rdparty/libpng/pngpriv.h

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${FFMEPG_LIB_PATH}
	export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${FFMPEG_PKG_PATH}
	export PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR:${FFMEPG_LIB_PATH}

    mkdir -p build
    cd build
    
    if [[ ${BUILD_PLATFORM} == "hisi" ]];
    then
        cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/opencv ${COMPILER} -DAR=${AR} -DRANLIB=${RANLIB} -DBUILD_LIST="core,imgproc,highgui,imgcodecs,videoio,video,calib3d,features2d" -DBUILD_SHARED_LIBS=OFF -DBUILD_opencv_apps=OFF -DOPENCV_FORCE_3RDPARTY_BUILD=OFF -DWITH_GTK=OFF -DWITH_IPP=OFF -DBUILD_TESTS=OFF -DWITH_1394=OFF -DBUILD_opencv_apps=OFF -DWITH_ITT=OFF -DBUILD_ZLIB=ON -DWITH_TIFF=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_WEBP=OFF -DCMAKE_CXX_FLAGS="-DCVAPI_EXPORTS -mcpu=${CPU} -mfpu=${FPU}" -DWITH_FFMPEG=ON -DBUILD_JPEG=ON -DBUILD_PNG=ON ..
    else
        cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/opencv ${COMPILER} -DAR=${AR} -DRANLIB=${RANLIB} -DBUILD_LIST="core,imgproc,highgui,imgcodecs,videoio,video,calib3d,features2d" -DBUILD_SHARED_LIBS=OFF -DBUILD_opencv_apps=OFF -DOPENCV_FORCE_3RDPARTY_BUILD=OFF -DWITH_GTK=OFF -DWITH_IPP=OFF -DBUILD_TESTS=OFF -DWITH_1394=OFF -DBUILD_opencv_apps=OFF -DWITH_ITT=OFF -DBUILD_ZLIB=ON -DWITH_TIFF=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_WEBP=OFF -DCMAKE_CXX_FLAGS="-DCVAPI_EXPORTS" -DWITH_FFMPEG=ON -DBUILD_JPEG=ON -DBUILD_PNG=ON ..
    fi
    make -j$(nproc)
    make install
    cp 3rdparty/lib/* ${INSTALL_DIR}/opencv/lib
    cd ${CUR_PATH}
}

# ================ FFMpeg ================
build_ffmpeg()
{
    cd ${SRC_PATH}/ffmpeg
    if [[ ${BUILD_PLATFORM} == "rk" ]];
    then
        cd ${SRC_PATH}
        if [ -d ffmpeg ];
        then
            rm -r ffmpeg
        fi

        wget -c https://whale-cv-video.oss-cn-hangzhou.aliyuncs.com/ffmpeg_rkmpp.tar.gz
        tar zxf ffmpeg_rkmpp.tar.gz
        cp -rf ffmpeg ${INSTALL_DIR}

        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig
    elif [[ ${BUILD_PLATFORM} == "hisi" ]];
    then
        ./configure --prefix=${INSTALL_DIR}/ffmpeg --pkg-config=pkg-config --enable-version3 --enable-ffmpeg --enable-static --enable-pic --disable-ffplay --disable-ffprobe --disable-doc --disable-htmlpages --disable-podpages --disable-txtpages --cross-prefix=${COMPILER_PREFIX} --target-os=linux --enable-cross-compile --arch=arm --enable-nonfree --enable-gpl --enable-lto
        make -j$(nproc)
        make install
        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig
    elif [[ ${BUILD_PLATFORM} == "rkpro" ]];
    then
        export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${INSTALL_DIR}/x264/lib/pkgconfig:${INSTALL_DIR}/x265/lib/pkgconfig:${INSTALL_DIR}/rk_mpp/lib/pkgconfig"
        ./configure --cross-prefix=aarch64-linux-gnu- --enable-cross-compile --target-os=linux --cc=${C_COMPILER} --arch=arm64 --prefix=${INSTALL_DIR}/ffmpeg --disable-x86asm --enable-parsers --disable-decoders --enable-decoder=h264  --enable-decoder=aac --disable-encoders  --disable-muxers  --enable-muxer=mp4 --disable-demuxers --disable-protocols --enable-protocol=file --disable-filters --disable-bsfs --enable-ffmpeg
        make -j$(nproc)
        make install
        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig
    else
        export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${INSTALL_DIR}/x264/lib/pkgconfig:${INSTALL_DIR}/x265/lib/pkgconfig:${INSTALL_DIR}/rk_mpp/lib/pkgconfig"
        ./configure --enable-version3 --enable-gpl --enable-rkmpp --enable-nonfree --enable-libx264  --enable-ffmpeg --enable-static --enable-pic --enable-lto --disable-ffplay --disable-ffprobe --disable-doc --disable-htmlpages --disable-podpages --disable-txtpages --cross-prefix=${COMPILER_PREFIX} --target-os=linux --enable-cross-compile --arch=arm --enable-nonfree --enable-gpl --extra-cflags="-I${INSTALL_DIR}/x264/include -I${INSTALL_DIR}/x265/include -I${INSTALL_DIR}/rk_mpp/include -I/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/include/drm" \
        --extra-ldflags="-L${INSTALL_DIR}/x264/lib -L${INSTALL_DIR}/x265/lib -L${INSTALL_DIR}/rk_mpp/lib -L/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/lib"
        make -j$(nproc)
        make install
        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig

    fi
    
    cd ${CUR_PATH}
}
build_zlib()
{
    cd ${SRC_PATH}/zlib
    ZLIB_ROOT_DIR="${INSTALL_DIR}/zlib"
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd ./build
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${ZLIB_ROOT_DIR} -DAMD64=OFF -DASM686=OFF ${COMPILER} ..
    make -j$(nproc) & make install
    # rm ${ZLIB_ROOT_DIR}/lib/libz.so*
    ZLIB_INCLUDE_DIR="${ZLIB_ROOT_DIR}/include"
    ZLIB_LIBRARY="${ZLIB_ROOT_DIR}/lib/libz.so"
    cd ${CUR_PATH}
}
build_for_rkpro()
{
    echo "================ Build for RKPRO ================"
    INSTALL_DIR="${CUR_PATH}/${LIB_PATH}/${RKPRO_LIB_PATH}/${BUILD_TYPE}"
    COMPILER_PREFIX="/opt/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-"
    C_COMPILER="${COMPILER_PREFIX}gcc"
    CXX_COMPILER="${COMPILER_PREFIX}c++"
    LD="${COMPILER_PREFIX}ld"
    AR="${COMPILER_PREFIX}ar"
    RANLIB="${COMPILER_PREFIX}ranlib"
    COMPILER="-DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DRANDLIB=${RANDLIB}"
    BUILD_PLATFORM="rkpro"
    HOST_SYSTEM_NAME="linux-aarch64"
    CONFIGURE_HOST="aarch64-none-linux"
    TOOLCHAIN_ROOT="$(dirname `which ${C_COMPILER}`)/../aarch64-linux-gnu"
    TOOLCHAIN_INCLUDE="${TOOLCHAIN_ROOT}/include"
    TOOLCHAIN_LIB="${TOOLCHAIN_ROOT}/lib64"
    CPU="cortex-a73"
    FPU="vfpv3"
    CMAKE_SYSTEM_PROCESSOR="armv7-a"

    export LD_LIBRARY_PATH=

    mkdir -p ${INSTALL_DIR}

    # rm -rf ${SRC_PATH}

    # build_rk_mpp
    # build_x264
    # build_x265
    # build_ffmpeg
    # build_opencv
    build_zlib

    echo "================ Build for RKPRO DONE ================"
}


build_for_rkpro