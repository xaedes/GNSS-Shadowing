# @author  Martin Hünermund

# Produce the final Version.h using template Version.h.in and substituting variables.
# We don't want to polute our source tree with it, thus putting it in binary tree.
configure_file("version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)

# define publicly available header files
set(HEADER_FILES
	utils.h
)

# expose header files to other projects
foreach(header ${HEADER_FILES})
	configure_file("${header}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)
endforeach(header)
