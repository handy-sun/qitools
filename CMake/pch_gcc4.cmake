# (ADD_PCH_RULE  _header_filename _src_list)
# Version 7/26/2010
#
# use this macro before "add_executable"
#
# _header_filename
#	header to make a .gch
#
# _src_list 
#   the variable name (do not use ${..}) which contains a
#     a list of sources (a.cpp b.cpp c.cpp ...)
#  This macro will append a header file to it, then this src_list can be used in
#	"add_executable..."
#
#
# Now a .gch file should be generated and gcc should use it. 
#       	(add -Winvalid-pch to the cpp flags to verify)
#
# make clean should delete the pch file
#
# example : ADD_PCH_RULE(headers.h myprog_SRCS)
macro(ADD_PCH_RULE  _header_filename _src_list)
    set(_gch_filename "${_header_filename}.gch")
    list(APPEND ${_src_list} ${_gch_filename})
    set(_args ${CMAKE_CXX_FLAGS})
    list(APPEND _args -c ${_header_filename} -o ${_gch_filename})
    get_directory_property(DIRINC INCLUDE_DIRECTORIES)
    foreach(_inc ${DIRINC})
        list(APPEND _args "-I" ${_inc})
    endforeach(_inc ${DIRINC})
    separate_arguments(_args)
    add_custom_command(OUTPUT ${_gch_filename}
        COMMAND rm -f ${_gch_filename}
        COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} ${_args}
        DEPENDS headers.h)
endmacro(ADD_PCH_RULE _src_list _header_filename)
