project(HGE_FONTCONV)
cmake_minimum_required(VERSION 2.8)

#------ sources ------
set(HGE_FONTCONV_SRC
        fontconv.cpp
        )

#------ additional includes ------
include_directories(${HGE_SOURCE_DIR}/include)

#------ link directories listed ------
link_directories(${HGE_SOURCE_DIR}/linklib)
link_directories(${DIRECTX_SDK_DIR}/lib ${DIRECTX_SDK_DIR}/lib/x86)

link_libraries(zlib hgehelpers hge)

#------ target -----
add_executable(fontconv ${HGE_FONTCONV_SRC})
add_dependencies(fontconv hge hge-static)

if (MSVC)
    SET_TARGET_PROPERTIES(fontconv PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci")
endif ()
