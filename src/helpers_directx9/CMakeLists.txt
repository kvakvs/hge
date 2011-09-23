project( HGEHELPERS )
cmake_minimum_required( VERSION 2.8 )

#------ sources ------
set( HGEHELPERS_SRC
    hgeanim.cpp
    hgecolor.cpp
    hgedistort.cpp
    hgefont.cpp
    hgegui.cpp
    hgeguictrls.cpp
    hgeparticle.cpp
    hgepmanager.cpp
    hgerect.cpp
    hgeresource.cpp
    hgesprite.cpp
    hgestrings.cpp
    hgevector.cpp
    parser.cpp
    resources.cpp
    parser.h
    resources.h
    )

#------ additional includes ------
include_directories( ${HGE_SOURCE_DIR}/include )

#------ link directories listed ------
link_directories( ${HGE_SOURCE_DIR}/linklib )

#------ precompiled headers support (not done for now) ------
## if (MSVC)
##     set( PRECOMP_FILENAME "precompiled_headers" )
##     set_source_files_properties( ${PRECOMP_FILENAME}.cpp PROPERTIES COMPILE_FLAGS "/Yc${PRECOMP_FILENAME}.h" )
##     foreach( src_file ${HGEHELPERS_SRC} )
##         set_source_files_properties( ${src_file} PROPERTIES COMPILE_FLAGS "/Yu${PRECOMP_FILENAME}.h" )
##     endforeach( src_file ${HGEHELPERS_SRC} )
##     list( APPEND HGEHELPERS_SRC ${PRECOMP_FILENAME}.cpp ${PRECOMP_FILENAME}.h )
## endif (MSVC)

#------ target -----
add_library( hgehelpers STATIC ${HGEHELPERS_SRC} )
