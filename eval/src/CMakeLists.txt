# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME "${PROJECT_NAME}")
set(APPLICATION_NAME_PREFIX "${PROJECT_NAME}_")

set(LIB_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/../include/${PROJECT_NAME}/utils.h"
)

set(LIB_SOURCES
    version.cpp
	utils.cpp
)

set(PROGRAMS
    mean_loc_dop
    mean_loc_dop_against_pred
    plan
    benchmark
    benchmarkLazy
    benchmarkOnlyDS
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
target_link_libraries(${LIBRARY_NAME} mapping)
target_link_libraries(${LIBRARY_NAME} world)

# This is a make target, so you can do a "make projBrunner"

# Links the target exe (projBrunner) against the library (libprojB) and other required libraries

include_directories ($<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:sat,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:mapping,INTERFACE_INCLUDE_DIRECTORIES>)
include_directories ($<TARGET_PROPERTY:world,INTERFACE_INCLUDE_DIRECTORIES>)

foreach(program_name ${PROGRAMS})
    set(APPLICATION_NAME "${APPLICATION_NAME_PREFIX}${program_name}_run")
    add_executable(${APPLICATION_NAME} "${program_name}.cpp")
    target_link_libraries(${APPLICATION_NAME} ${LIBRARY_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} )
endforeach(program_name)

# expose header files to other libraries
target_include_directories(${LIBRARY_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include/
)

configure_file("eval.sh" "${BIN_DIR}/" COPYONLY)
configure_file("eval_pred.sh" "${BIN_DIR}/" COPYONLY)
configure_file("eval_plans.sh" "${BIN_DIR}/" COPYONLY)
configure_file("benchmark.sh" "${BIN_DIR}/" COPYONLY)
configure_file("benchmarkLazy.sh" "${BIN_DIR}/" COPYONLY)
configure_file("plan1.txt" "${BIN_DIR}/" COPYONLY)
configure_file("plan2.txt" "${BIN_DIR}/" COPYONLY)

