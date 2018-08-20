cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(AVL6682  LANGUAGES CXX)
set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/avl_sdk)
add_library(AVL6682 STATIC
	${SDK_PATH}/src/AVL68XX_Porting.cpp
	${SDK_PATH}/src/AVL_Demod_CommonAPI.cpp
	${SDK_PATH}/src/AVL_Demod_CommonInternal.cpp
	${SDK_PATH}/src/AVL_Demod_DVBC.cpp
	${SDK_PATH}/src/AVL_Demod_DVBSx.cpp
	${SDK_PATH}/src/AVL_Demod_DVBTx.cpp
	${SDK_PATH}/src/AVL_Demod_ISDBT.cpp
	${SDK_PATH}/src/AVL_Demod_Config.cpp
	${SDK_PATH}/src/MxL608.cpp
	${SDK_PATH}/src/MxL608_TunerApi.cpp
	${SDK_PATH}/src/MxL608_TunerCfg.cpp
	${SDK_PATH}/src/MxL608_OEM_Drv.cpp
	${SDK_PATH}/src/MxL608_TunerSpurTable.cpp
	${SDK_PATH}/src/ExtAV2011.cpp
	${SDK_PATH}/src/user_defined_function.cpp 
	${SDK_PATH}/src/AVLDemod.cpp
)
include_directories(${SDK_PATH}/inc)
target_link_libraries(AVL6682 rt dl pthread)


