# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "${PROJECT_NAME}")
set(APPLICATION_NAME "${PROJECT_NAME}_run")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/sgp4coord.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/sgp4ext.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/sgp4io.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/sgp4unit.h"
)

set(LIB_SOURCES
	version.cpp
    sgp4coord.cpp
    sgp4ext.cpp
    sgp4io.cpp
    sgp4unit.cpp
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
# target_link_libraries(${LIBRARY_NAME} ProjA)

# This is a make target, so you can do a "make projBrunner"
add_executable(${APPLICATION_NAME} ${APP_SOURCES})

# Links the target exe (projBrunner) against the library (libprojB) and other required libraries
target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} )

include_directories ($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME})

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)


