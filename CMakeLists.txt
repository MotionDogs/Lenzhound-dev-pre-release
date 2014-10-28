project(lenzhound)
cmake_minimum_required(VERSION 2.8.7)

include_directories(tools/gtest/include)

add_subdirectory(tools/gtest)
add_subdirectory(test)

set(BOARD arduino:avr:leonardo)

SET(PORT_DESC "USB port to connect to Arduino unit")
if(WIN32)
	set(PORT com3 CACHE STRING PORT_DESC)
else(WIN32)
	set(PORT /dev/ttyACM0 CACHE STRING PORT_DESC)
endif(WIN32)

set(SKETCHBOOK .)
set(OPTS 
	--board ${BOARD} --port ${PORT} --pref sketchbook.path=${SKETCHBOOK})

add_custom_target(verify-rxr
	arduino --verify ${OPTS} ./Rxr/Rxr.ino)
add_custom_target(verify-txr
	arduino --verify ${OPTS} ./Txr/Txr.ino)
add_custom_target(upload-rxr
	arduino --upload ${OPTS} ./Rxr/Rxr.ino)
add_custom_target(upload-txr
	arduino --upload ${OPTS} ./Txr/Txr.ino)
add_custom_target(run-tests
	./test/test
	DEPENDS test)