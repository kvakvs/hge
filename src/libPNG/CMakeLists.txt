project( HGELIBPNG )
cmake_minimum_required( VERSION 2.8 )

#------ sources ------
set( HGELIBPNG_SRC
    png.c
    pngerror.c
    pnggccrd.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngvcrd.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
    png.h
    pngconf.h    )

#------ additional includes ------
include_directories( ${HGE_SOURCE_DIR}/include )
include_directories( ${HGE_SOURCE_DIR}/src )
include_directories( ${HGE_SOURCE_DIR}/src/zlib-1.2.5 )

#------ link directories listed ------
link_directories( ${HGE_SOURCE_DIR}/linklib )

#------ target -----
add_library( hgelibpng STATIC ${HGELIBPNG_SRC} )
