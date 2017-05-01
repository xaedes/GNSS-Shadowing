# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "${PROJECT_NAME}")
set(MAPPER_APPLICATION_NAME "mapper_run")
set(FOURIER_APPLICATION_NAME "fourierAnalysis_run")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/mapProperties.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/vectorMap.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/shadowMap.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/visibilityMap.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/dopMap.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/types.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/occupancyMap.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/mapper.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/mapperLazyTimesteps.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/mapCoordinateConverter.h"
)
    
set(LIB_SOURCES
	version.cpp
    visibilityMap.cpp
    shadowMap.cpp
    dopMap.cpp
    occupancyMap.cpp
    mapCoordinateConverter.cpp
    mapProperties.cpp
    mapper.cpp
    mapperLazyTimesteps.cpp
)


set(MAPPER_APP_SOURCES
    main.cpp
)

set(FOURIER_APP_SOURCES
    fourierAnalysis.cpp
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
target_link_libraries(${LIBRARY_NAME} sat)
target_link_libraries(${LIBRARY_NAME} shadowing)
target_link_libraries(${LIBRARY_NAME} world)

# This is a make target, so you can do a "make projBrunner"
add_executable(${MAPPER_APPLICATION_NAME} ${MAPPER_APP_SOURCES})
add_executable(${FOURIER_APPLICATION_NAME} ${FOURIER_APP_SOURCES})


# Links the target exe (projBrunner) against the library (libprojB) and other required libraries
target_link_libraries(${MAPPER_APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${OpenCV_LIBS})
target_link_libraries(${FOURIER_APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${OpenCV_LIBS})

include_directories ($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:sat,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:shadowing,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:world,INTERFACE_INCLUDE_DIRECTORIES>)

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)


configure_file("mapperSettings.xml" "${BIN_DIR}/" COPYONLY)
configure_file("fourierSettings.xml" "${BIN_DIR}/" COPYONLY)
