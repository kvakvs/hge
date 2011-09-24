project( HGE_FONTEDIT )
cmake_minimum_required( VERSION 2.8 )

#------ sources ------
set( HGE_FONTEDIT_SRC
    commands.cpp
    fonted.cpp
    fontlist.cpp
    generate.cpp
    hgeguirange.cpp
    savepng.cpp
    fonted.h
    fontlist.h
    hgeguirange.h    )

#------ additional includes ------
include_directories( ${HGE_SOURCE_DIR}/include )
include_directories( ${HGE_SOURCE_DIR}/src )
include_directories( ${HGE_SOURCE_DIR}/src/libPNG )
include_directories( ${HGE_SOURCE_DIR}/src/zlib-1.2.5 )

include_directories( ${DIRECTX_SDK_DIR}/include )

#------ link directories listed ------
link_directories( ${HGE_SOURCE_DIR}/linklib )
link_directories( ${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x64 ${DIRECTX_SDK_DIR}/lib/x86 )
#if( ${HGE_DIRECTX_VER} MATCHES "8" )
#    link_libraries( d3d8 d3dx8 )
#endif()
#if( ${HGE_DIRECTX_VER} MATCHES "9" )
#    link_libraries( d3d9 d3dx9 )
#endif()
link_libraries( winmm )
link_libraries( hgelibpng zlib hgehelpers hge )

#------ target -----
add_executable( fonted WIN32 ${HGE_FONTEDIT_SRC} )

if( MSVC )
    SET_TARGET_PROPERTIES( fonted PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci" )
endif()
