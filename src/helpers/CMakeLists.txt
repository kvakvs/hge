project(HGEHELPERS)
cmake_minimum_required(VERSION 2.8)

#------ sources ------
set(HGEHELPERS_SRC
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
        ../../../include/hgegui.h
        )

#------ additional includes ------
include_directories(${HGE_SOURCE_DIR}/include)

#------ link directories listed ------
link_directories(${HGE_SOURCE_DIR}/linklib)

#------ target -----
add_library(hgehelpers STATIC ${HGEHELPERS_SRC})
