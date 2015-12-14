add_definitions( -Wall -O0 -ggdb -std=c++11 )

include_directories( .
                     ${PROJECT_SOURCE_DIR}/src
                     ${PROJECT_BINARY_DIR}
)

set( SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp
             ${PROJECT_SOURCE_DIR}/src/dispatcher.cpp
             ${PROJECT_SOURCE_DIR}/src/proxydevice.cpp
             ${PROJECT_SOURCE_DIR}/src/serialwrapper.cpp
             ${PROJECT_SOURCE_DIR}/src/signalhandler.cpp
             ${PROJECT_SOURCE_DIR}/src/utility.cpp
             ${PROJECT_SOURCE_DIR}/src/serial.cpp
             ${PROJECT_SOURCE_DIR}/src/serialuniximpl.cpp
             ${PROJECT_SOURCE_DIR}/src/virtualdevice.cpp
             ${PROJECT_SOURCE_DIR}/src/configuration.cpp
             ${PROJECT_SOURCE_DIR}/src/exception.cpp
)

add_executable( sproxyd ${SOURCES} )

target_link_libraries( sproxyd -llogcpp -lboost_program_options -linicpp -lutil )

install( TARGETS sproxyd RUNTIME DESTINATION usr/sbin )
