cmake_minimum_required(VERSION 3.9)

project(socklib VERSION 1.0.1 DESCRIPTION "A socket library")

add_library(socklib SHARED
	socklib/internal/clientsock.cpp
	socklib/internal/clientsock.h
	socklib/internal/serversock.cpp
	socklib/internal/serversock.h
	socklib/internal/sockdefs.h
	socklib/internal/socket.cpp
	socklib/internal/socket.h
	socklib/internal/socklib.cpp
	socklib/internal/socklib.h
	socklib/acesock.h
)

set_target_properties(socklib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(socklib PROPERTIES PUBLIC_HEADER socklib/acesock.h)
target_include_directories(socklib PRIVATE socklib/.)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/socklib.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
