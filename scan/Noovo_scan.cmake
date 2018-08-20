project(Noovo_scan  LANGUAGES CXX)
set(SCAN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scan)
include_directories(${SCAN_PATH}/inc)
link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(Noovo_scan STATIC
	${SCAN_PATH}/src/AutoScan.cpp
	${SCAN_PATH}/src/Demuxer.cpp
	${SCAN_PATH}/src/noovo_iconv.c
	${SCAN_PATH}/src/unicode.c
	${SCAN_PATH}/src/PMTTable.cpp
	${SCAN_PATH}/src/PATTable.cpp
	${SCAN_PATH}/src/SDTTable.cpp
)
target_link_libraries(Noovo_scan AVL6682 pthread sqlite3 Noovo_dmx)
