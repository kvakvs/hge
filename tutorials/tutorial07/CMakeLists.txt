project( TUT07 )
cmake_minimum_required( VERSION 2.8 )

#------ sources ------
set( TUT07_SRC
    hge_tut07.cpp )

#------ additional includes ------
include_directories( ${HGE_SOURCE_DIR}/include )
include_directories( ${HGE_SOURCE_DIR}/src/libPNG )
include_directories( ${HGE_SOURCE_DIR}/src/zlib-1.2.5 )
include_directories( ${DIRECTX_SDK_DIR}/include )

#------ link directories listed ------
link_directories( ${HGE_SOURCE_DIR}/linklib )
link_directories( ${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x64 ${DIRECTX_SDK_DIR}/lib/x86 )
link_libraries( hge )

#------ target -----
add_executable( hge_tut07 WIN32 ${TUT07_SRC} )

if( MSVC )
    SET_TARGET_PROPERTIES( hge_tut07 PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci" )
endif()
