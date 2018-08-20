project(Noovo_live  LANGUAGES CXX)
set(LIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/live)
include_directories(${LIVE_PATH}/inc)
link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(Noovo_live STATIC
	${LIVE_PATH}/src/Dvrlock.cpp
	${LIVE_PATH}/src/Adinsert.cpp
	${LIVE_PATH}/src/Audio.cpp
	${LIVE_PATH}/src/CtxWrapper.cpp
	${LIVE_PATH}/src/Video.cpp
	${LIVE_PATH}/src/Transcoder.cpp
	${LIVE_PATH}/src/TransState.cpp
	${LIVE_PATH}/src/Timer.cpp
	${LIVE_PATH}/src/Dvbtlive.cpp
	${LIVE_PATH}/inc/Datastock.hpp
	${LIVE_PATH}/inc/Stream.hpp
)
set( FFMPEG_LIB avformat avfilter avcodec swresample swscale avutil avdevice x264 postproc)
target_link_libraries(Noovo_live m Aml_encoder dl pthread AVL6682 Noovo_dmx ${FFMPEG_LIB})
