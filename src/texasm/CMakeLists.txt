project(HGE_TEXASM)
cmake_minimum_required(VERSION 2.8)

#------ sources ------
set(HGE_TEXASM_SRC
        optimized_texture.cpp
        RectPlacement.cpp
        sprite_object.cpp
        texasm.cpp
        texture_assembler.cpp
        writePNG.cpp
        gfx_object.h
        optimized_texture.h
        RectPlacement.h
        sprite_object.h
        texasm.h
        texture_assembler.h)

#------ additional includes ------
include_directories(${HGE_SOURCE_DIR}/include)
include_directories(${HGE_SOURCE_DIR}/src/libPNG)
include_directories(${HGE_SOURCE_DIR}/src/zlib-1.2.5)
include_directories(${DIRECTX_SDK_DIR}/include)

#------ link directories listed ------
link_directories(${HGE_SOURCE_DIR}/linklib)
link_directories(${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x86)

link_libraries(winmm)
link_libraries(zlib hgelibpng hge)

#------ target -----
add_executable(texasm ${HGE_TEXASM_SRC})
add_dependencies(texasm hge hge-static hgelibpng)

if (MSVC)
    SET_TARGET_PROPERTIES(texasm PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci")
endif ()
