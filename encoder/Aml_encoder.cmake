project(Aml_encoder  LANGUAGES CXX)
set(ENCODER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/encoder)
include_directories(${ENCODER_PATH}/inc)
link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(Aml_encoder STATIC
	${ENCODER_PATH}/src/AML_HWEncoder.cpp
	${ENCODER_PATH}/src/enc_api.cpp
	${ENCODER_PATH}/src/gxvenclib_fast.cpp
	${ENCODER_PATH}/src/libvpcodec.cpp
	${ENCODER_PATH}/src/parser.cpp
	${ENCODER_PATH}/src/rate_control_gx_fast.cpp
)
target_link_libraries(Aml_encoder m rt)
