# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "${PROJECT_NAME}")
set(APPLICATION_NAME "${PROJECT_NAME}_run")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/shadowing.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/mesh.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/transformation.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/dataStructures.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/types.h"
)

set(LIB_SOURCES
    version.cpp
    shadowing.cpp
    mesh.cpp
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
target_link_libraries(${LIBRARY_NAME} common obj sat)
target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBS})

# This is a make target, so you can do a "make projBrunner"
add_executable(${APPLICATION_NAME} ${APP_SOURCES})

# Links the target exe (projBrunner) against the library (libprojB) and other required libraries
target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} )

include_directories ($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:obj,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:sat,INTERFACE_INCLUDE_DIRECTORIES>)

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)


