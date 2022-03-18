# This file is used for cross compilation on hisi chip platform

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

add_definitions("-D__PLATFORM_RK__ -DRK3399 -fopenmp -O2 -ftree-vectorize")