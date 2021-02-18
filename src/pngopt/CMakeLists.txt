project(HGE_PNGOPT)
cmake_minimum_required(VERSION 2.8)

#------ sources ------
set(HGE_PNGOPT_SRC
        pngopt.cpp
        savepngpitch.cpp
        pngopt.h)

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
add_executable(pngopt ${HGE_PNGOPT_SRC})
add_dependencies(pngopt hge hge-static hgelibpng)

if (MSVC)
    SET_TARGET_PROPERTIES(pngopt PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libci")
endif ()
