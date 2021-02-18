# Change notes for 1.9

*  Multiple API calls changed some types from `char*` to `const char*`, also blending mode is now using 
   `hgeBlendMode` enum - use type cast like so:
   `spt->SetBlendMode((hgeBlendMode)(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE));`
*  `uint32_t` color argument in many functions and classes is now a simple wrapper class `hgeColor32`.
   To give default clear/draw color, you can use `hgeColor32::WHITE()` and `BLACK()` constexpr functions 
   or create your colors.
*  Multiple improvements to memory safety (strings and vectors where `char *` has a risk of overflow),
   modernising the source (smart pointers, etc), and type safety (replacing generic pointers and ints with
   structs and enums).

# Support & Questions

Documentation and more info http://kvakvs.github.io/hge

Example game can be found here https://github.com/kvakvs/hge_skel

For those times when you absolutely need help, there is the Discord channel: https://discord.gg/TdjamHt

# ANNOUNCEMENT

DirectX8 support has ended. There will be no assistance with DirectX8 
issues, DirectX8 code will be gradually removed. Please use DirectX9.

# INSTALLING

This package is configured with CMakeLists.txt for use with CMake. Once ran,
CMake generates project files for one of known IDE's for Windows (as well as
Linux/Unix/MacOSX but this version of HGE only builds on Windows).

1. Edit CMakeLists.txt in root folder, set "DIRECTX_SDK_DIR" to where you have
   DirectX 9 SDK (see that it comes from microsoft.com, tagged year ~2010, size ~550 MB).
1. Run `cmake_studioNNNN.bat` (NOTE: you can create own bat file if CMake supports your IDE).
1. Project directory will be created in `_build`, open project file (.SLN solution file)
   with your IDE and click "BUILD" in your IDE.
1. Resulting library `HGE.DLL` and HGE.LIB will be placed in `bin` directory, all
   intermediate libraries will be placed in `/linklib`. There is also HGE-STATIC
   library which doesn't require DLL to run, it can be found in `linklib`.
1. All temporary build files will stay inside `_build/`. To clean intermediate files one
   should delete `linklib`, `bin` and `_build` 

## IMPORTANT

1. If you got a 64bit project by mistake, add "-A Win32" to CMake command line. 
   See example in `cmake_studio2019.bat`
1. If you are going to change any build options, please keep in mind, that this is CMake 
   generated project, editing anything inside `_build` folder will be overwritten
   next time, when you compile HGE project. If you need to add any compiler options, please
   edit main or each `CMakeLists.txt` in HGE181 and `src/*` folders (depending how big
   your changes are).
1. if you link against HGE library, you only need to mention HGE.LIB in project settings,
   and have `HGE.DLL` in your program folder. HGEHELPERS is not needed (its linked internally).
1. if you link against HGE-STATIC flavour of library, you don't need `HGE.DLL` anymore, all
   functionality will be included in your application. You need to mention HGEHELPERS, ZLIB 
   and HGELIBPNG in linker settings of your project for this to work (as well as D3D8.lib 
   D3DX8.lib or 9 if you are using DirectX9 and WINMM.lib). Consult project settings of 
   'hge' project regarding Linker input. 
