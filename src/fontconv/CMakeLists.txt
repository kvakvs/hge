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
link_directories( ${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x64 ${DIRECTX_SDK_DIR}/lib/x86 )

#if( ${HGE_DIRECTX_VER} MATCHES "8" )
#    link_libraries( d3d8 d3dx8 )
#endif()
#if( ${HGE_DIRECTX_VER} MATCHES "9" )
#    link_libraries( d3d9 d3dx9 )
#endif()
#link_libraries( winmm )

link_libraries( zlib hgehelpers hge )

#------ target -----
add_executable( fontconv ${HGE_FONTCONV_SRC} )

if( MSVC )
    SET_TARGET_PROPERTIES( fontconv PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci" )
endif()
