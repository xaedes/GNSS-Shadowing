# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "${PROJECT_NAME}")
set(APPLICATION_NAME "${PROJECT_NAME}_run")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/sat.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/satStore.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/coordinateSystems.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/dop.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/wdop.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/types.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/nmea.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/availableSats.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/satPositionProvider.h"
)

set(LIB_SOURCES
    version.cpp
    sat.cpp
    satStore.cpp
    coordinateSystems.cpp
    dop.cpp
    wdop.cpp
    nmea.cpp
    availableSats.cpp
    satPositionProvider.cpp
)

set(APP_HEADERS
)

set(APP_SOURCES
    main.cpp
)


# Define the library
add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})

# Set the build version. It will be used in the name of the lib, with corresponding
# symlinks created. SOVERSION could also be specified for api version. 
set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${VERSION}  # ${VERSION} was defined in the main CMakeLists.
    FRAMEWORK FALSE
    PUBLIC_HEADER "${LIB_HEADERS}" # specify the public headers
)

# Links the target library against the library (libprojBA)
target_link_libraries(${LIBRARY_NAME} common)
target_link_libraries(${LIBRARY_NAME} sgp4_vallado )
target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBS} )

# This is a make target, so you can do a "make projBrunner"
add_executable(${APPLICATION_NAME} ${APP_SOURCES})

# Links the target exe (projBrunner) against the library (libprojB) and other required libraries
target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} )
target_link_libraries(${APPLICATION_NAME} common)
# target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} )

include_directories ($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:sgp4_vallado,INTERFACE_INCLUDE_DIRECTORIES>)

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)


