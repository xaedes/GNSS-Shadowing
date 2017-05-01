# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "gnssShadowing")
set(APPLICATION_NAME "${PROJECT_NAME}_run")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/arrayBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/cvMatBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/coordinateSystemBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/satBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/worldBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/mappingBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/shadowingBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/objBindings.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/planningBindings.h"
)

set(LIB_SOURCES
	version.cpp
    bindings.cpp
    cv2MatBindings.cpp
    cv3MatBindings.cpp
)

set(APP_HEADERS
)

set(APP_SOURCES
    main.cpp
)

set(PYTHON_SOURCES
    test_coordinateSystemBindings.py
    test_satBindings.py
    test_mappingBindings.py
    test_worldBindings.py
    test_objBindings.py
    test_shadowingBindings.py
    test_planningBindings.py
)


# Define the library
add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})
set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "") # so we dont have to write the prefix when importing in python

#-get proper extension for python binary shared object on this platform
#execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_var('SO'))"
#        RESULT_VARIABLE PYTHON_${LIBRARY_NAME}_PY_PROCESS
#        OUTPUT_VARIABLE ${LIBRARY_NAME}_PY_SUFFIX
#        OUTPUT_STRIP_TRAILING_WHITESPACE)

#set_target_properties(${LIBRARY_NAME} PROPERTIES
#        ARCHIVE_OUTPUT_NAME ${LIBRARY_NAME}  # prevent name conflict for python2/3 outputs
#        PREFIX ""
#        OUTPUT_NAME ${LIBRARY_NAME} 
#        SUFFIX "${${LIBRARY_NAME}_PY_SUFFIX})"


# output library with python bindings to bin directory 
# so we can use it there 
set_target_properties(${LIBRARY_NAME} PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
                      LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
)

foreach(file ${PYTHON_SOURCES})
    configure_file("${file}" "${BIN_DIR}/" COPYONLY)
endforeach(file)


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
target_link_libraries(${LIBRARY_NAME} mapping)
target_link_libraries(${LIBRARY_NAME} world)
target_link_libraries(${LIBRARY_NAME} planning)
target_link_libraries(${LIBRARY_NAME} obj)
target_link_libraries(${LIBRARY_NAME} shadowing)
target_link_libraries(${LIBRARY_NAME} ${PYTHON_LIBRARIES})
target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES})
target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBRARIES})

# This is a make target, so you can do a "make projBrunner"
# add_executable(${APPLICATION_NAME} ${APP_SOURCES})

# Links the target exe (projBrunner) against the library (libprojB) and other required libraries
# target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} )

include_directories($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories($<TARGET_PROPERTY:sat,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories($<TARGET_PROPERTY:mapping,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories($<TARGET_PROPERTY:world,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories($<TARGET_PROPERTY:planning,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories($<TARGET_PROPERTY:obj,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories($<TARGET_PROPERTY:shadowing,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)


