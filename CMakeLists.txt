## HGE CMake script including HGEHELP and examples
##
project( HGE )
cmake_minimum_required( VERSION 2.8 )

##----------------------------------------
## Edit this to match your configuration
##----------------------------------------

SET( DIRECTX_SDK_DIR "C:/DXSDK" )
##--- set this to 8 or 9 ---
SET( HGE_DIRECTX_VER "9" )
SET( HGE_UNICODE "1" )

##----------------------------------------
## End of manual configuration section
## try to minimize tampering with code below unless you know how CMake works
##----------------------------------------




add_definitions( "-DHGE_DIRECTX_VER=${HGE_DIRECTX_VER}" "-DHGE_UNICODE=${HGE_UNICODE}" )
# disable warnings on STD non conformance and security issues
add_definitions( "-D_CRT_SECURE_NO_WARNINGS" "-D_CRT_NONSTDC_NO_WARNINGS" "-D_SCL_SECURE_NO_WARNINGS" "-D_WIN32_WINNT=0x0501" "/MP" )
## add_definitions( /MDd )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HGE_SOURCE_DIR}/bin CACHE PATH "Single Directory for all executables." )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${HGE_SOURCE_DIR}/bin CACHE PATH "Single Directory for all executables." )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HGE_SOURCE_DIR}/linklib CACHE PATH "Single Directory for all static libraries." )
SET( CMAKE_INCLUDE_CURRENT_DIR ON )


#---------------------------------
add_subdirectory( src/zlib-1.2.5 )
add_subdirectory( src/libPNG )

#--- HGE core library and helpers ---
add_subdirectory( src/helpers )
add_subdirectory( src/core )

#--- HGE TOOLS - comment out of not needed ---
add_subdirectory( src/fontconv )
add_subdirectory( src/fonted )
add_subdirectory( src/particleed )
add_subdirectory( src/pngopt )
add_subdirectory( src/texasm )

#--- HGE examples ---
add_subdirectory( tutorials/tutorial01 )
add_subdirectory( tutorials/tutorial02 )
add_subdirectory( tutorials/tutorial03 )
add_subdirectory( tutorials/tutorial04 )
add_subdirectory( tutorials/tutorial05 )
add_subdirectory( tutorials/tutorial06 )
add_subdirectory( tutorials/tutorial07 )
add_subdirectory( tutorials/tutorial08 )

if( ${HGE_DIRECTX_VER} MATCHES "9" )
    add_subdirectory( tutorials/tutorial05_shaders )
endif()
