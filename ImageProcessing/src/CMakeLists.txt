file(GLOB SOURCES "*.cpp")
add_library(img_proc SHARED ${SOURCES})

set_target_properties(img_proc PROPERTIES PUBLIC_HEADER ../include/ImageProcessModule.h)
install(TARGETS img_proc 
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
	)

