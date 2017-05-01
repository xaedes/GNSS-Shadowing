# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "${PROJECT_NAME}")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/timing.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/math.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/csvRow.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/rotationMatrices.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/setUtils.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/types.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/measureTime.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/simpleStatistics.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/stringUtils.h"
)

set(LIB_SOURCES
    version.cpp
    csvRow.cpp
    timing.cpp
    measureTime.cpp
    stringUtils.cpp
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

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)
