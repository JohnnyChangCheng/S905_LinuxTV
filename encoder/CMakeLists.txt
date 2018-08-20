cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
set(CMAKE_C_COMPILER   /home/johnny/toolchain/gcc/linux-x86/aarch64/gcc-linaro-aarch64-linux-gnu-4.9/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /home/johnny/toolchain/gcc/linux-x86/aarch64/gcc-linaro-aarch64-linux-gnu-4.9/bin/aarch64-linux-gnu-g++)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS " -DARM -Wall -O3 -Wdeprecated-declarations")
project(Aml_encoder  LANGUAGES CXX)
set(ENCODER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ENCODER_PATH}/inc)
link_directories(${CMAKE_SOURCE_DIR}/../lib)
message(${CMAKE_SOURCE_DIR}/../lib)
include_directories(${CMAKE_SOURCE_DIR}/../include)
add_executable(Aml_encoder 
	${ENCODER_PATH}/src/AML_HWEncoder.cpp
	${ENCODER_PATH}/src/enc_api.cpp
	${ENCODER_PATH}/src/gxvenclib_fast.cpp
	${ENCODER_PATH}/src/libvpcodec.cpp
	${ENCODER_PATH}/src/parser.cpp
    ${ENCODER_PATH}/src/rate_control_gx_fast.cpp
    ${ENCODER_PATH}/test.cpp
)
set(FFMPEG_LIB avformat avfilter avcodec swresample swscale avutil avdevice x264 postproc)
target_link_libraries(Aml_encoder m dl pthread ${FFMPEG_LIB})