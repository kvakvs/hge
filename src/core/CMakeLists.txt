project(HGECORE)
cmake_minimum_required(VERSION 2.8)

#------ sources ------
set(HGECORE_SRC
        demo.cpp
        graphics.cpp
        ini.cpp
        input.cpp
        power.cpp
        random.cpp
        resource.cpp
        sound.cpp
        system.cpp
        timer.cpp
        hge_impl.h
        BASS/bass.h
        ${HGE_SOURCE_DIR}/include/hge.h
        ${HGE_SOURCE_DIR}/include/hgeanim.h
        ${HGE_SOURCE_DIR}/include/hgecolor.h
        ${HGE_SOURCE_DIR}/include/hgedistort.h
        ${HGE_SOURCE_DIR}/include/hgefont.h
        ${HGE_SOURCE_DIR}/include/hgegui.h
        ${HGE_SOURCE_DIR}/include/hgeguictrls.h
        ${HGE_SOURCE_DIR}/include/hgeparticle.h
        ${HGE_SOURCE_DIR}/include/hgerect.h
        ${HGE_SOURCE_DIR}/include/hgeresource.h
        ${HGE_SOURCE_DIR}/include/hgesprite.h
        ${HGE_SOURCE_DIR}/include/hgestrings.h
        ${HGE_SOURCE_DIR}/include/hgevector.h
        ${HGE_SOURCE_DIR}/include/impl/hgegapi.h
        ${HGE_SOURCE_DIR}/include/impl/hgehandle.h
        ${HGE_SOURCE_DIR}/include/impl/hgekeys.h
        ${HGE_SOURCE_DIR}/include/impl/hgedefs.h
        ${HGE_SOURCE_DIR}/include/impl/hgecolor.h
        )

#------ additional includes ------
include_directories(${HGE_SOURCE_DIR}/include)
include_directories(${HGE_SOURCE_DIR}/src/libPNG)
include_directories(${HGE_SOURCE_DIR}/src/zlib-1.2.5)
include_directories(${HGE_SOURCE_DIR}/src/zlib-1.2.5/contrib/minizip)

include_directories(${DIRECTX_SDK_DIR}/include)

#------ link directories listed ------
link_directories(${HGE_SOURCE_DIR}/linklib)
link_directories(${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x86)
link_libraries(d3d9 d3dx9)
link_libraries(winmm)
link_libraries(zlib hgehelpers)

#------ target (for creating HGE.DLL) -----
add_library(hge SHARED ${HGECORE_SRC})
add_dependencies(hge hgehelpers hgelibpng zlib)

#if (MSVC)
#    SET_TARGET_PROPERTIES(hge PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci")
#endif ()

#------ target (for creating static HGE.LIB) -----
add_library(hge-static STATIC ${HGECORE_SRC})
add_dependencies(hge-static hgehelpers hgelibpng)
