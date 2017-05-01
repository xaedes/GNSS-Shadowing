# Function to link between sub-projects
function(add_dependent_subproject subproject_name)
    #if (NOT TARGET ${subproject_name}) # target unknown
    if(NOT PROJECT_${subproject_name}) # var unknown because we build only this subproject, ProjA must have been built AND installed
        find_package(${subproject_name} CONFIG REQUIRED)
    else () # we know the target thus we are doing a build from the top directory
        include_directories(../${subproject_name}/include)
    endif ()
endfunction(add_dependent_subproject)

# Make sure we tell the topdir CMakeLists that we exist (if build from topdir)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(PROJECT_${PROJECT_NAME} true PARENT_SCOPE)
endif()

function(append_file IN_FILE OUT_FILE)
  # http://stackoverflow.com/a/15283110/798588
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

# http://stackoverflow.com/a/27630120/798588
#usage:
# > set(input_list a.cpp b.cpp c.cpp)
# > prepend(output_list "prefix" input_list)
# output_list will contain:
# [prefixa.cpp prefixb.cpp prefixc.cpp]
function(prepend var prefix)
   set(listVar "")
   foreach(f ${ARGN})
      list(APPEND listVar "${prefix}${f}")
   endforeach(f)
   set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)
