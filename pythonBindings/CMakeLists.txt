cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake) # main (top) cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir
set(CMAKE_CXX_STANDARD 11) # tODO move up to a general cmake config for all sub projects ?

# CMake useful variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Set the name of your project here
project("pythonBindings")

# Set the version number of your project here (format is MAJOR.MINOR.PATCHLEVEL - e.g. 1.0.0)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

include(Common)

## Git (and its revision)
find_package(Git QUIET) # if we don't find git or FindGit.cmake is not on the system we ignore it.
## GetGitRevisionDescription module to retreive branch and revision information from Git
## Starting with Git 1.9 the module will be part of official cMake distribution, until then it has to be
## part of the application
## The Git module will trigger a reconfiguration for each pull that will bring a new revision on the local repository
set (VCS_REVISION "-1")
if(GIT_FOUND)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    # message(STATUS "GIT branch ${GIT_REFSPEC}")
    # message(STATUS "GIT revision ${GIT_SHA1}")
    set (VCS_REVISION ${GIT_SHA1})
endif()

# PYTHON option

set(PYTHON_OPTIONS "2.X" "3.X")
set(PYTHON_DESIRED_VERSION "3.X" CACHE STRING "Choose which python version to use, options are: ${PYTHON_OPTIONS}.")
set_property(CACHE PYTHON_DESIRED_VERSION PROPERTY STRINGS ${PYTHON_OPTIONS})


# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(OpenCV COMPONENTS core REQUIRED)
include("DetectPython")
if (${PYTHON_DESIRED_VERSION} STREQUAL "2.X")
    set(Python_ADDITIONAL_VERSIONS ${PYTHON2_VERSION_MAJOR}.${PYTHON2_VERSION_MINOR})
    find_package(Boost COMPONENTS python-py${PYTHON2_VERSION_MAJOR}${PYTHON2_VERSION_MINOR} REQUIRED)
else ()
    set(Python_ADDITIONAL_VERSIONS ${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR})
    find_package(Boost COMPONENTS python-py${PYTHON3_VERSION_MAJOR}${PYTHON3_VERSION_MINOR} REQUIRED)
endif ()


#========pick python stuff========================================
if (${PYTHON_DESIRED_VERSION} STREQUAL "2.X")
    SET(PYTHON_INCLUDE_DIRS ${PYTHON2_INCLUDE_PATH} ${PYTHON2_INCLUDE_DIR} ${PYTHON2_INCLUDE_DIR2} ${PYTHON2_NUMPY_INCLUDE_DIRS})
    SET(PYTHON_LIBRARIES ${PYTHON2_LIBRARY})
    SET(PYTHON_EXECUTABLE ${PYTHON2_EXECUTABLE})
    SET(PYTHON_PACKAGES_PATH ${PYTHON2_PACKAGES_PATH})
else ()
    SET(PYTHON_INCLUDE_DIRS ${PYTHON3_INCLUDE_PATH} ${PYTHON3_INCLUDE_DIR} ${PYTHON3_INCLUDE_DIR2} ${PYTHON3_NUMPY_INCLUDE_DIRS})
    SET(PYTHON_LIBRARIES ${PYTHON3_LIBRARY})
    SET(PYTHON_EXECUTABLE ${PYTHON3_EXECUTABLE})
    SET(PYTHON_PACKAGES_PATH ${PYTHON3_PACKAGES_PATH})
endif ()

# # Find ProjA if we are compiling only ProjB otherwise reuse the targets
# add_dependent_subproject(ProjA)
add_dependent_subproject(common)

# This sets the include directory for the reference project. This is the -I flag in gcc.
# All the includes should be in this variable To add new directories to the include path,
# just add them in this list (after a space), or add an include_directories statement in a
# subdirectory's CMakeLists.
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include   # add the binary tree to the search path so that we will find version.h
    ${Boost_INCLUDE_DIRS}
)

# Mac needed variables (adapt for your needs - http://www.cmake.org/Wiki/CMake_RPATH_handling#Mac_OS_X_and_the_RPATH)
# set(CMAKE_MACOSX_RPATH ON)
# set(CMAKE_SKIP_BUILD_RPATH FALSE)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

enable_testing()
add_subdirectory(include)
add_subdirectory(src)
# add_subdirectory(doc)
# add_subdirectory(test)

# 
# SYSTEM PACKAGING (RPM, TGZ, ...)
# _____________________________________________________________________________

# include(CPackConfig)

# 
# CMAKE PACKAGING (for other CMake projects to use this one easily)
# _____________________________________________________________________________

# include(PackageConfigurator)