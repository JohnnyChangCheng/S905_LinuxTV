project(Noovo_dmx  LANGUAGES C)
set(DMX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dmx)
add_library(Noovo_dmx STATIC
	${DMX_PATH}/src/noovo_dvr.c
	${DMX_PATH}/src/noovo_dmx.c
)
include_directories(${DMX_PATH}/inc)


