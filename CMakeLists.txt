cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

# needed on top-level CMakeLists.txt
enable_testing() 

# Target doc definition
# The challenge here is that the sub projects must all define a custom target but with different names to avoid
# conflicts. We also want to be able to issue "make doc" from the top dir while being able to issue "make doc"
# in the subdirs *when* we compile only a subproject.
# We define as well a function AddDocTarget in cmake/DoxygenTarget.cmake
#add_custom_target(doc) 
#ADD_DEPENDENCIES(doc docProjA docProjB)

set(Boost_FIND_QUIETLY TRUE)
set(CMAKE_RULE_MESSAGES OFF)

set(BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib")

# add_definitions(-DBENCHMARK)
add_definitions(-DMS_WIN64)

add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(obj)
add_subdirectory(sgp4_vallado)
add_subdirectory(sat)
add_subdirectory(gui)
add_subdirectory(mapping)
add_subdirectory(shadowing)
add_subdirectory(planning)
add_subdirectory(eval)
add_subdirectory(world)
add_subdirectory(pythonBindings)
