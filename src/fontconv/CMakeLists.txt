project( HGE_FONTCONV )
cmake_minimum_required( VERSION 2.8 )

#------ sources ------
set( HGE_FONTCONV_SRC
    fontconv.cpp
    )

#------ additional includes ------
include_directories( ${HGE_SOURCE_DIR}/include )
## include_directories( ${DIRECTX_SDK_DIR}/include )

#------ link directories listed ------
link_directories( ${HGE_SOURCE_DIR}/linklib )
link_directories( ${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x64 )
link_libraries( d3d8 d3dx8 winmm )
link_libraries( zlib hgehelpers hge-static )

#------ target -----
add_executable( fontconv ${HGE_FONTCONV_SRC} )

if( MSVC )
    SET_TARGET_PROPERTIES( fontconv PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci" )
endif()
