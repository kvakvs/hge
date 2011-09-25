#pragma once

// CMake adds PROJECTNAME_EXPORTS when compiles DLL
#ifdef hge_EXPORTS
    #define HGEDLL
#endif
//------

#ifndef HGE_EXPORT
    #ifdef HGEDLL
        #define HGE_EXPORT  __declspec(dllexport)
    #else
        #define HGE_EXPORT
    #endif
#endif

#ifndef HGE_CALL
    #define HGE_CALL  __stdcall
#endif
