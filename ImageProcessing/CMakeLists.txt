cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Os -Wall -pedantic")
set(CMAKE_EXE_LINKER_FLAGS "-s ${CIMG_FLAGS}")

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

include_directories(include)


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)
