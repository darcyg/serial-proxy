cmake_minimum_required( VERSION 2.8 )

project( serial-proxy )

set( CMAKE_INSTALL_PREFIX "/" )

set( PACKAGE_MAJOR 0 )
set( PACKAGE_MINOR 1 )
set( PACKAGE_PATCH 0 )

message( "MAJOR: ${PACKAGE_MAJOR}" )
message( "MINOR: ${PACKAGE_MINOR}" )
message( "PATCH: ${PACKAGE_PATCH}" )

add_subdirectory( bin )
