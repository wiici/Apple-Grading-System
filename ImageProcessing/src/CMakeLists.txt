file(GLOB SOURCES "*.cpp")
add_library(main_lib SHARED ${SOURCES})

set_target_properties(main_lib PROPERTIES PUBLIC_HEADER ../include/ImageProcessModule.h)
install(TARGETS main_lib 
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
	)

